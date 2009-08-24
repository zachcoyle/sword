#include <stdio.h>
#include <iostream.h>
#include <stdlib.h>
#include <swmgr.h>

int main(int argc, char **argv)
{
	SWMgr manager;
	SWModule *target;
	ModMap::iterator it;

	if (argc != 3) {
		fprintf(stderr, "usage: %s <modname> <\"lookup key\">\n", argv[0]);
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

	target->SetKey(argv[2]);

	(const char *)*target;		// force an entry lookup to resolve key so we
						// get the idxbuf entry for the key
	cout << (const char *)(SWKey &)*target << ":\n";
	cout << target->StripText();
	cout << "\n";
	cout << "==========================\n";
	return 0;
}