/*****************************************************************************
 * InstallMgr functions to be made into something usefully exposed by
 * master Glassey
 *
 */
 

#include <installmgr.h>
#include <filemgr.h>

#include <fcntl.h>
#ifndef __GNUC__
#include <io.h>
#else
#include <unistd.h>
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

#ifdef CURLAVAILABLE
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#endif

#include <defs.h>
#include <vector>
#include <swmgr.h>
#include <dirent.h>

using namespace std;

SWORD_NAMESPACE_START

int my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream);
int my_fprogress(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);

static InstallMgr_init _InstallMgr_init;

InstallMgr_init::InstallMgr_init() {
#ifdef CURLAVAILABLE
	curl_global_init(CURL_GLOBAL_DEFAULT);
#else
//	fprintf(stderr, "libCURL is needed for remote installation functions\n");
#endif
}

InstallMgr_init::~InstallMgr_init() {
#ifdef CURLAVAILABLE
	curl_global_cleanup();
#else
//	fprintf(stderr, "libCURL is needed for remote installation functions\n");
#endif
}


int my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream) {
	struct FtpFile *out=(struct FtpFile *)stream;
	if (out && !out->stream) {
		/* open file for writing */
		out->stream=fopen(out->filename, "wb");
		if (!out->stream)
			return -1; /* failure, can't open file to write */
	}
	return fwrite(buffer, size, nmemb, out->stream);
}


int my_fprogress(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow) {
	if (clientp) {
		((InstallMgr *)clientp)->statusUpdate(dltotal, dlnow);
	}
	return 0;
}


InstallMgr::InstallMgr(const char *privatePath) {
	this->privatePath = 0;
	stdstr(&(this->privatePath), privatePath);
	installConf = new SWConfig(((SWBuf)privatePath + "/InstallMgr.conf").c_str());

	SectionMap::iterator sourcesSection;
	ConfigEntMap::iterator sourceBegin;
	ConfigEntMap::iterator sourceEnd;

	sources.clear();
	
	sourcesSection = installConf->Sections.find("Sources");
	passive = (!stricmp((*installConf)["General"]["PassiveFTP"].c_str(), "true"));

	if (sourcesSection != installConf->Sections.end()) {
		sourceBegin = sourcesSection->second.lower_bound("FTPSource");
		sourceEnd = sourcesSection->second.upper_bound("FTPSource");

		while (sourceBegin != sourceEnd) {
			InstallSource *is = new InstallSource(sourceBegin->second.c_str(), "FTP");
			sources[is->caption] = is;
			SWBuf parent = (SWBuf)privatePath + "/" + is->source + "/file";
			FileMgr::createParent(parent.c_str());
			is->localShadow = (SWBuf)privatePath + "/" + is->source;
			sourceBegin++;
		}
	}
}


InstallMgr::~InstallMgr() {
	delete [] privatePath;
	delete installConf;
}


void InstallMgr::statusUpdate(double dltotal, double dlnow) {
}

void InstallMgr::preDownloadStatus(long totalBytes, long completedBytes, const char *message) {
}

char InstallMgr::FTPURLGetFile(void *session, const char *dest, const char *sourceurl) {
	char retVal = 0;
#ifdef CURLAVAILABLE
	struct FtpFile ftpfile = {dest, NULL};

	CURL *curl = (CURL *)session;
	CURLcode res;
	
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, sourceurl);
	
		curl_easy_setopt(curl, CURLOPT_USERPWD, "ftp:installmgr@user.com");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_fwrite);
		if (!passive)
			curl_easy_setopt(curl, CURLOPT_FTPPORT, "-");
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
		curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, this);
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, my_fprogress);
		/* Set a pointer to our struct to pass to the callback */
		curl_easy_setopt(curl, CURLOPT_FILE, &ftpfile);

		/* Switch on full protocol/debug output */
		curl_easy_setopt(curl, CURLOPT_VERBOSE, TRUE);

		res = curl_easy_perform(curl);

		if(CURLE_OK != res) {
			retVal = -1;
		}
	}

	if (ftpfile.stream)
		fclose(ftpfile.stream); /* close the local file */
#else
	fprintf(stderr, "libCURL is needed for remote installation functions\n");
