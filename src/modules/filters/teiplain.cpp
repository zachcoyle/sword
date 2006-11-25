/***************************************************************************
                     teiplain.cpp  -  TEI to Plaintext filter
                             -------------------
    begin                : 2006-07-05
    copyright            : 2006 by CrossWire Bible Society
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
#include <teiplain.h>
#include <ctype.h>

SWORD_NAMESPACE_START

TEIPlain::TEIPlain() {
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


bool TEIPlain::handleToken(SWBuf &buf, const char *token, BasicFilterUserData *userData) {
  // manually process if it wasn't a simple substitution
	if (!substituteToken(buf, token)) {
		//MyUserData *u = (MyUserData *)userData;
		XMLTag tag(token);

		// <p> paragraph tag
		if (!strcmp(tag.getName(), "p")) {
			if ((!tag.isEndTag()) && (!tag.isEmpty())) {	// non-empty start tag
				buf += "\n";
			}
			else if (tag.isEndTag()) {	// end tag
				buf += "\n";
				userData->supressAdjacentWhitespace = true;
			}
			else {					// empty paragraph break marker
				buf += "\n\n";
				userData->supressAdjacentWhitespace = true;
			}
		}

		// <entryFree>
		else if (!strcmp(tag.getName(), "entryFree")) {
			SWBuf n = tag.getAttribute("n");
			if ((!tag.isEndTag()) && (!tag.isEmpty())) {
			        if (n != "") {
					buf += n;
					buf += ". ";
				}
			}
		}

		// <sense>
		else if (!strcmp(tag.getName(), "sense")) {
			SWBuf n = tag.getAttribute("n");
			if ((!tag.isEndTag()) && (!tag.isEmpty())) {
			        if (n != "") {
					buf += n;
					buf += ". ";
				}
			}
			else if (tag.isEndTag()) {
			                buf += "\n";
			}
		}

		// <div>
		else if (!strcmp(tag.getName(), "div")) {

			if ((!tag.isEndTag()) && (!tag.isEmpty())) {
				buf.append("\n\n\n");
			}
			else if (tag.isEndTag()) {
			}
		}

		// <etym>
		else if (!strcmp(tag.getName(), "etym")) {
			if ((!tag.isEndTag()) && (!tag.isEmpty())) {
				buf += "[";
			}
			else if (tag.isEndTag()) {
			        buf += "]";
			}
		}

		else {
			return false;  // we still didn't handle token
		}
	}
	return true;
}


SWORD_NAMESPACE_END
