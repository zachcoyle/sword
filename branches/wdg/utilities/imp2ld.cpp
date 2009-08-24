#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <rawld.h>
#include <rawld4.h>
#include <zld.h>
#include <zipcomprs.h>

using std::string;

#ifndef NO_SWORD_NAMESPACE
using sword::zLD;
using sword::ZipCompress;
using sword::RawLD4;
using sword::RawLD;
using sword::SWKey;
#endif


int main(int argc, char **argv) {

  const char * helptext ="imp2ld 1.0 Lexicon/Dictionary/Daily Devotional/Glossary module creation tool for the SWORD Project\n  usage:\n   %s <filename> [modname] [ 4 (default) | 2 | z - module driver]\n";

  signed long i = 0;
  string keybuffer;
  string entbuffer;
  string linebuffer;
  char modname[16];
  char links = 0;
  std::vector<string> linkbuffer;

  if (argc > 2) {
    strcpy (modname, argv[2]);
  }
  else if (argc > 1) {
    for (i = 0; (i < 16) && (argv[1][i]) && (argv[1][i] != '.'); i++) {
      modname[i] = argv[1][i];
    }
    modname[i] = 0;
  }
  else {
    fprintf(stderr, helptext, argv[0]);
    exit(-1);
  }

  std::ifstream infile(argv[1]);

  char mode = 1;
  if (argc > 3) {
    switch (*argv[3]) {
    case 'z':
      mode = 3;
      break;
    case '2':
      mode = 2;
      break;
    default:
      mode = 1;
    }
  }

  zLD* modZ;
  RawLD* mod2;
  RawLD4* mod4;
  SWKey* key;

  if (mode == 3) {
    zLD::createModule(modname);
    modZ = new zLD(modname, 0, 0, 30, new ZipCompress());
    key = modZ->CreateKey();
  }
  else if (mode == 2) {
    RawLD::createModule(modname);
    mod2 = new RawLD(modname);
    key = mod2->CreateKey();
  }
  else if (mode == 1) {
    RawLD4::createModule(modname);
    mod4 = new RawLD4(modname);
    key = mod4->CreateKey();
  }

  key->Persist(1);

  while (!infile.eof()) {
    std::getline(infile, linebuffer);
    if (linebuffer.substr(0,3) == "$$$") {
      if (keybuffer.size() && entbuffer.size()) {
	std::cout << keybuffer << std::endl;
	*key = keybuffer.c_str();

	if (mode == 3) {
	  modZ->setKey(*key);
	  modZ->setEntry(entbuffer.c_str(), entbuffer.size());
          for (i = 0; i < links; i++) {
                SWKey tmpkey = linkbuffer[i].c_str();
                modZ->linkEntry(&tmpkey);
                std::cout << "Linking: " << linkbuffer[i] << std::endl;
          }
	}
	else if (mode == 2) {
	  mod2->setKey(*key);
	  mod2->setEntry(entbuffer.c_str(), entbuffer.size());
          for (i = 0; i < links; i++) {
                SWKey tmpkey = linkbuffer[i].c_str();
                mod2->linkEntry(&tmpkey);
                std::cout << "Linking: " << linkbuffer[i] << std::endl;
          }
	}
	else if (mode == 1) {
	  mod4->setKey(*key);
	  mod4->setEntry(entbuffer.c_str(), entbuffer.size());
          for (i = 0; i < links; i++) {
                SWKey tmpkey = linkbuffer[i].c_str();
                mod4->linkEntry(&tmpkey);
                std::cout << "Linking: " << linkbuffer[i] << std::endl;
          }
	}
      }
      keybuffer = linebuffer.substr(3,linebuffer.size()) ;
      entbuffer.resize(0);
      linkbuffer.clear();
      links = 0;
    }
    else if (linebuffer.substr(0,3) == "%%%") {
      linkbuffer.push_back(linebuffer.substr(3,linebuffer.size()));
      links++;
    }
    else {
      entbuffer += linebuffer;
    }
  }

  //handle final entry
  if (keybuffer.size() && entbuffer.size()) {
    std::cout << keybuffer << std::endl;
    *key = keybuffer.c_str();

    if (mode == 3) {
	  modZ->setKey(*key);
          modZ->setEntry(entbuffer.c_str(), entbuffer.size());
          for (i = 0; i < links; i++) {
                SWKey tmpkey = linkbuffer[i].c_str();
                modZ->linkEntry(&tmpkey);
                std::cout << "Linking: " << linkbuffer[i] << std::endl;
          }
    }
    else if (mode == 2) {
	  mod2->setKey(*key);
          mod2->setEntry(entbuffer.c_str(), entbuffer.size());
          for (i = 0; i < links; i++) {
                SWKey tmpkey = linkbuffer[i].c_str();
                mod2->linkEntry(&tmpkey);
                std::cout << "Linking: " << linkbuffer[i] << std::endl;
          }

    }
    else if (mode == 1) {
          mod4->setKey(*key);
          mod4->setEntry(entbuffer.c_str(), entbuffer.size());
          for (i = 0; i < links; i++) {
                SWKey tmpkey = linkbuffer[i].c_str();
                mod4->linkEntry(&tmpkey);
                std::cout << "Linking: " << linkbuffer[i] << std::endl;
          }

    }
  }
  infile.close();

  return 0;
}