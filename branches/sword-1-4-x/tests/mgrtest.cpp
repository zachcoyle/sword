#include <swmgr.h>
#include <iostream.h>
#include <versekey.h>

int main(int argc, char **argv) {
	SWMgr mymgr;
	ModMap::iterator it;

	mymgr.setGlobalOption("Strong's Numbers", "On");
	for (it = mymgr.Modules.begin(); it != mymgr.Modules.end(); it++) {
		if ((!strcmp((*it).second->Type(), "Biblical Texts")) || (!strcmp((*it).second->Type(), "Commentaries"))) {
			(*it).second->SetKey("James 1:19");
			cout << "[" << (*it).second->Name() << "] [" << (*it).second->Description() << "]\n";
			cout << (char *) (*it).second->StripText() << "\n\n";
		}
	}
	SWModule *mhc = mymgr.Modules["MHC"];
	if (mhc) {
		for (mhc->Key("Gen 1:1"); mhc->Key() < (VerseKey) "Gen 1:10"; (*mhc)++)
			cout << (const char *) *mhc << "\n";
	}
	return 0;
}