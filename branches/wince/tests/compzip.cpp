#include <ctype.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#ifndef __GNUC__
#include <io.h>
#else
#include <unistd.h>
#endif

#include <zipcomprs.h>
#ifndef NO_SWORD_NAMESPACE
using namespace sword;
#endif

class FileCompress: public ZipCompress {
	int ifd;
	int ofd;
	int ufd;
	int zfd;
public:
	FileCompress(char *);
	~FileCompress();
	unsigned long GetChars(char *, unsigned long len);
	unsigned long SendChars(char *, unsigned long len);
	void Encode();
	void Decode();
};


FileCompress::FileCompress(char *fname) 
{
	char buf[256];

#ifndef O_BINARY
#define O_BINARY 0
#endif

	ufd  = open(fname, O_RDWR|O_CREAT|O_BINARY, 00644);

	sprintf(buf, "%s.zip", fname);
	zfd = open(buf, O_RDWR|O_CREAT|O_BINARY, 00644);
}

	
FileCompress::~FileCompress() 
{
	close(ufd);
	close(zfd);
}


unsigned long FileCompress::GetChars(char *buf, unsigned long len) 
{
	return read(ifd, buf, len);
}


unsigned long FileCompress::SendChars(char *buf, unsigned long len) 
{
	return write(ofd, buf, len);
}


void FileCompress::Encode() 
{
	ifd = ufd;
	ofd = zfd;

	ZipCompress::Encode();
}


void FileCompress::Decode() 
{
	ifd = zfd;
	ofd = ufd;

	ZipCompress::Decode();
}


main(int argc, char **argv)
{
	int decomp = 0;
	SWCompress *fobj;
	
	if (argc != 2) {
		fprintf(stderr, "usage: %s <filename|filename.zip>\n", argv[0]);
		exit(1);
	}

	if (strlen(argv[1]) > 4) {
		if (!strcmp(&argv[1][strlen(argv[1])-4], ".zip")) {
			argv[1][strlen(argv[1])-4] = 0;
			decomp = 1;
		}
	}
			
	fobj = new FileCompress(argv[1]);
	
	if (decomp)
		fobj->Decode();
	else fobj->Encode();

	delete fobj;
}
