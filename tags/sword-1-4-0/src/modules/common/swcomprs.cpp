/******************************************************************************
 *  swcomprs.cpp   - code for class 'SWCompress'- a driver class that provides
 *				compression utilities.
 */

#include <string.h>
#include <stdlib.h>
#include <swcomprs.h>


/******************************************************************************
 * SWCompress Statics
 */

// m_ring_buffer is a text buffer.  It contains "nodes" of
// uncompressed text that can be indexed by position.  That is,
// a substring of the ring buffer can be indexed by a position
// and a length.  When decoding, the compressed text may contain
// a position in the ring buffer and a count of the number of
// bytes from the ring buffer that are to be moved into the
// uncompressed buffer.  
//
// This ring buffer is not maintained as part of the compressed
// text.  Instead, it is reconstructed dynamically.  That is,
// it starts out empty and gets built as the text is decompressed.
//
// The ring buffer contain N bytes, with an additional F - 1 bytes
// to facilitate string comparison.

unsigned char SWCompress::m_ring_buffer[N + F - 1];

// m_match_position and m_match_length are set by InsertNode().
//
// These variables indicate the position in the ring buffer 
// and the number of characters at that position that match
// a given string.

short int SWCompress::m_match_position;
short int SWCompress::m_match_length;

// m_lson, m_rson, and m_dad are the Japanese way of referring to
// a tree structure.  The dad is the parent and it has a right and
// left son (child).
//
// For i = 0 to N-1, m_rson[i] and m_lson[i] will be the right 
// and left children of node i.  
//
// For i = 0 to N-1, m_dad[i] is the parent of node i.
//
// For i = 0 to 255, rson[N + i + 1] is the root of the tree for 
// strings that begin with the character i.  Note that this requires 
// one byte characters.
//
// These nodes store values of 0...(N-1).  Memory requirements
// can be reduces by using 2-byte integers instead of full 4-byte
// integers (for 32-bit applications).  Therefore, these are 
// defined as "short ints."

short int SWCompress::m_lson[N + 1];
short int SWCompress::m_rson[N + 257];
short int SWCompress::m_dad[N + 1];


/******************************************************************************
 * SWCompress Constructor - Initializes data for instance of SWCompress
 *
 */

SWCompress::SWCompress()
{
	buf = zbuf = 0;
	Init();
}


/******************************************************************************
 * SWCompress Destructor - Cleans up instance of SWCompress
 */

SWCompress::~SWCompress()
{
	if (zbuf)
		free(zbuf);

	if (buf)
		free(buf);
}


void SWCompress::Init()
{
		if (buf)
			free(buf);

		if (zbuf)
			free(zbuf);

		buf    = 0;
		zbuf   = 0;
		direct  = 0;
		zlen    = 0;
		zpos    = 0;
		pos     = 0;
}


char *SWCompress::Buf(char *ibuf)
{
	if (ibuf) {
		Init();
		buf = (char *) malloc(strlen(ibuf) + 1);
		strcpy(buf, ibuf);
	}

	if (!buf)
		Decode();

	return buf;
}


char *SWCompress::zBuf(unsigned short *len, char *ibuf)
{
	if (ibuf) {
		Init();
		zbuf = (char *) malloc(*len);
		memcpy(zbuf, ibuf, *len);
		zlen = *len;
	}

	if (!zbuf)
		Encode();

	*len = (short)zlen;
	return zbuf;
}


int SWCompress::GetChars(char *ibuf, int len)
{
	int slen;

	if (direct) {
		len = (((zlen - zpos) > len) ? len : zlen - zpos);
		if (len > 0) {
			memmove(ibuf, &zbuf[zpos], len);
			zpos += len;
		}
	}
	else {
		slen = strlen(buf);
		len = (((slen - pos) > len) ? len : slen - pos);
		if (len > 0) {
			memmove(ibuf, &buf[pos], len);
			pos += len;
		}
	}
	return len;
}
	

