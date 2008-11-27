/***************************************************************************
                     thmlgbf.cpp  -  ThML to GBF filter
                             -------------------
    begin                : 1999-10-28
    copyright            : 2001 by CrossWire Bible Society
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
#include <string.h>
#include <thmlgbf.h>


ThMLGBF::ThMLGBF()
{
}


char ThMLGBF::ProcessText(char *text, int maxlen)
{
  char *to, *from, token[2048];
  int tokpos = 0;
  bool intoken 	= false;
  int len;
  bool ampersand = false;
  
  len = strlen(text) + 1;						// shift string to right of buffer
  if (len < maxlen) {
    memmove(&text[maxlen - len], text, len);
    from = &text[maxlen - len];
  }
  else	from = text;							// -------------------------------
  for (to = text; *from; from++)
    {
      if (*from == '<') {
	intoken = true;
	tokpos = 0;
	memset(token, 0, 2048);
	ampersand = false;
	continue;
      }
      else if (*from == '&') {
	intoken = true;
	tokpos = 0;
	memset(token, 0, 2048);
	ampersand = true;
	continue;
      }
      if (*from == ';' && ampersand) {
	intoken = false;
	
	if (!strncmp("nbsp", token, 4)) *to++ = ' ';
	else if (!strncmp("quot", token, 4)) *to++ = '"';
	else if (!strncmp("amp", token, 3)) *to++ = '&';
	else if (!strncmp("lt", token, 2)) *to++ = '<';
	else if (!strncmp("gt", token, 2)) *to++ = '>';
	else if (!strncmp("brvbar", token, 6)) *to++ = '|';
	else if (!strncmp("sect", token, 4)) *to++ = '�';
	else if (!strncmp("copy", token, 4)) *to++ = '�';
	else if (!strncmp("laquo", token, 5)) *to++ = '�';
	else if (!strncmp("reg", token, 3)) *to++ = '�';
	else if (!strncmp("acute", token, 5)) *to++ = '�';
	else if (!strncmp("para", token, 4)) *to++ = '�';
	else if (!strncmp("raquo", token, 5)) *to++ = '�';
	
	else if (!strncmp("Aacute", token, 6)) *to++ = '�';
	else if (!strncmp("Agrave", token, 6)) *to++ = '�';
	else if (!strncmp("Acirc", token, 5)) *to++ = '�';
	else if (!strncmp("Auml", token, 4)) *to++ = '�';
	else if (!strncmp("Atilde", token, 6)) *to++ = '�';
	else if (!strncmp("Aring", token, 5)) *to++ = '�';
	else if (!strncmp("aacute", token, 6)) *to++ = '�';
	else if (!strncmp("agrave", token, 6)) *to++ = '�';
	else if (!strncmp("acirc", token, 5)) *to++ = '�';
	else if (!strncmp("auml", token, 4)) *to++ = '�';
	else if (!strncmp("atilde", token, 6)) *to++ = '�';
	else if (!strncmp("aring", token, 5)) *to++ = '�';
	else if (!strncmp("Eacute", token, 6)) *to++ = '�';
	else if (!strncmp("Egrave", token, 6)) *to++ = '�';
	else if (!strncmp("Ecirc", token, 5)) *to++ = '�';
	else if (!strncmp("Euml", token, 4)) *to++ = '�';
	else if (!strncmp("eacute", token, 6)) *to++ = '�';
	else if (!strncmp("egrave", token, 6)) *to++ = '�';
	else if (!strncmp("ecirc", token, 5)) *to++ = '�';
	else if (!strncmp("euml", token, 4)) *to++ = '�';
	else if (!strncmp("Iacute", token, 6)) *to++ = '�';
	else if (!strncmp("Igrave", token, 6)) *to++ = '�';
	else if (!strncmp("Icirc", token, 5)) *to++ = '�';
	else if (!strncmp("Iuml", token, 4)) *to++ = '�';
	else if (!strncmp("iacute", token, 6)) *to++ = '�';
	else if (!strncmp("igrave", token, 6)) *to++ = '�';
	else if (!strncmp("icirc", token, 5)) *to++ = '�';
	else if (!strncmp("iuml", token, 4)) *to++ = '�';
	else if (!strncmp("Oacute", token, 6)) *to++ = '�';
	else if (!strncmp("Ograve", token, 6)) *to++ = '�';
	else if (!strncmp("Ocirc", token, 5)) *to++ = '�';
	else if (!strncmp("Ouml", token, 4)) *to++ = '�';
	else if (!strncmp("Otilde", token, 6)) *to++ = '�';
	else if (!strncmp("oacute", token, 6)) *to++ = '�';
	else if (!strncmp("ograve", token, 6)) *to++ = '�';
	else if (!strncmp("ocirc", token, 5)) *to++ = '�';
	else if (!strncmp("ouml", token, 4)) *to++ = '�';
	else if (!strncmp("otilde", token, 6)) *to++ = '�';
	else if (!strncmp("Uacute", token, 6)) *to++ = '�';
	else if (!strncmp("Ugrave", token, 6)) *to++ = '�';
	else if (!strncmp("Ucirc", token, 5)) *to++ = '�';
	else if (!strncmp("Uuml", token, 4)) *to++ = '�';
	else if (!strncmp("uacute", token, 6)) *to++ = '�';
	else if (!strncmp("ugrave", token, 6)) *to++ = '�';
	else if (!strncmp("ucirc", token, 5)) *to++ = '�';
	else if (!strncmp("uuml", token, 4)) *to++ = '�';
	else if (!strncmp("Yacute", token, 6)) *to++ = '�';
	else if (!strncmp("yacute", token, 6)) *to++ = '�';
	else if (!strncmp("yuml", token, 4)) *to++ = '�';
	
	else if (!strncmp("deg", token, 3)) *to++ = '�';
	else if (!strncmp("plusmn", token, 6)) *to++ = '�';
	else if (!strncmp("sup2", token, 4)) *to++ = '�';
	else if (!strncmp("sup3", token, 4)) *to++ = '�';
	else if (!strncmp("sup1", token, 4)) *to++ = '�';
	else if (!strncmp("nbsp", token, 4)) *to++ = '�';
	else if (!strncmp("pound", token, 5)) *to++ = '�';
	else if (!strncmp("cent", token, 4)) *to++ = '�';
	else if (!strncmp("frac14", token, 6)) *to++ = '�';
	else if (!strncmp("frac12", token, 6)) *to++ = '�';
	else if (!strncmp("frac34", token, 6)) *to++ = '�';
	else if (!strncmp("iquest", token, 6)) *to++ = '�';
	else if (!strncmp("iexcl", token, 5)) *to++ = '�';
	else if (!strncmp("ETH", token, 3)) *to++ = '�';
	else if (!strncmp("eth", token, 3)) *to++ = '�';
	else if (!strncmp("THORN", token, 5)) *to++ = '�';
	else if (!strncmp("thorn", token, 5)) *to++ = '�';
	else if (!strncmp("AElig", token, 5)) *to++ = '�';
	else if (!strncmp("aelig", token, 5)) *to++ = '�';
	else if (!strncmp("Oslash", token, 6)) *to++ = '�';
	else if (!strncmp("curren", token, 6)) *to++ = '�';
	else if (!strncmp("Ccedil", token, 6)) *to++ = '�';
	else if (!strncmp("ccedil", token, 6)) *to++ = '�';
	else if (!strncmp("szlig", token, 5)) *to++ = '�';
	else if (!strncmp("Ntilde", token, 6)) *to++ = '�';
	else if (!strncmp("ntilde", token, 6)) *to++ = '�';
	else if (!strncmp("yen", token, 3)) *to++ = '�';
	else if (!strncmp("not", token, 3)) *to++ = '�';
	else if (!strncmp("ordf", token, 4)) *to++ = '�';
	else if (!strncmp("uml", token, 3)) *to++ = '�';
	else if (!strncmp("shy", token, 3)) *to++ = '�';
	else if (!strncmp("macr", token, 4)) *to++ = '�';
	continue;
	
      }
      else if (*from == '>' && !ampersand)
	{
	  intoken = false;
	  // process desired tokens
	  if (!strncmp(token, "sync type=\"Strongs\" value=\"", 27)) {
	      *to++ = '<';
	      *to++ = 'W';
	      for (unsigned int i = 27; token[i] != '\"'; i++)
		*to++ = token[i];
	      *to++ = '>';
	      continue;
	  }
	  if (!strncmp(token, "sync type=\"Morph\" value=\"", 25)) {
	    *to++ = '<';
	    *to++ = 'W';
	    *to++ = 'T';
	    for (unsigned int i = 25; token[i] != '\"'; i++)
	      *to++ = token[i];
	    *to++ = '>';
	    continue;
	  }
	  else if (!strncmp(token, "scripRef", 8)) {
	    *to++ = '#';
	  }
	  else if (!strncmp(token, "/scripRef", 9)) {
	    *to++ = '|';
	  }
	  else if (!strncmp(token, "note place=\"foot\"", 17)) {
	    *to++ = '<';
	    *to++ = 'R';
	    *to++ = 'F';
	    *to++ = '>';
	    continue;
	  }
	  else if (!strncmp(token, "/note", 5)) {
	    *to++ = '<';
	    *to++ = 'R';
	    *to++ = 'f';
	    *to++ = '>';
	    continue;
	  }
	  else if (!strncmp(token, "sup", 3)) {
	    *to++ = '<';
	    *to++ = 'F';
	    *to++ = 'S';
	    *to++ = '>';
	  }
	  else if (!strncmp(token, "/sup", 4)) {
	    *to++ = '<';
	    *to++ = 'F';
	    *to++ = 's';
	    *to++ = '>';
	  }
	  else if (!strncmp(token, "foreign lang=\"el\"", 17)) {
	    *to++ = '<';
	    *to++ = 'F';
	    *to++ = 'N';
	    *to++ = 'S';
	    *to++ = 'I';
	    *to++ = 'L';
	    *to++ = ' ';
	    *to++ = 'G';
	    *to++ = 'a';
	    *to++ = 'l';
	    *to++ = 'a';
	    *to++ = 't';
	    *to++ = 'i';
	    *to++ = 'a';
	    *to++ = '>';
	    continue;
	  }
	  else if (!strnicmp(token, "font color=#ff0000", 18)) {
	    *to++ = '<';
	    *to++ = 'F';
	    *to++ = 'R';
	    *to++ = '>';
	    continue;
	  }
	  else if (!strnicmp(token, "\\font", 5)) {
	    *to++ = '<';
	    *to++ = 'F';
	    *to++ = 'r';
	    *to++ = '>';
	    continue;	    
	  }
	  else if (!strncmp(token, "foreign lang=\"he\"", 17)) {
	    *to++ = '<';
	    *to++ = 'F';
	    *to++ = 'N';
	    *to++ = 'S';
	    *to++ = 'I';
	    *to++ = 'L';
	    *to++ = ' ';
	    *to++ = 'E';
	    *to++ = 'z';
	    *to++ = 'r';
	    *to++ = 'a';
	    *to++ = '>';
	    continue;
	  }
	  else if (!strncmp(token, "/foreign", 8)) {
	    *to++ = '<';
	    *to++ = 'F';
	    *to++ = 'n';
	    *to++ = '>';
	    continue;				
	  }
	  else if (!strncmp(token, "br", 2) || !strncmp(token, "BR", 2)) {
		    *to++ = '<';
		    *to++ = 'C';
		    *to++ = 'L';
		    *to++ = '>';
		    continue;
		  }
	  else switch(*token) {
	          case 'I':			// font tags
		  case 'i':
		    *to++ = '<';
		    *to++ = 'F';
		    *to++ = 'I';
		    *to++ = '>';
		    continue;
		  case 'B':		// bold start
		  case 'b':
		    *to++ = '<';
		    *to++ = 'F';
		    *to++ = 'B';
		    *to++ = '>';
		    continue;
		  case '/':
		    switch(token[1]) {
		    case 'P':
		    case 'p':
		      *to++ = '<';
		      *to++ = 'C';
		      *to++ = 'M';
		      *to++ = '>';
		      continue;
		    case 'I':
		    case 'i':		// italic end
		      *to++ = '<';
		      *to++ = 'F';
		      *to++ = 'i';
		      *to++ = '>';
		      continue;
		    case 'B':		// bold start
		    case 'b':
		      *to++ = '<';
		      *to++ = 'F';
		      *to++ = 'b';
		      *to++ = '>';
		      continue;
		    }
		  }
	  continue;
	}
	if (intoken) {
		if (tokpos < 2047)
			token[tokpos++] = *from;
     }
	else	*to++ = *from;
  }
  *to++ = 0;
  *to = 0;          
  return 0;
}