#endif
	return retVal;
}



vector<struct ftpparse> InstallMgr::FTPURLGetDir(void *session, const char *dirurl) {

	vector<struct ftpparse> dirList;
	
	if (!FTPURLGetFile(session, "dirlist", dirurl)) {
		int fd = open("dirlist", O_RDONLY|O_BINARY);
		long size = lseek(fd, 0, SEEK_END);
		lseek(fd, 0, SEEK_SET);
		char *buf = new char [ size + 1 ];
		read(fd, buf, size);
		close(fd);
		char *start = buf;
		char *end = start;
		while (start < (buf+size)) {
			struct ftpparse item;
			bool looking = true;
			for (end = start; *end; end++) {
				if (looking) {
					if ((*end == 10) || (*end == 13)) {
						*end = 0;
						looking = false;
					}
				}
				else if ((*end != 10) && (*end != 13))
					break;
			}
			int status = ftpparse(&item, start, end - start);
			if (status)
				dirList.push_back(item);
			start = end;
		}
	}
	return dirList;
}


void *InstallMgr::FTPOpenSession() {
	void *retVal = 0;
#ifdef CURLAVAILABLE
	CURL *curl;

	retVal = curl_easy_init();
#else
	fprintf(stderr, "libCURL is needed for remote installation functions\n");
#endif
	return retVal;
}


void InstallMgr::FTPCloseSession(void *session) {
#ifdef CURLAVAILABLE
	CURL *curl = (CURL *)session;
	curl_easy_cleanup(curl);
#else
	fprintf(stderr, "libCURL is needed for remote installation functions\n");
#endif
}


int InstallMgr::removeModule(SWMgr *manager, const char *modName) {
	SectionMap::iterator module;
	ConfigEntMap::iterator fileBegin;
	ConfigEntMap::iterator fileEnd, entry;

	module = manager->config->Sections.find(modName);

	if (module != manager->config->Sections.end()) {
			
		fileBegin = module->second.lower_bound("File");
		fileEnd = module->second.upper_bound("File");

		SWBuf modFile;
		SWBuf modDir;
		entry = module->second.find("AbsoluteDataPath");
		modDir = entry->second.c_str();
		if (fileBegin != fileEnd) {	// remove each file
			while (fileBegin != fileEnd) {
				modFile = modDir;
				modFile += "/";
				modFile += fileBegin->second.c_str();
				//remove file
				remove(modFile.c_str());
				fileBegin++;
			}
		}
		else {	//remove all files in DataPath directory

			DIR *dir;
			struct dirent *ent;
			ConfigEntMap::iterator entry;


			if (dir = opendir(modDir.c_str())) {
				rewinddir(dir);
				while ((ent = readdir(dir))) {
					if ((strcmp(ent->d_name, ".")) && (strcmp(ent->d_name, ".."))) {
						modFile = modDir;
						modFile += "/";
						modFile += ent->d_name;
						remove(modFile.c_str());
					}
				}
				closedir(dir);
			}
			if (dir = opendir(manager->configPath)) {	// find and remove .conf file
				rewinddir(dir);
				while ((ent = readdir(dir))) {
					if ((strcmp(ent->d_name, ".")) && (strcmp(ent->d_name, ".."))) {
						modFile = manager->configPath;
						modFile += "/";
						modFile += ent->d_name;
						SWConfig *config = new SWConfig(modFile.c_str());
						if (config->Sections.find(modName) != config->Sections.end()) {
							delete config;
							remove(modFile.c_str());
						}
						else	delete config;
					}
				}
				closedir(dir);
			}
		}
		return 0;
	}
	return 1;
}



InstallSource::InstallSource(const char *confEnt, const char *type) {
	char *buf = 0;
	stdstr(&buf, confEnt);

	caption = strtok(buf, "|");
	source = strtok(0, "|");
	directory = strtok(0, "|");
	delete [] buf;
	this->type = type;
	mgr = 0;
	userData = 0;
}


InstallSource::~InstallSource() {
	if (mgr)
		delete mgr;
}


