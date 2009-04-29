#ifndef _ORBIT_CPP_IDL_sword_IMPL_HH
#define _ORBIT_CPP_IDL_sword_IMPL_HH

#include "swordorb-cpp-skels.h"
#include <swmodule.h>
#include <swmgr.h>
#include <rawtext.h>
#include <map>


namespace swordorb {

static const char *SWNULL = "<SWNULL>";
extern sword::RawText NULLMod;

//Inherit from abstract Skeleton:
class SWModule_impl : public POA_swordorb::SWModule {
	sword::SWModule *delegate;
public:
	SWModule_impl(sword::SWModule *delegate)  { this->delegate = delegate; }

	StringList *search(const char *istr, SearchType searchType, CORBA::Long flags, const char *scope) throw(CORBA::SystemException);
	void   terminateSearch() throw(CORBA::SystemException) { delegate->terminateSearch = true; }
	char   error() throw(CORBA::SystemException) { return delegate->Error(); }
	CORBA::Long getEntrySize() throw(CORBA::SystemException) { return delegate->getEntrySize(); }
	void   setKeyText(const char *key) throw(CORBA::SystemException) { delegate->KeyText(key); }
	char *getKeyText() throw(CORBA::SystemException) { return CORBA::string_dup((char *)delegate->KeyText()); }
	StringList *getKeyChildren() throw(CORBA::SystemException);
	char *getName() throw(CORBA::SystemException) { return CORBA::string_dup((char *)delegate->Name()); }
	char *getDescription() throw(CORBA::SystemException) { return CORBA::string_dup((char *)delegate->Description()); }
	char *getCategory() throw(CORBA::SystemException);
	void   previous() throw(CORBA::SystemException) { delegate->decrement(); }
	void   next() throw(CORBA::SystemException) { delegate->increment(); }
	void   begin() throw(CORBA::SystemException) { delegate->setPosition(sword::TOP); }
	char *getStripText() throw(CORBA::SystemException) { return CORBA::string_dup((char *)delegate->StripText()); }
	char *getRenderText() throw(CORBA::SystemException) { return CORBA::string_dup((char *)delegate->RenderText()); }
	char *getConfigEntry(const char *key) throw(CORBA::SystemException) { return CORBA::string_dup(((char *)delegate->getConfigEntry(key)) ? (char *)delegate->getConfigEntry(key):SWNULL); }

};

typedef std::map<std::string, SWModule_impl *> SWModuleMap;

class SWMgr_impl : public POA_swordorb::SWMgr {
	sword::SWMgr *delegate;
	SWModuleMap moduleImpls;
public:
	SWMgr_impl(sword::SWMgr *delegate)  { this->delegate = delegate; }

	ModInfoList *getModInfoList() throw(CORBA::SystemException);
	SWModule_ptr getModuleByName(const char *name) throw(CORBA::SystemException);
	char *getPrefixPath() throw(CORBA::SystemException) { return CORBA::string_dup(delegate->prefixPath); }
	char *getConfigPath() throw(CORBA::SystemException) { return CORBA::string_dup(delegate->configPath); }
	void  setGlobalOption(const char *option, const char *value) throw(CORBA::SystemException) { delegate->setGlobalOption(option, value); }
	char *getGlobalOption(const char *option) throw(CORBA::SystemException) { return CORBA::string_dup((char *)delegate->getGlobalOption(option)); }
	char *getGlobalOptionTip(const char *option) throw(CORBA::SystemException) { return CORBA::string_dup((char *)delegate->getGlobalOptionTip(option)); }
	StringList *getGlobalOptions() throw(CORBA::SystemException);
	StringList *getGlobalOptionValues(const char *option) throw(CORBA::SystemException);
	void     setCipherKey(const char *modName, const char *key) throw(CORBA::SystemException) { delegate->setCipherKey(modName, key); }
	void     terminate() throw(CORBA::SystemException);
	CORBA::Boolean     testConnection() throw(CORBA::SystemException);

};
}; // namespace hellomodule


#endif //_ORBIT_CPP_IDL_helloworld_IMPL_HH