int SWCompress::SendChars(char *ibuf, int len)
{
	int slen;

	if (direct) {
		if (buf) {
			slen = strlen(buf);
			if ((pos + len) > slen) {
				buf = (char *) realloc(buf, pos + len + 1024);
				memset(&buf[pos], 0, len + 1024);
			}
		}
		else	buf = (char *)calloc(1, len + 1024);
		memmove(&buf[pos], ibuf, len);
		pos += len;
	}
	else {
		if (zbuf) {
			if ((zpos + len) > zlen) {
				zbuf = (char *) realloc(zbuf, zpos + len + 1024);
				zlen = zpos + len + 1024;
			}
		}
		else {
			zbuf = (char *)calloc(1, len + 1024);
			zlen = len + 1024;
		}
		memmove(&zbuf[zpos], ibuf, len);
		zpos += len;
	}
	return len;
}


/******************************************************************************
 * SWCompress::InitTree	- This function initializes the tree nodes to "empty"
 *						states. 
 */

void SWCompress::InitTree(void)
{
	int  i;

	// For i = 0 to N - 1, m_rson[i] and m_lson[i] will be the right
	// and left children of node i.  These nodes need not be
	// initialized.  However, for debugging purposes, it is nice to
	// have them initialized.  Since this is only used for compression
	// (not decompression), I don't mind spending the time to do it.
	//
	// For the same range of i, m_dad[i] is the parent of node i.
	// These are initialized to a known value that can represent
	// a "not used" state.
	
	for (i = 0; i < N; i++) {
		m_lson[i] = NOT_USED;
		m_rson[i] = NOT_USED;
		m_dad[i] = NOT_USED;
	}

	// For i = 0 to 255, m_rson[N + i + 1] is the root of the tree
	// for strings that begin with the character i.  This is why
	// the right child array is larger than the left child array.
	// These are also initialzied to a "not used" state.
	//
	// Note that there are 256 of these, one for each of the possible
	// 256 characters.

	for (i = N + 1; i <= (N + 256); i++) {
		m_rson[i] = NOT_USED;
	}
}


/******************************************************************************
 * SWCompress::InsertNode	- This function inserts a string from the ring
 *							buffer into one of the trees.  It loads the
 *							match position and length member variables
 *							for the longest match.
 *	
 *							The string to be inserted is identified by
 *							the parameter Pos, A full F bytes are
 *							inserted.  So,
 *							m_ring_buffer[Pos ... Pos+F-1]
 *							are inserted.
 *
 *							If the matched length is exactly F, then an
 *							old node is removed in favor of the new one
 *							(because the old one will be deleted
 *							sooner).
 *
 *							Note that Pos plays a dual role.  It is
 *							used as both a position in the ring buffer
 *							and also as a tree node.
 *							m_ring_buffer[Pos] defines a character that
 *							is used to identify a tree node.
 *
 * ENT:	pos	- position in the buffer
 */

void SWCompress::InsertNode(short int Pos)
{
	short int i;
	short int p;
	int cmp;
	unsigned char * key;

/*
	ASSERT(Pos >= 0);
	ASSERT(Pos < N);
*/

	cmp = 1;
	key = &(m_ring_buffer[Pos]);

	// The last 256 entries in m_rson contain the root nodes for
	// strings that begin with a letter.  Get an index for the
	// first letter in this string.

	p = (short int) (N + 1 + key[0]);

	// Set the left and right tree nodes for this position to "not
	// used."

	m_lson[Pos] = NOT_USED;
	m_rson[Pos] = NOT_USED;

	// Haven't matched anything yet.

	m_match_length = 0;

	for ( ; ; ) {
		if (cmp >= 0) {
			if (m_rson[p] != NOT_USED) {
				p = m_rson[p];
			}
			else {
				m_rson[p] = Pos;
				m_dad[Pos] = p;
				return;
			}
		}
		else {
			if (m_lson[p] != NOT_USED) {
				p = m_lson[p];
			}
			else {
				m_lson[p] = Pos;
				m_dad[Pos] = p;
				return;
			}
		}

		// Should we go to the right or the left to look for the
		// next match?

		for (i = 1; i < F; i++) {
			cmp = key[i] - m_ring_buffer[p + i];
			if (cmp != 0)
				break;
		}

		if (i > m_match_length) {
			m_match_position = p;
			m_match_length = i;

			if (i >= F)
				break;
		}
	}

	m_dad[Pos] = m_dad[p];
	m_lson[Pos] = m_lson[p];
	m_rson[Pos] = m_rson[p];

	m_dad[ m_lson[p] ] = Pos;
	m_dad[ m_rson[p] ] = Pos;

	if (m_rson[ m_dad[p] ] == p) {
		m_rson[ m_dad[p] ] = Pos;
	}
	else {
		m_lson[ m_dad[p] ] = Pos;
	}

	// Remove "p"

	m_dad[p] = NOT_USED;
}


