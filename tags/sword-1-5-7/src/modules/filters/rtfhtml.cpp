/***************************************************************************
                          rtfhtml.cpp  -  description
                             -------------------
    begin                : Wed Oct 13 1999
    copyright            : (C) 1999 by The team of BibleTime
    email                : info@bibletime.de
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
#include <rtfhtml.h>

SWORD_NAMESPACE_START

RTFHTML::RTFHTML() {

}


char RTFHTML::processText(SWBuf &text, const SWKey *key, const SWModule *module)
{
	bool center = false;

	const char *from;
	SWBuf orig = text;
	from = orig.c_str();
	for (text = ""; *from; from++)
	{
		if (*from == '\\') // a RTF command
		{
			if ( !strncmp(from+1, "pard", 4) )
				//(from[1] == 'p') && (from[2] == 'a') && (from[3] == 'r') && (from[4] == 'd'))
			{ // switch all modifiers off
				if (center)
				{
					text += "</CENTER>";
					center = false;
				}
				from += 4;
				continue;
			}
			if ( !strncmp(from+1, "par", 3) )
				//(from[1] == 'p') && (from[2] == 'a') && (from[3] == 'r'))
			{
				text += "<P>\n";
				from += 3;
				continue;
			}
			if (from[1] == ' ')
			{
				from += 1;
				continue;
			}
			if ( !strncmp(from+1, "qc", 2) )
			//(from[1] == 'q') && (from[2] == 'c')) // center on
			{
				if (!center)
				{
					text += "<CENTER>";
					center = true;
				}
				from += 2;
				continue;
			}
		}

		text += *from;
	}
	return 0;
}

SWORD_NAMESPACE_END
