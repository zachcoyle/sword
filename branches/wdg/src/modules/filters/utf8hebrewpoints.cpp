/******************************************************************************
 *
 * UTF8HebrewPoints - SWFilter descendant to remove UTF-8 Hebrew vowel points
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <utf8hebrewpoints.h>

SWORD_NAMESPACE_START

const char oName[] = "Hebrew Vowel Points";
const char oTip[] = "Toggles Hebrew Vowel Points";

const SWBuf choices[3] = {"On", "Off", ""};
const StringList oValues(&choices[0], &choices[2]);

UTF8HebrewPoints::UTF8HebrewPoints() : SWOptionFilter(oName, oTip, &oValues) {
	setOptionValue("On");
}

UTF8HebrewPoints::~UTF8HebrewPoints(){};


char UTF8HebrewPoints::processText(SWBuf &text, const SWKey *key, const SWModule *module) {
	if (!option) {
		//The UTF-8 range 0xD6 0xB0 to 0xD6 0xBF excluding 0xD6 0x consist of Hebrew cantillation marks so block those out.
		SWBuf orig = text;
		const unsigned char* from = (unsigned char*)orig.c_str();
		for (text = ""; *from; from++) {
			if ((*from == 0xD6) && (*(from + 1) >= 0xB0 && *(from + 1) <= 0xBF) && (*(from + 1) != 0xBE)) {
				from++;
			}
			else {
     			        text += *from;
                        }
		}
     	}
	return 0;
}

SWORD_NAMESPACE_END
