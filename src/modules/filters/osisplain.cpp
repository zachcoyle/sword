/***************************************************************************
                     osisplain.cpp  -  OSIS to Plaintext filter
                             -------------------
    begin                : 2003-02-15
    copyright            : 2003 by CrossWire Bible Society
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdlib.h>
#include <osisplain.h>
#include <utilxml.h>
#include <ctype.h>

SWORD_NAMESPACE_START

OSISPlain::OSISPlain() {
	setTokenStart("<");
	setTokenEnd(">");
  
	setEscapeStart("&");
	setEscapeEnd(";");
  
	setEscapeStringCaseSensitive(true);
  
	addEscapeStringSubstitute("amp", "&");
	addEscapeStringSubstitute("apos", "'");
	addEscapeStringSubstitute("lt", "<");
	addEscapeStringSubstitute("gt", ">");
	addEscapeStringSubstitute("quot", "\"");
  
	setTokenCaseSensitive(true);  
}

char OSISPlain::processText(SWBuf &text, const SWKey *key, const SWModule *module)
{
        SWBasicFilter::processText(text, key, module);  //handle tokens as usual
	const char *from;
	SWBuf orig = text;
	from = orig.c_str();
	for (text = ""; *from; from++) {  //loop to remove extra spaces
                if ((strchr(" \t\n\r", *from))) {
                        while (*(from+1) && (strchr(" \t\n\r", *(from+1)))) {
                                from++;
                        }
                        text += " ";
                }
                else {
                        text += *from;
                }
        }
        text += (char)0;
        return 0;
}

bool OSISPlain::handleToken(SWBuf &buf, const char *token, DualStringMap &userData) {
  // manually process if it wasn't a simple substitution
	if (!substituteToken(buf, token)) {
		if (((*token == 'w') && (token[1] == ' ')) ||
		((*token == '/') && (token[1] == 'w') && (!token[2]))) {
			bool start = false;
			if (*token == 'w') {
				if (token[strlen(token)-1] != '/') {
					userData["w"] = token;
					return true;
				}
				start = true;
			}
			XMLTag tag = (start) ? token : userData["w"].c_str();
			bool show = true;	// to handle unplaced article in kjv2003-- temporary till combined

			SWBuf lastText = (start) ? "stuff" : userData["lastTextNode"].c_str();
					
			const char *attrib;
			const char *val;
			if (attrib = tag.getAttribute("xlit")) {
				val = strchr(attrib, ':');
				val = (val) ? (val + 1) : attrib;
				buf.appendFormatted(" <%s>", val);
			}
			if (attrib = tag.getAttribute("gloss")) {
				val = strchr(attrib, ':');
				val = (val) ? (val + 1) : attrib;
				buf.appendFormatted(" <%s>", val);
			}
			if (attrib = tag.getAttribute("lemma")) {
				int count = tag.getAttributePartCount("lemma");
				int i = (count > 1) ? 0 : -1;		// -1 for whole value cuz it's faster, but does the same thing as 0
				do {
					attrib = tag.getAttribute("lemma", i);
					if (i < 0) i = 0;	// to handle our -1 condition
					val = strchr(attrib, ':');
					val = (val) ? (val + 1) : attrib;
					if ((strchr("GH", *val)) && (isdigit(val[1])))
						val++;
					if ((!strcmp(val, "3588")) && (lastText.length() < 1))
						show = false;
					else	buf.appendFormatted(" <%s>}", val);
				} while (++i < count);
			}
			if ((attrib = tag.getAttribute("morph")) && (show)) {
				int count = tag.getAttributePartCount("morph");
				int i = (count > 1) ? 0 : -1;		// -1 for whole value cuz it's faster, but does the same thing as 0
				do {
					attrib = tag.getAttribute("morph", i);
					if (i < 0) i = 0;	// to handle our -1 condition
					val = strchr(attrib, ':');
					val = (val) ? (val + 1) : attrib;
					if ((*val == 'T') && (strchr("GH", val[1])) && (isdigit(val[2])))
						val+=2;
					buf.appendFormatted(" (%s)", val);
				} while (++i < count);
			}
			if (attrib = tag.getAttribute("POS")) {
				val = strchr(attrib, ':');
				val = (val) ? (val + 1) : attrib;
				buf.appendFormatted(" <%s>", val);
			}
		}

		// <note> tag
		else if (!strncmp(token, "note", 4)) {
				if (!strstr(token, "strongsMarkup")) {	// leave strong's markup notes out, in the future we'll probably have different option filters to turn different note types on or off
					buf += " (";
				}
				else	userData["suspendTextPassThru"] = "true";
			}
		else if (!strncmp(token, "/note", 5)) {
			if (userData["suspendTextPassThru"] == "false")
				buf += ")";
			else	userData["suspendTextPassThru"] = "false";
		}

		// <p> paragraph tag
		else if (((*token == 'p') && ((token[1] == ' ') || (!token[1]))) ||
			((*token == '/') && (token[1] == 'p') && (!token[2]))) {
				buf += "\n";
		}

		// <line> poetry, etc
		else if ((!strncmp(token, "line", 4)) ||
			(!strncmp(token, "/line", 5))) {
				buf += "\n";
		}

		// <title>
		else if (!strncmp(token, "/title", 6)) {
			buf += "\n";
		}

		else {
			return false;  // we still didn't handle token
		}
	}
	return true;
}


SWORD_NAMESPACE_END
