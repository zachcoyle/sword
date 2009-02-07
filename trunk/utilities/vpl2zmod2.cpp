/*
 * Copyright 2009 CrossWire Bible Society (http://www.crosswire.org)
 *	CrossWire Bible Society
 *	P. O. Box 2528
 *	Tempe, AZ  85280-2528
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation version 2.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 */

#include <ctype.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>

#ifndef __GNUC__
#include <io.h>
#else
#include <unistd.h>
#endif

#include <swmgr.h>
#include <ztext2.h>
#include <iostream>
#include <swbuf.h>
#include <lzsscomprs.h>
#include <zipcomprs.h>
#include <versekey2.h>

#ifndef NO_SWORD_NAMESPACE
using sword::SWMgr;
using sword::SWBuf;
using sword::zText2;
using sword::VerseKey2;
using sword::SW_POSITION;
using sword::SWCompress;
//using sword::CipherFilter;
using sword::zText2;
using sword::ZipCompress;
using sword::LZSSCompress;
#endif

using std::string;

char readline(int fd, char **buf) {
	char ch;
	if (*buf)
		delete [] *buf;
	*buf = 0;
	int len;


	long index = lseek(fd, 0, SEEK_CUR);
	// clean up any preceding white space
	while ((len = read(fd, &ch, 1)) == 1) {
		if ((ch != 13) && (ch != ' ') && (ch != '\t'))
			break;
		else index++;
	}


	while (ch != 10) {
        if ((len = read(fd, &ch, 1)) != 1)
			break;
	}
	
	int size = (lseek(fd, 0, SEEK_CUR) - index) - 1;

	*buf = new char [ size + 1 ];

	if (size > 0) {
		lseek(fd, index, SEEK_SET);
		read(fd, *buf, size);
		read(fd, &ch, 1);   //pop terminating char
		(*buf)[size] = 0;

		// clean up any trailing junk on buf
		for (char *it = *buf+(strlen(*buf)-1); it > *buf; it--) {
			if ((*it != 10) && (*it != 13) && (*it != ' ') && (*it != '\t'))
				break;
			else *it = 0;
		}
	}
	else **buf = 0;
	return !len;
}


char *parseVReg(char *buf) {
	char stage = 0;

	while (*buf) {
		switch (stage) {
		case 0:
			if (isalpha(*buf))
				stage++;
			break;
		case 1:
			if (isdigit(*buf))
				stage++;
			break;
		case 2:
			if (*buf == ':')
				stage++;
			break;
		case 3:
			if (isdigit(*buf))
				stage++;
			break;
	   case 4:
			if (*buf == ' ') {
				*buf = 0;
				return ++buf;
			}
			break;
		}
		buf++;
	}
	return (stage == 4) ? buf : 0;  // if we got to stage 4 return after key buf, else return 0;
}


bool isKJVRef(const char *buf) {
	VerseKey2 vk, test;
	vk.AutoNormalize(0);
	vk.Headings(1);	// turn on mod/testmnt/book/chap headings
	vk.Persist(1);
	// lets do some tests on the verse --------------
	vk = buf;
	test = buf;

	if (vk.Book() && vk.Chapter() && vk.Verse()) { // if we're not a heading
		//std::cerr << (const char*)vk << " == "  << (const char*)test << std::endl;
		return (vk == test);
	}
	else return true;	// no check if we're a heading... Probably bad.
}


void fixText(char *text) {
	char *to = text;
	while(*text) {
		*to++ = *text++;
		*to++ = *text++;
		if (!*text)
			break;
		if (*text != ' ')
			std::cerr << "problem\n";
		else	text++;
	}
	*to = 0;
}

