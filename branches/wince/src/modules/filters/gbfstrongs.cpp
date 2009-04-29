/******************************************************************************
 *
 * gbfstrongs -	SWFilter decendant to hide or show strongs number
 *			in a GBF module.
 */


#include <stdlib.h>
#include <string.h>
#include <gbfstrongs.h>
#include <swmodule.h>
#ifndef __GNUC__
#else
#include <unixstr.h>
#endif
#include <ctype.h>

SWORD_NAMESPACE_START

const char GBFStrongs::on[] = "On";
const char GBFStrongs::off[] = "Off";
const char GBFStrongs::optName[] = "Strong's Numbers";
const char GBFStrongs::optTip[] = "Toggles Strong's Numbers On and Off if they exist";


GBFStrongs::GBFStrongs() {
	option = false;
	options.push_back(on);
	options.push_back(off);
}


GBFStrongs::~GBFStrongs() {
}

void GBFStrongs::setOptionValue(const char *ival)
{
	option = (!stricmp(ival, on));
}

const char *GBFStrongs::getOptionValue()
{
	return (option) ? on:off;
}

char GBFStrongs::ProcessText(char *text, int maxlen, const SWKey *key, const SWModule *module)
{
	char *to, *from, token[2048]; // cheese.  Fix.
	int tokpos = 0;
	bool intoken = false;
	int len;
	bool lastspace = false;
	int word = 1;
	char val[128];
	char wordstr[5];
	char *valto;
	char *ch;
	char *textStart = text, *textEnd = 0;
	bool newText = false;
	std::string tmp;

	len = strlen(text) + 1;	// shift string to right of buffer
	if (len < maxlen) {
		memmove(&text[maxlen - len], text, len);
		from = &text[maxlen - len];
	}
	else	from = text;	// -------------------------------

	for (to = text; *from; from++) {
		if (*from == '<') {
			intoken = true;
			tokpos = 0;
			token[0] = 0;
			token[1] = 0;
			token[2] = 0;
			textEnd = to;
			continue;
		}
		if (*from == '>') {	// process tokens
			intoken = false;
			if (*token == 'W' && (token[1] == 'G' || token[1] == 'H')) {	// Strongs
				if (module->isProcessEntryAttributes()) {
					valto = val;
					for (unsigned int i = 2; ((token[i]) && (i < 150)); i++)
						*valto++ = token[i];
					*valto = 0;
					if (atoi((!isdigit(*val))?val+1:val) < 5627) {
						// normal strongs number
						sprintf(wordstr, "%03d", word++);
						module->getEntryAttributes()["Word"][wordstr]["Strongs"] = val;
						tmp = "";
						tmp.append(textStart, (int)(textEnd - textStart));
						module->getEntryAttributes()["Word"][wordstr]["Text"] = tmp;
						newText = true;
					}
					else {
						// verb morph
						sprintf(wordstr, "%03d", word-1);
						module->getEntryAttributes()["Word"][wordstr]["Morph"] = val;
					}
				}
				if (!option) {
					if ((from[1] == ' ') || (from[1] == ',') || (from[1] == ';') || (from[1] == '.') || (from[1] == '?') || (from[1] == '!') || (from[1] == ')') || (from[1] == '\'') || (from[1] == '\"')) {
						if (lastspace)
							to--;
					}
					if (newText) {textStart = to; newText = false; }
					continue;
				}
			}
			// if not a strongs token, keep token in text
			*to++ = '<';
			for (char *tok = token; *tok; tok++)
				*to++ = *tok;
			*to++ = '>';
			if (newText) {textStart = to; newText = false; }
			continue;
		}
		if (intoken) {
			if (tokpos < 2045)
				token[tokpos++] = *from;
				token[tokpos+2] = 0;
		}
		else	{
			*to++ = *from;
			lastspace = (*from == ' ');
		}
	}
	*to++ = 0;
	*to = 0;
	return 0;
}

SWORD_NAMESPACE_END