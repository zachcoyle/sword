#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

main()
{
	int fd, l1, l2, l3;
	char *fnames[] = {"ot.bks", "ot.cps", "nt.bks", "nt.cps"};
	long val;
	char buf[64];

#ifndef O_BINARY		// O_BINARY is needed in Borland C++ 4.53
#define O_BINARY 0		// If it hasn't been defined than we probably
#endif				// don't need it.


	for (l1 = 0; l1 < 2; l1++) {
		for (l2 = 0; l2 < 2; l2++) {
			l3 = 1;
			sprintf(buf, "%s", fnames[(l1*2)+l2]);
			printf("		// %s\n", fnames[(l1*2)+l2]);
			fd = open(buf, O_RDONLY|O_BINARY);
			while (read(fd, &val, 4) == 4) {
				l3++;
				printf("%ld, ", val/(4 + (l2*2)));
				if (!(l3%7))
					printf("\n");
			}
			close(fd);
			printf("}, \n");
		}
	}
}
