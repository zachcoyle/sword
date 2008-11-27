/******************************************************************************
*
* utf8nfkd - SWFilter decendant to perform NFKD (compatability decomposition
*                   normalization) on UTF-8 text
*/

#ifdef _ICU_

#include <stdlib.h>
#include <string.h>

#ifdef __GNUC__
#include <unixstr.h>
#endif

#include <utf8nfkd.h>

UTF8NFKD::UTF8NFKD() {
        conv = ucnv_open("UTF-8", &err);
}

UTF8NFKD::~UTF8NFKD() {
         ucnv_close(conv);
}

char UTF8NFKD::ProcessText(char *text, int maxlen, const SWKey *key, const SWModule *module)
{
	 if ((unsigned long)key < 2)	// hack, we're en(1)/de(0)ciphering
		return -1;
        int32_t len = strlen(text) * 2;
        source = new UChar[len + 1]; //each char could become a surrogate pair

	// Convert UTF-8 string to UTF-16 (UChars)
        len = ucnv_toUChars(conv, source, len, text, -1, &err);
        target = new UChar[len + 1];

        //compatability decomposition
        unorm_normalize(source, len, UNORM_NFKD, 0, target, len, &err);

        ucnv_fromUChars(conv, text, maxlen, target, -1, &err);

        delete [] source;
        delete [] target;

	return 0;
}

#endif