#include <stdio.h>
#include <rawtext.h>
#include <swmgr.h>
#include <regex.h> // GNU
#include <iostream>

void percentUpdate(char percent, void *userData) {
	static char printed = 0;
	char maxHashes = *((char *)userData);
	
	while ((((float)percent)/100) * maxHashes > printed) {
		std::cout << "=";
		printed++;
		std::cout.flush();
	}
/*
	std::cout << (int)percent << "% ";
*/
	std::cout.flush();
}


int main(int argc, char **argv)
{
	SWMgr manager;
	SWModule *target;
	ListKey listkey;
	VerseKey vk;
	ModMap::iterator it;

	if ((argc != 3) && (argc != 5)) {
		fprintf(stderr, "usage: %s <modname> <\"search string\"> [\"min\" \"max\"]\n", argv[0]);
		exit(-1);
	}

	it = manager.Modules.find(argv[1]);
	if (it == manager.Modules.end()) {
		fprintf(stderr, "Could not find module [%s].  Available modules:\n", argv[1]);
		for (it = manager.Modules.begin(); it != manager.Modules.end(); it++) {
			fprintf(stderr, "[%s]\t - %s\n", (*it).second->Name(), (*it).second->Description());
		}
		exit(-1);
	}

	target = (*it).second;

	if (argc == 5) {			// if min / max specified
		vk.LowerBound(argv[3]);
		vk.UpperBound(argv[4]);
		vk.Persist(1);
		target->setKey(vk);
	}

	std::cout << "[0=================================50===============================100]\n ";
	char lineLen = 70;
	listkey = target->Search(argv[2], -2, 0/*REG_ICASE*/, 0, 0, &percentUpdate, &lineLen);
	std::cout << "\n";
	while (!listkey.Error()) {
		std::cout << (const char *)listkey << std::endl;
		listkey++;
	}

}