int InstallMgr::FTPCopy(InstallSource *is, const char *src, const char *dest, bool dirTransfer, const char *suffix) {
	terminate = false;
	void *session = FTPOpenSession();
	SWBuf url = (SWBuf)"ftp://" + is->source + is->directory.c_str() + "/"; //dont forget the final slash
	if (FTPURLGetFile(session, "dirlist", url.c_str())) {
		return -1;
	}
	if (dirTransfer) {
		SWBuf url = (SWBuf)"ftp://" + is->source + is->directory.c_str() + "/" + src + "/"; //dont forget the final slash
		vector<struct ftpparse> dirList = FTPURLGetDir(session, url.c_str());

		if (!dirList.size()) {
			return -1;
		}
					
		long totalBytes = 0;
		for (int i = 0; i < dirList.size(); i++)
			totalBytes += dirList[i].size;
		long completedBytes = 0;
		for (int i = 0; i < dirList.size(); i++) {
			if (dirList[i].flagtrycwd != 1) {
				SWBuf buffer = (SWBuf)dest + "/" + (dirList[i].name);
				if (!strcmp(&buffer.c_str()[buffer.length()-strlen(suffix)], suffix)) {
					SWBuf buffer2 = "Downloading (";
					buffer2.appendFormatted("%d", i+1);
					buffer2 += " of ";
					buffer2.appendFormatted("%d", dirList.size());
					buffer2 += "): ";
					buffer2 += (dirList[i].name);
					preDownloadStatus(totalBytes, completedBytes, buffer2.c_str());
					FileMgr::createParent(buffer.c_str());	// make sure parent directory exists
					try {
						SWBuf url = (SWBuf)"ftp://" + is->source + is->directory.c_str() + "/" + src + "/" + dirList[i].name; //dont forget the final slash
						if (FTPURLGetFile(session, buffer.c_str(), url.c_str())) {
							return -2;
						}
						completedBytes += dirList[i].size;
					}
					catch (...) {}
					if (terminate)
						break;
				}
			}
		}
	}
	else {
//		Synchronize((TThreadMethod)&PreDownload2);
		try {
			SWBuf url = (SWBuf)"ftp://" + is->source + is->directory.c_str() + "/" + src; //dont forget the final slash
			if (FTPURLGetFile(session, dest, url.c_str())) {
				return -1;
			}
		}
		catch(...) {
			terminate = true;
		}
	}
	try {
		FTPCloseSession(session);
	}
	catch(...){}
}