/******************************************************************************
 * SWCompress::DeleteNode	- This function removes the node "Node" from the
 *							tree.
 *
 * ENT:	node	- node to be removed
 */

void SWCompress::DeleteNode(short int Node)
{
	short int  q;

/*
	ASSERT(Node >= 0);
	ASSERT(Node < (N+1));
*/

	if (m_dad[Node] == NOT_USED) { // not in tree, nothing to do
		return;
	}

	if (m_rson[Node] == NOT_USED) {
		q = m_lson[Node];
	}
	else if (m_lson[Node] == NOT_USED) {
		q = m_rson[Node];
	}
	else {
		q = m_lson[Node];
		if (m_rson[q] != NOT_USED) {
			do {
				q = m_rson[q];
			} while (m_rson[q] != NOT_USED);

			m_rson[ m_dad[q] ] = m_lson[q];
			m_dad[ m_lson[q] ] = m_dad[q];
			m_lson[q] = m_lson[Node];
			m_dad[ m_lson[Node] ] = q;
		}

		m_rson[q] = m_rson[Node];
		m_dad[ m_rson[Node] ] = q;
	}

	m_dad[q] = m_dad[Node];

	if (m_rson[ m_dad[Node] ] == Node) {
		m_rson[ m_dad[Node] ] = q;
	}
	else {
		m_lson[ m_dad[Node] ] = q;
	}

	m_dad[Node] = NOT_USED;
}


/******************************************************************************
 * SWCompress::Encode	- This function "encodes" the input stream into the
 *						output stream.
 *						The GetChars() and SendChars() functions are
 *						used to separate this method from the actual
 *						i/o.
 */

