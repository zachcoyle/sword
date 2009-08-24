/******************************************************************************
 *
 * OSISRedLetterWords -	SWFilter descendant to toggle red coloring for words
 *			of Christ in an OSIS module.
 */


#include <stdlib.h>
#include <osisredletterwords.h>
#include <swmodule.h>
#include <utilxml.h>


SWORD_NAMESPACE_START

const char oName[] = "Words of Christ in Red";
const char oTip[] = "Toggles Red Coloring for Words of Christ On and Off if they are marked";

const SWBuf choices[3] = {"Off", "On", ""};
const StringList oValues(&choices[0], &choices[2]);

OSISRedLetterWords::OSISRedLetterWords() : SWOptionFilter(oName, oTip, &oValues) {
	setOptionValue("On");
}


OSISRedLetterWords::~OSISRedLetterWords() {
}


char OSISRedLetterWords::processText(SWBuf &text, const SWKey *key, const SWModule *module) {
	if (option) //leave in the red lettered words
		return 0;
	
	SWBuf token;
	bool intoken    = false;
	char buf[254];

	SWBuf orig = text;
	const char *from = orig.c_str();

	//taken out of the loop
	const char* start = 0;
	const char* end = 0;
		
	for (text = ""; *from; from++) {
		if (*from == '<') {
			intoken = true;
			token = "";
			continue;
		}
		else if (*from == '>') {	// process tokens
			intoken = false;

			if ((token[0] == 'q') && (token[1] == ' ')) { //q tag
				start = strstr(token.c_str(), " who=\"Jesus\"");
				if (start && (strlen(start) >= 12)) { //we found a quote of Jesus Christ
					end = start+12; //marks the end of the who attribute value
					
					text.append('<');
					text.append(token, start - (token.c_str())); //the text before the who attr
					text.append(end, token.c_str() + token.length() - end);  //text after the who attr
					text.append('>');
					
					continue;
				}
			}
			
			//token not processed, append it. We don't want to alter the text
			text.append('<');
			text.append(token);
			text.append('>');
			continue;
		}
		
		if (intoken) { //copy token
			token.append(*from);
		}
		else { //copy text which is not inside a token
			text.append(*from);
		}
	}
	return 0;
}

SWORD_NAMESPACE_END
