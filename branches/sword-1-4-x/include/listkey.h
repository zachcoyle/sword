/******************************************************************************
 * listkey.h	- code for base class 'listkey'.  listkey is the basis for all
 *				types of keys for indexing into modules
 *				(e.g. verse, word,
 *				place, etc.)
 *
 * $Id: listkey.h,v 1.1.1.1.2.1 2000/04/09 23:22:28 scribe Exp $
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

#ifndef SWLSTKEY_H
#define SWLSTKEY_H

#include <swkey.h>

class ListKey: public SWKey {

protected:
	int arraypos;
	int arraymax;
	int arraycnt;
	SWKey **array;
public:
	ListKey(const char *ikey = 0);
	ListKey(ListKey const &k);
	virtual ~ListKey();

	virtual SWKey *clone() const;
	virtual void ClearList();	// delete all elements
	virtual int Count();
	virtual void Remove();	// remove current element
	virtual char SetToElement(int, POSITION = TOP);
	virtual SWKey *GetElement(int pos = -1);
	virtual ListKey &operator <<(const SWKey &); // add to list
	virtual ListKey &operator =(ListKey &ikey);
	virtual SWKey &operator =(const char *ikey){ return SWKey::operator =(ikey); }
	virtual SWKey &operator =(const SWKey &ikey){ return SWKey::operator =(ikey); }
	virtual SWKey &operator =(POSITION);
	virtual SWKey &operator -=(int decrement);
	virtual SWKey &operator +=(int increment);
	virtual char Traversable() { return 1; }
};


#endif
