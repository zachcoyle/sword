/******************************************************************************
 *  swsourcemgr.cpp   - implementaion of class SWMgr used to interact with an install
 *				base of sword modules.
 *
 * $Id: swsourcemgr.cpp,v 1.2 2003/06/27 01:41:07 scribe Exp $
 *
 * Copyright 2002 CrossWire Bible Society (http://www.crosswire.org)
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

 #include "swsourcemgr.h"
 #include "filemgr.h"
 #include "utilstr.h"
 #include "swconfig.h"
 #include <dirent.h>
 
 SWORD_NAMESPACE_START
 
 SWSourceMgr::SWSourceMgr(const char *iConfigPath) {
	SWBuf path;
	
	//init();
	
	path = iConfigPath;
	if ((iConfigPath[strlen(iConfigPath)-1] != '\\') && (iConfigPath[strlen(iConfigPath)-1] != '/'))
		path += "/";
	if (FileMgr::existsFile(path.c_str(), "mods.conf")) {
		stdstr(&prefixPath, path.c_str());
		path += "mods.conf";
		stdstr(&configPath, path.c_str());
	}
	else {
		if (FileMgr::existsDir(path.c_str(), "mods.d")) {
			stdstr(&prefixPath, path.c_str());
			path += "mods.d";
			stdstr(&configPath, path.c_str());
			configType = 1;
		}
	}

	if (configPath)
		loadConfigDir(configPath);
}

void SWSourceMgr::loadConfigDir(const char *ipath)
{
   DIR *dir;
   struct dirent *ent;
   SWBuf newmodfile;
 
	if ((dir = opendir(ipath))) {
		rewinddir(dir);
		while ((ent = readdir(dir))) {
			if ((strcmp(ent->d_name, ".")) && (strcmp(ent->d_name, ".."))) {
				newmodfile = ipath;
				if ((ipath[strlen(ipath)-1] != '\\') && (ipath[strlen(ipath)-1] != '/'))
					newmodfile += "/";
				newmodfile += ent->d_name;
				if (config) {
					SWConfig tmpConfig(newmodfile.c_str());
					*config += tmpConfig;
				}
				else	config = myconfig = new SWConfig(newmodfile.c_str());
			}
		}
		closedir(dir);
		if (!config) {	// if no .conf file exist yet, create a default
			newmodfile = ipath;
			if ((ipath[strlen(ipath)-1] != '\\') && (ipath[strlen(ipath)-1] != '/'))
				newmodfile += "/";
			newmodfile += "globals.conf";
			config = myconfig = new SWConfig(newmodfile.c_str());
		}
	}
}


 
 SWORD_NAMESPACE_END