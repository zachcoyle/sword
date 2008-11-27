/***************************************************************************
                          rwphtml.h  -  description
                             -------------------
    begin                : Tue Jun 15 15:34:26 CEST 1999

    copyright            : (C) 1999 by Torsten Uhlmann
    email                : TUhlmann@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/* $Header: /space/oldserver/cvsroot/core/sword/include/plainhtml.h,v 1.6 2001/02/19 16:59:51 mgruner Exp $ */
/* $Revision: 1.6 $ */

#ifndef PLAINHTML_H
#define PLAINHTML_H

#include <swfilter.h>

#include <defs.h>

  /** this filter converts plain text to HTML text
  */
class SWDLLEXPORT PLAINHTML:public SWFilter
{
public:
  PLAINHTML ();
  virtual char ProcessText (char *text, int maxlen, const SWKey * key);
};

#endif