/******************************************************************************
 *
 * gbfplain -	SWFilter descendant to strip out all GBF tags or convert to
 *		ASCII rendered symbols.
 */


#include <stdlib.h>
#include <gbfplain.h>

SWORD_NAMESPACE_START

GBFPlain::GBFPlain() {
}


char GBFPlain::processText (SWBuf &text, const SWKey *key, const SWModule *module)
{
	char token[2048];
	int tokpos = 0;
	bool intoken = false;
	SWBuf orig = text;
	const char* from = orig.c_str();
	
	for (text = ""; *from; ++from) {
		if (*from == '<') {
			intoken = true;
			tokpos = 0;
			token[0] = 0;
			token[1] = 0;
			token[2] = 0;
			continue;
		}
		if (*from == '>') {
			intoken = false;
						// process desired tokens
			switch (*token) {
			case 'W':	// Strongs
				switch(token[1]) {
				case 'G':               // Greek
				case 'H':               // Hebrew
				case 'T':               // Tense
					text.append(" <");
					//for (char *tok = token + 2; *tok; tok++)
					//	text += *tok;
					text.append(token+2);
					text.append("> ");
					continue;
				}
				break;
			case 'R':
				switch(token[1]) {
				case 'F':               // footnote begin
					text.append(" [");
					continue;
				case 'f':               // footnote end
					text.append("] ");
					continue;
				}
				break;
			case 'C':
				switch(token[1]) {
				case 'A':               // ASCII value
					text.append((char)atoi(&token[2]));
					continue;
				case 'G':
					text.append('>');
					continue;
/*								Bug in WEB
				case 'L':
					*to++ = '<';
					continue;
*/
				case 'L':	//        Bug in WEB.  Use above entry when fixed
				case 'N':               // new line
					text.append('\n');
					continue;
				case 'M':               // new paragraph
					text.append("\n\n");
					continue;
				}
				break;
			}
			continue;
		}
		if (intoken) {
			if (tokpos < 2045)
				token[tokpos++] = *from;
				token[tokpos+2] = 0;
		}
		else	text.append(*from);
	}
	return 0;
}

SWORD_NAMESPACE_END
