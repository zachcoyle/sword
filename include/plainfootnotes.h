/***************************************************************************
 *
 * $Id: plainfootnotes.h,v 1.6 2003/02/20 07:25:19 scribe Exp $
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

#ifndef PLAINFOOTNOTES_H
#define PLAINFOOTNOTES_H

#include <swfilter.h>

SWORD_NAMESPACE_START

class SWKey;

/**Shows or hides footnotes in plain text.
 *@author The team of BibleTime
 */
class SWDLLEXPORT PLAINFootnotes : public SWFilter {
	bool option;
	static const char on[];
	static const char off[];
	static const char optName[];
	static const char optTip[];
	OptionsList options;

public:
	PLAINFootnotes();
	virtual ~PLAINFootnotes();
	virtual char processText(SWBuf &text, const SWKey *key = 0, const SWModule *module = 0);
	virtual const char *getOptionName() { return optName; }
	virtual const char *getOptionTip() { return optTip; }
	virtual void setOptionValue(const char *ival);
	virtual const char *getOptionValue();
	virtual OptionsList getOptionValues() { return options; }
};

SWORD_NAMESPACE_END
#endif