void SWCompress::Encode(void)
{
	short int i;						// an iterator
	short int r;						// node number in the binary tree
	short int s;						// position in the ring buffer
	unsigned short int len;			 // len of initial string
	short int last_match_length;		// length of last match
	short int code_buf_pos;			 // position in the output buffer
	unsigned char code_buf[17];		 // the output buffer
	unsigned char mask;				 // bit mask for byte 0 of out buf
	unsigned char c;					// character read from string

	// Start with a clean tree.

	InitTree();
	direct = 0;	// set direction needed by [Put|Send]Chars()

	// code_buf[0] works as eight flags.  A "1" represents that the
	// unit is an unencoded letter (1 byte), and a "0" represents
	// that the next unit is a <position,length> pair (2 bytes).
	//
	// code_buf[1..16] stores eight units of code.  Since the best
	// we can do is store eight <position,length> pairs, at most 16 
	// bytes are needed to store this.
	//
	// This is why the maximum size of the code buffer is 17 bytes.

	code_buf[0] = 0;
	code_buf_pos = 1;

	// Mask iterates over the 8 bits in the code buffer.  The first
	// character ends up being stored in the low bit.
	//
	//  bit   8   7   6   5   4   3   2   1
	//		|						   |
	//		|			 first sequence in code buffer
	//		|
	//	  last sequence in code buffer		

	mask = 1;

	s = 0;
	r = (short int) N - (short int) F;

	// Initialize the ring buffer with spaces...

	// Note that the last F bytes of the ring buffer are not filled.
	// This is because those F bytes will be filled in immediately
	// with bytes from the input stream.

	memset(m_ring_buffer, ' ', N - F);
	
	// Read F bytes into the last F bytes of the ring buffer.
	//
	// This function loads the buffer with X characters and returns
	// the actual amount loaded.

	len = GetChars((char *) &(m_ring_buffer[r]), F);

	// Make sure there is something to be compressed.

	if (len == 0)
		return;

	// Insert the F strings, each of which begins with one or more
	// 'space' characters.  Note the order in which these strings
	// are inserted.  This way, degenerate trees will be less likely
	// to occur.

	for (i = 1; i <= F; i++) {
		InsertNode((short int) (r - i));
	}

	// Finally, insert the whole string just read.  The
	// member variables match_length and match_position are set.

	InsertNode(r);

	// Now that we're preloaded, continue till done.

	do {

		// m_match_length may be spuriously long near the end of
		// text.

		if (m_match_length > len) {
			m_match_length = len;
		}

		// Is it cheaper to store this as a single character?  If so,
		// make it so.

		if (m_match_length < THRESHOLD) {
			// Send one character.  Remember that code_buf[0] is the
			// set of flags for the next eight items.

			m_match_length = 1;	 
			code_buf[0] |= mask;  
			code_buf[code_buf_pos++] = m_ring_buffer[r];
		}

		// Otherwise, we do indeed have a string that can be stored
		// compressed to save space.

		else {
			// The next 16 bits need to contain the position (12 bits)
			// and the length (4 bits).

			code_buf[code_buf_pos++] = (unsigned char) m_match_position;
			code_buf[code_buf_pos++] = (unsigned char) (
				((m_match_position >> 4) & 0xf0) | 
				(m_match_length - THRESHOLD) );
		}

		// Shift the mask one bit to the left so that it will be ready
		// to store the new bit.

		mask = (unsigned char) (mask << 1);

		// If the mask is now 0, then we know that we have a full set
		// of flags and items in the code buffer.  These need to be
		// output.

		if (!mask) {
			// code_buf is the buffer of characters to be output.
			// code_buf_pos is the number of characters it contains.

			SendChars((char *) code_buf, code_buf_pos);

			// Reset for next buffer...

			code_buf[0] = 0;
			code_buf_pos = 1;
			mask = 1;
		}

		last_match_length = m_match_length;

		// Delete old strings and read new bytes...

		for (i = 0; i < last_match_length; i++) {
			// Get next character...

			if (GetChars((char *) &c, 1) != 1)
				break;

			// Delete "old strings"

			DeleteNode(s);

			// Put this character into the ring buffer.
			//		  
			// The original comment here says "If the position is near
			// the end of the buffer, extend the buffer to make
			// string comparison easier."
			//
			// That's a little misleading, because the "end" of the 
			// buffer is really what we consider to be the "beginning"
			// of the buffer, that is, positions 0 through F.
			//
			// The idea is that the front end of the buffer is duplicated
			// into the back end so that when you're looking at characters
			// at the back end of the buffer, you can index ahead (beyond
			// the normal end of the buffer) and see the characters
			// that are at the front end of the buffer wihtout having
			// to adjust the index.
			//
			// That is...
			//
			//	  1234xxxxxxxxxxxxxxxxxxxxxxxxxxxxx1234
			//	  |							   |  |
			//	  position 0		  end of buffer  |
			//										 |
			//				  duplicate of front of buffer

			m_ring_buffer[s] = c;

			if (s < F - 1) {
				m_ring_buffer[s + N] = c;
			}

			// Increment the position, and wrap around when we're at
			// the end.  Note that this relies on N being a power of 2.

			s = (short int) ( (s + 1) & (N - 1) );
			r = (short int) ( (r + 1) & (N - 1) );

			// Register the string that is found in 
			// m_ring_buffer[r..r+F-1].

			InsertNode(r);
		}

		// If we didn't quit because we hit the last_match_length,
		// then we must have quit because we ran out of characters
		// to process.

		while (i++ < last_match_length) {							  
			DeleteNode(s);

			s = (short int) ( (s + 1) & (N - 1) );
			r = (short int) ( (r + 1) & (N - 1) );

			// Note that len hitting 0 is the key that causes the
			// do...while() to terminate.  This is the only place
			// within the loop that len is modified.
			//
			// Its original value is F (or a number less than F for
			// short strings).

			if (--len) {
				InsertNode(r);	   /* buffer may not be empty. */
			}
		}

		// End of do...while() loop.  Continue processing until there
		// are no more characters to be compressed.  The variable
		// "len" is used to signal this condition.
	} while (len > 0);

	// There could still be something in the output buffer.  Send it
	// now.

	if (code_buf_pos > 1) {
		// code_buf is the encoded string to send.
		// code_buf_ptr is the number of characters.

		SendChars((char *) code_buf, code_buf_pos);
	}
	zlen = zpos;
}


