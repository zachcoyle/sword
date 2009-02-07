#include <stdio.h>
#include <rawtext.h>
#include <swmgr.h>

int main(int argc, char **argv)
{
	SWMgr manager(0,0,true);
	SWModule *target;
	ListKey listkey;
	VerseKey vk;
	ModMap::iterator it;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <modname>\n", argv[0]);
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
	target = it->second;

	if (!target->hasSearchFramework()) {
		fprintf(stderr, "%s: error: %s does not support a search framework.\n", *argv, it->second->Name());
		exit(-2);
	}

	printf("Building framework, please wait...\n");
	char error = target->createSearchFramework();
	if (error) {
		fprintf(stderr, "%s: couldn't create search framework (permissions?)\n", *argv);
	}
}