int main(int argc, char **argv) {
	int iType = 3;
	int iPerBlock = 7;
	int compType = 2;
	SWBuf cipherKey = "";
	SWCompress *compressor = 0;

	// Let's test our command line arguments
	if (argc < 2) {
//		fprintf(stderr, "usage: %s <vpl_file> </path/to/mod> [0|1 - file includes prepended verse references]\n", argv[0]);
		fprintf(stderr, "usage: %s <source_vpl_file> </path/to/output/mod/> [0|1 - prepended verse refs] [0|1 - NT only]\n\n", argv[0]);
		fprintf(stderr, "\tWith no verse refs, source file must contain exactly 31102 lines.\n");
		fprintf(stderr, "\tThis is KJV verse count plus headings for MODULE,\n");
		fprintf(stderr, "\tTESTAMENT, BOOK, CHAPTER. An example snippet follows:\n\n");
		fprintf(stderr, "\t\tMODULE HEADER\n");
		fprintf(stderr, "\t\tOLD TESTAMENT HEADER\n");
		fprintf(stderr, "\t\tGENESIS HEADER\n");
		fprintf(stderr, "\t\tCHAPTER 1 HEADER\n");
		fprintf(stderr, "\t\tIn the beginning...\n\n");
		fprintf(stderr, "\t... implying there must also be a CHAPTER2 HEADER,\n");
        fprintf(stderr, "\tEXODUS HEADER, NEW TESTAMENT HEADER, etc.  If there is no text for\n");
		fprintf(stderr, "\tthe header, a blank line must, at least, hold place.\n\n");
		fprintf(stderr, "\tWith verse refs, source file must simply contain any number of lines,\n");
		fprintf(stderr, "\tthat begin with the verse reference for which it is an entry.  e.g.:\n\n");
		fprintf(stderr, "\t\tgen 1:0 CHAPTER 1 HEADER\n");
		fprintf(stderr, "\t\tgen 1:1 In the beginning...\n\n");
		exit(-1);
	}

	// Let's see if we can open our input file
	int fd = FileMgr::openFileReadOnly(argv[1]);
	if (fd < 0) {
		fprintf(stderr, "error: %s: couldn't open input file: %s \n", argv[0], argv[1]);
		exit(-2);
	}

	// not used yet, but for future support of a vpl file with each line
	// prepended with verse reference, eg. "Gen 1:1 In the beginning..."
	bool vref = false;

	// if 'nt' is the 4th arg, our vpl file only has the NT
	bool ntonly = false;

	if (argc > 3) {
		vref = (argv[3][0] == '0') ? false : true;
		if (argc > 4) {
			ntonly = (argv[4][0] == '0') ? false : true;
			if (argc > 5) {
				iType = atoi(argv[5]);
				if (argc > 6) {
					iPerBlock = atoi(argv[6]);
					if (argc > 7) {
						compType = atoi(argv[7]);
						if (argc > 8) {
							cipherKey = argv[8];
						}
					}
				}
			}
		}
	}

	// Try to initialize a default set of datafiles and indicies at our
	// datapath location passed to us from the user.
	if (zText2::createModule(argv[2], iType)) {
		fprintf(stderr, "error: %s: couldn't create module at path: %s \n", argv[0], argv[2]);
		exit(-3);
	}
	switch (compType) {	// these are deleted by zText
	case 1: compressor = new LZSSCompress(); break;
	case 2: compressor = new ZipCompress(); break;
	}

	// Do some initialization stuff
	char *buffer = 0;
	zText2 mod(argv[2], 0, 0, iType, iPerBlock, compressor);	// open our datapath with our zText2 driver.
	VerseKey2 vk;
	vk.AutoNormalize(0);
	vk.Headings(1);	// turn on mod/testmnt/book/chap headings
	vk.Persist(1);

	mod.setKey(vk);

	// Loop through module from TOP to BOTTOM and set next line from
	// input file as text for this entry in the module
	mod = TOP;
	if (ntonly) vk = "Matthew 1:1";
	  
	int successive = 0;  //part of hack below
	while ((!mod.Error()) && (!readline(fd, &buffer))) {
		if (*buffer == '|')	// comments, ignore line
			continue;
		if (vref) {
			const char *verseText = parseVReg(buffer);
			if (!verseText) {	// if we didn't find a valid verse ref
				std::cerr << "No valid verse ref found on line: " << buffer << "\n";
				exit(-4);
			}

			vk = buffer;
			if (vk.Error()) {
				std::cerr << "Error parsing key: " << buffer << "\n";
				exit(-5);
			}
			string orig = mod.getRawEntry();

			if (!isKJVRef(buffer)) {
				VerseKey2 origVK = vk;
				/* This block is functioning improperly -- problem with AutoNormalize???
				do {
					vk--;
				}
				while (!vk.Error() && !isKJVRef(vk)); */
				//hack to replace above:
				successive++;
				vk -= successive;
				orig = mod.getRawEntry();

				std::cerr << "Not a valid KJV ref: " << origVK << "\n";
				std::cerr << "appending to ref: " << vk << "\n";
				orig += " [ (";
				orig += origVK;
				orig += ") ";
				orig += verseText;
				orig += " ] ";
				verseText = orig.c_str();
			}
			else {
			  successive = 0;
			}

			if (orig.length() > 1)
				   std::cerr << "Warning, overwriting verse: " << vk << std::endl;
			  
			// ------------- End verse tests -----------------
			mod << verseText;	// save text to module at current position
		}
		else {
			fixText(buffer);
			mod << buffer;	// save text to module at current position
			mod++;	// increment module position
		}
	}

	// clear up our buffer that readline might have allocated
	if (buffer)
		delete [] buffer;
}