/******************************************************************************
 * SWCompress::Decode	- This function "decodes" the input stream into the
 *						output stream.
 *						The GetChars() and SendChars() functions are
 *						used to separate this method from the actual
 *						i/o.
 */

void SWCompress::Decode(void)
{
	int k;
	int r;							  // node number
	unsigned char c[F];				 // an array of chars
	unsigned char flags;				// 8 bits of flags
	int flag_count;					 // which flag we're on
	short int pos;					  // position in the ring buffer
	short int len;					  // number of chars in ring buffer

	direct = 1;	// set direction needed by [Put|Send]Chars()

	// Initialize the ring buffer with a common string.
	//
	// Note that the last F bytes of the ring buffer are not filled.

	memset(m_ring_buffer, ' ', N - F);
	
	r = N - F;

	flags = (char) 0;
	flag_count = 0;

	for ( ; ; ) {

		// If there are more bits of interest in this flag, then
		// shift that next interesting bit into the 1's position.
		//
		// If this flag has been exhausted, the next byte must 
		// be a flag.

		if (flag_count > 0) {
			flags = (unsigned char) (flags >> 1);
			flag_count--;
		}
		else {
			// Next byte must be a flag.

			if (GetChars((char *) &flags, 1) != 1)
				break;

			// Set the flag counter.  While at first it might appear
			// that this should be an 8 since there are 8 bits in the
			// flag, it should really be a 7 because the shift must
			// be performed 7 times in order to see all 8 bits.

			flag_count = 7;
		}

		// If the low order bit of the flag is now set, then we know
		// that the next byte is a single, unencoded character.

		if (flags & 1) {
			if (GetChars((char *) c, 1) != 1)
				break;

			if (SendChars((char *) c, 1) != 1)
				break;

			// Add to buffer, and increment to next spot. Wrap at end.

			m_ring_buffer[r] = c[0];
			r = (short int) ( (r + 1) & (N - 1) );
		}

		// Otherwise, we know that the next two bytes are a
		// <position,length> pair.  The position is in 12 bits and
		// the length is in 4 bits.

		else {
			// Original code:
			//  if ((i = getc(infile)) == EOF)
			//	  break;
			//  if ((j = getc(infile)) == EOF)
			//	  break;
			//  i |= ((j & 0xf0) << 4);	
			//  j = (j & 0x0f) + THRESHOLD;
			//
			// I've modified this to only make one input call, and
			// have changed the variable names to something more
			// obvious.

			if (GetChars((char *) c, 2) != 2)
				break;

			// Convert these two characters into the position and
			// length.  Note that the length is always at least
			// THRESHOLD, which is why we're able to get a length
			// of 18 out of only 4 bits.

			pos = (short int) ( c[0] | ((c[1] & 0xf0) << 4) );

			len = (short int) ( (c[1] & 0x0f) + THRESHOLD );

			// There are now "len" characters at position "pos" in
			// the ring buffer that can be pulled out.  Note that
			// len is never more than F.

			for (k = 0; k < len; k++) {
				c[k] = m_ring_buffer[(pos + k) & (N - 1)];

				// Add to buffer, and increment to next spot. Wrap at end.

				m_ring_buffer[r] = c[k];
				r = (short int) ( (r + 1) & (N - 1) );
			}

			// Add the "len" characters to the output stream.

			if (SendChars((char *) c, len) != len)
				break;
		}
	}
}