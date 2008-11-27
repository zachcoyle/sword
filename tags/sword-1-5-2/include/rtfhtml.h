/***************************************************************************
                          rtfhtml.h  -  description                              
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

/* $Header: /space/oldserver/cvsroot/core/sword/include/rtfhtml.h,v 1.4 2001/02/15 21:20:14 jansorg Exp $ */
/* $Revision: 1.4 $ */

#ifndef RTFHTML_H
#define RTFHTML_H

#include <swfilter.h>

#include <defs.h>

  /** this filter converts RTF text into HTML text
  */
class SWDLLEXPORT RTFHTML:public SWFilter
{
public:
  RTFHTML ();
  virtual char ProcessText (char *text, int maxlen, const SWKey * key);
};

#endif