/******************************************************************************
 *  stringmgr.cpp - implementation of class StringMgr
 *
 * $Id$
 *
 * Copyright 1998 CrossWire Bible Society (http://www.crosswire.org)
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

#include <stringmgr.h>
#include <swlog.h>
#include <localemgr.h>
#include <utilstr.h>

#ifdef _ICU_

#include <unicode/utypes.h>
#include <unicode/ucnv.h>
#include <unicode/ustring.h>
#include <unicode/uchar.h>

#include <unicode/unistr.h>
#include <unicode/translit.h>

#include <unicode/locid.h>

#endif

SWORD_NAMESPACE_START

StringMgr *StringMgr::systemStringMgr = 0;

class __staticsystemStringMgr {
public:
	__staticsystemStringMgr() { }
	~__staticsystemStringMgr() { if (StringMgr::systemStringMgr) delete StringMgr::systemStringMgr; StringMgr::systemStringMgr = 0; }
} _staticsystemStringMgr;


#ifdef _ICU_

//here comes our ICUStringMgr reimplementation
class ICUStringMgr : public StringMgr {
public:
	virtual char *upperUTF8(char *, unsigned int maxlen = 0) const;
	
protected:
	virtual bool supportsUnicode() const { return true; };
};

#endif


/** Default constructor
*/		
StringMgr::StringMgr() {
}

/** Copy constructor
*/	
StringMgr::StringMgr(const StringMgr &m) {
}

/** Destructor
*/	
StringMgr::~StringMgr() {
}

/** Sets the global StringMgr handle
* @param newStringMgr The new global StringMgr. This pointer will be deleted by this StringMgr
*/	
void StringMgr::setSystemStringMgr(StringMgr *newStringMgr) {
	if (systemStringMgr) 
		delete systemStringMgr;
	
	systemStringMgr = newStringMgr;

   // TODO: this is magic. apparently we have to reset the system localemgr upon changing stringmgr.
   // setting system stringmgr should be set before localemgr and not possible to change.
   // rework this design.
	LocaleMgr::getSystemLocaleMgr()->setSystemLocaleMgr(new LocaleMgr());
}

/** Returns the global StringMgr handle
* @return The global string handle
*/
StringMgr* StringMgr::getSystemStringMgr() {
	if (!systemStringMgr) {
#ifdef _ICU_
		systemStringMgr = new ICUStringMgr();
// 		SWLog::getSystemLog()->logInformation("created default ICUStringMgr");
#else
		systemStringMgr = new StringMgr();
//  		SWLog::getSystemLog()->logInformation("created default StringMgr");
#endif
	}
	
	return systemStringMgr;
}


/** Converts the param to an upper case Utf8 string
* @param The text encoded in utf8 which should be turned into an upper case string
*/	
char *StringMgr::upperUTF8(char *t, unsigned int maxlen) const {
	// try to decide if it's worth trying to toupper.  Do we have more
	// characters which are probably lower latin than not?
	long performOp = 0;
	for (const char *ch = t; *ch; ch++)
		performOp += (*ch > 0) ? 1 : -1;

	if (performOp > 0) {
		return upperLatin1(t);
	}

	return t;
}

/** Converts the param to an uppercase latin1 string
* @param The text encoded in latin1 which should be turned into an upper case string
*/	
char* StringMgr::upperLatin1(char* buf, unsigned int maxlen) const {
	if (!buf)
		return 0;
		
	char *ret = buf;
	bool checkMax = maxlen;

	while (*buf && (!checkMax || maxlen--)) {
		*buf = SW_toupper(*buf);
		buf++;
	}

	return ret;
}

bool StringMgr::supportsUnicode() const {
	return false; //default impl has no UTF8 support
}


#ifdef _ICU_

char *ICUStringMgr::upperUTF8(char *buf, unsigned int maxlen) const {
	char *ret = buf;
	int max = (maxlen) ? maxlen : strlen(buf);
		
	UErrorCode err = U_ZERO_ERROR;
		
	if (!buf || !max) {
		return ret;
	}
		
	UChar *lowerStr = new UChar[max+10];
	UChar *upperStr = new UChar[max+10];
		
	u_strFromUTF8(lowerStr, max+9, 0, buf, -1, &err);
	if (err != U_ZERO_ERROR) {
//		SWLog::getSystemLog()->logError("from: %s", u_errorName(err));
		delete [] lowerStr;
		delete [] upperStr;
		return ret;
	}

	u_strToUpper(upperStr, max+9, lowerStr, -1, 0, &err);
	if (err != U_ZERO_ERROR) {
//		SWLog::getSystemLog()->logError("upperCase: %s", u_errorName(err));
		delete [] lowerStr;
		delete [] upperStr;
		return ret;
	}

	ret = u_strToUTF8(ret, max, 0, upperStr, -1, &err);
		
	delete [] lowerStr;
	delete [] upperStr;
	return ret;
}
	
#endif

SWORD_NAMESPACE_END