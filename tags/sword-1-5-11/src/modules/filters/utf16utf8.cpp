/******************************************************************************
 *
 * UTF16UTF8 -	SWFilter descendant to convert UTF-16 to UTF-8
 *
 */

#include <stdlib.h>
#include <stdio.h>

#include <utf16utf8.h>
#include <swbuf.h>

SWORD_NAMESPACE_START

UTF16UTF8::UTF16UTF8() {
}


char UTF16UTF8::processText(SWBuf &text, const SWKey *key, const SWModule *module)
{
  unsigned short *from;

  int len;
  unsigned long uchar;
  unsigned short schar;
  len = 0;
  from = (unsigned short*) text.c_str();
  while (*from) {
        len += 2;
        from++;
  }

	SWBuf orig = text;
	from = (unsigned short*)orig.c_str();


  // -------------------------------

  for (text = ""; *from; from++) {
    uchar = 0;

    if (*from < 0xD800 || *from > 0xDFFF) {
      uchar = *from;
    }
    else if (*from >= 0xD800 && *from <= 0xDBFF) {
      uchar = *from;
      schar = *(from+1);
      if (uchar < 0xDC00 || uchar > 0xDFFF) {
	//error, do nothing
	continue;
      }
      uchar &= 0x03ff;
      schar &= 0x03ff;
      uchar <<= 10;
      uchar |= schar;
      uchar += 0x10000;
      from++;
    }
    else {
      //error, do nothing
      continue;
    }
    
    if (uchar < 0x80) { 
      text += uchar;
    }
    else if (uchar < 0x800) { 
      text += 0xc0 | (uchar >> 6); 
      text += 0x80 | (uchar & 0x3f);
    }
    else if (uchar < 0x10000) {
      text += 0xe0 | (uchar >> 12);
      text += 0x80 | ((uchar >> 6) & 0x3f);
      text += 0x80 | (uchar & 0x3f);
    }
    else if (uchar < 0x200000) {
      text += 0xF0 | (uchar >> 18);
      text += 0x80 | ((uchar >> 12) & 0x3F);
      text += 0x80 | ((uchar >> 6) & 0x3F);
      text += 0x80 | (uchar & 0x3F);
    }
  }
  
  return 0;
}




SWORD_NAMESPACE_END