int InstallMgr::installModule(SWMgr *destMgr, const char *fromLocation, const char *modName, InstallSource *is) {
	SectionMap::iterator module, section;
	ConfigEntMap::iterator fileBegin;
	ConfigEntMap::iterator fileEnd;
	ConfigEntMap::iterator entry;
	SWBuf sourceDir;
	SWBuf buffer;
	bool aborted = false;
	bool cipher = false;


	if (is)
		sourceDir = (SWBuf)privatePath + "/" + is->source;
	else	sourceDir = fromLocation;

	SWMgr mgr(sourceDir.c_str());
	
	module = mgr.config->Sections.find(modName);

	if (module != mgr.config->Sections.end()) {
	
		entry = module->second.find("CipherKey");
		if (entry != module->second.end())
			cipher = true;
		
		fileEnd = module->second.upper_bound("File");
		fileBegin = module->second.lower_bound("File");

		if (fileBegin != fileEnd) {	// copy each file
			if (is) {
				while (fileBegin != fileEnd) {	// ftp each file first
					buffer = sourceDir + "/" + fileBegin->second.c_str();
					if (FTPCopy(is, fileBegin->second.c_str(), buffer.c_str())) {
						aborted = true;
						break;	// user aborted
					}
					fileBegin++;
				}
				fileBegin = module->second.lower_bound("File");
			}

			if (!aborted) {
				// DO THE INSTALL
				while (fileBegin != fileEnd) {
					copyFileToSWORDInstall(destMgr, sourceDir.c_str(), fileBegin->second.c_str());
					fileBegin++;
				}
			}
			//---------------

			if (is) {
				fileBegin = module->second.lower_bound("File");
				while (fileBegin != fileEnd) {	// delete each tmp ftp file
					buffer = sourceDir + "/" + fileBegin->second.c_str();
					remove(buffer.c_str());
					fileBegin++;
				}
			}
		}
		else {	//copy all files in DataPath directory
			DIR *dir;
			struct dirent *ent;
			ConfigEntMap::iterator entry;
			SWBuf modDir;
			SWBuf modFile;
			SWBuf sourceOrig = sourceDir;

			entry = module->second.find("DataPath");
			if (entry != module->second.end()) {
				modDir = entry->second.c_str();
				entry = module->second.find("ModDrv");
				if (entry != module->second.end()) {
					if (!strcmp(entry->second.c_str(), "RawLD") || !strcmp(entry->second.c_str(), "RawLD4") || !strcmp(entry->second.c_str(), "zLD") || !strcmp(entry->second.c_str(), "RawGenBook") || !strcmp(entry->second.c_str(), "zGenBook")) {
						char *buf = new char [ strlen(modDir.c_str()) + 1 ];
	
						strcpy(buf, modDir.c_str());
						int end = strlen(buf) - 1;
						while (end) {
							if (buf[end] == '/')
								break;
							end--;
						}
						buf[end] = 0;
						modDir = buf;
						delete [] buf;
					}
				}

				if (is) {
					buffer = sourceDir + "/" + modDir;
					if (FTPCopy(is, modDir.c_str(), buffer.c_str(), true)) {
						aborted = true;	// user aborted
					}
				}
				sourceDir += "/";
				sourceDir += modDir;
				if (!aborted) {
					if (dir = opendir(sourceDir.c_str())) {
						rewinddir(dir);
						while ((ent = readdir(dir))) {
							if ((strcmp(ent->d_name, ".")) && (strcmp(ent->d_name, ".."))) {
								modFile = modDir;
								modFile += "/";
								modFile += ent->d_name;
								copyFileToSWORDInstall(destMgr, sourceOrig.c_str(), modFile.c_str());
							}
						}
						closedir(dir);
					}
				}
				if (is) {		// delete tmp ftp files
					if (dir = opendir(sourceDir.c_str())) {
						rewinddir(dir);
						while ((ent = readdir(dir))) {
							if ((strcmp(ent->d_name, ".")) && (strcmp(ent->d_name, ".."))) {
								modFile = sourceOrig + "/" + modDir;
								modFile += "/";
								modFile += ent->d_name;
								remove(modFile.c_str());
							}
						}
						closedir(dir);
					}
				}
				sourceDir = sourceOrig;
				sourceDir += "/mods.d/";
				if (!aborted) {
					if (dir = opendir(sourceDir.c_str())) {	// find and copy .conf file
						rewinddir(dir);
						while ((ent = readdir(dir))) {
							if ((strcmp(ent->d_name, ".")) && (strcmp(ent->d_name, ".."))) {
								modFile = sourceDir;
								modFile += ent->d_name;
								SWConfig *config = new SWConfig(modFile.c_str());
								if (config->Sections.find(modName) != config->Sections.end()) {
									delete config;
									SWBuf targetFile = destMgr->configPath; //"./mods.d/";
									targetFile += "/";
									targetFile += ent->d_name;
									FileMgr::copyFile(modFile.c_str(), targetFile.c_str());
									if (cipher) {
/*									
										CipherForm->modName = modName;
										CipherForm->confFile = targetFile;
										if (CipherForm->ShowModal() == mrCancel) {
											removeModule(modName);
											aborted = true;
										}
*/											
									}
								}
								else	delete config;
							}
						}
						closedir(dir);
					}
				}
			}
		}
		return (aborted) ? -1 : 0;
	}
	return 1;
}


int InstallMgr::copyFileToSWORDInstall(SWMgr *manager, const char *sourceDir, const char *fName) {
	SWBuf sourcePath = sourceDir;
	sourcePath += fName;

	SWBuf dest;
	dest = manager->prefixPath;
	if ((manager->prefixPath[strlen(manager->prefixPath)-1] != '\\') && ( manager->prefixPath[strlen(manager->prefixPath)-1] != '/'))
	   dest += "/";
	  dest += fName;

	return FileMgr::copyFile(sourcePath.c_str(), dest.c_str());
}


SWORD_NAMESPACE_END

