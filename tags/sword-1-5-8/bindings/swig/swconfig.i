%{
#include <stdio.h>
#include <string>
#include <map>
#include <defs.h>
#include <multimapwdef.h>
#include "swconfig.h"

using namespace sword;
using namespace std;
%}

%include "std_vector.i"
%include "std_string.i"
%include "typemaps.i"


typedef multimapwithdefault < SWBuf, SWBuf, less < SWBuf > > ConfigEntMap;
typedef map < SWBuf, ConfigEntMap, less < SWBuf > > SectionMap;

class SWConfig {
public:
  //member data
  SWBuf filename;
  SectionMap Sections;

  //member functions
  SWConfig(const char *ifilename);
  virtual ~ SWConfig();

  virtual void Load();
  virtual void Save();

%extend {
  void set(const char* group, const char* entry, const char* value) {
	self->Sections[group][entry] = value;
  };
  const char* get(const char* group, const char* entry) {
	return self->Sections[group][entry].c_str();
  };
}

};