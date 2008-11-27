#include <iostream>
#include <string>

#include "unicode/udata.h"   /* Data structures */
#include "unicode/ures.h"   /* Data structures */
#include "unicode/utypes.h"   /* Basic ICU data types */
#include "unicode/ucnv.h"     /* C   Converter API    */
#include "unicode/convert.h"  /* C++ Converter API    */
#include "unicode/ustring.h"  /* some more string fcns*/

#include "unicode/translit.h"

#include "utf8transliterator.h"

// Print the given string to stdout
void uprintf(const UnicodeString &str) {
    char *buf = 0;
    int32_t len = str.length();
    // int32_t bufLen = str.extract(0, len, buf); // Preflight
    /* Preflighting seems to be broken now, so assume 1-1 conversion,
       plus some slop. */
    int32_t bufLen = len + 16;
        int32_t actualLen;
    buf = new char[bufLen + 1];
    actualLen = str.extract(0, len, buf/*, bufLen*/); // Default codepage conversion
    buf[actualLen] = 0;
    //printf("%s", buf);
    std::cout << buf;
    delete buf;
}


int main() {


  UErrorCode status = U_ZERO_ERROR;
  UDataMemory *pappData = udata_open("/usr/local/lib/sword/swicu", 
	"res", "root", &status);
  if (U_FAILURE(status)) 
  {
  	std::cout << "error: " << status << ":" << 
		u_errorName(status) << std::endl;
	return 0;
  } 
  UChar * uBuf;
  UChar * target;
  UConverter *conv;
  //UParseError perr = U_ZERO_ERROR;
  int32_t uBufSize = 0, uLength = 0;
  void * pAppData=NULL;
  char * samplestring = "If this compiles and runs without errors, apparently ICU is working.";
  //ures_open("/usr/local/lib/sword/swicu.dat", 
 // 	NULL, &status);
  //UDataMemory *pappData = udata_open("/usr/local/lib/sword/swicu", 
//	"res", "root", &status);
  if (U_FAILURE(status)) 
  {
  	std::cout << "error: " << status << ":" << 
		u_errorName(status) << std::endl;
	return 0;
  } 
  //UDataMemory *pappData2 = udata_open("/usr/local/lib/sword/swicu", 
//	"res", "translit_Latin_Gothic", &status);
  std::cout << status << std::endl; 
  if (U_FAILURE(status)) 
  {
  	std::cout << "error: " << status << ":" << 
		u_errorName(status) << std::endl;
	return 0;
  } 
  std::cout << "available " << Transliterator::countAvailableIDs() << std::endl;
  //udata_setAppData("/usr/local/lib/sword/swicu.dat" , pAppData, &status);
  //if (U_FAILURE(status)) 
  //{
  	//std::cout << "error: " << status << ":" << 
	//	u_errorName(status) << std::endl;
	//return 0;
  //} 

  int32_t i_ids = Transliterator::countAvailableIDs();
  
  std::cout << "available " << i_ids << std::endl;
  for (int i=0; i<i_ids;i++)
  {
	std::cout << "id " << i << ": ";
	uprintf(Transliterator::getAvailableID(i));
	std::cout << std::endl;
  } 


  //UTF8Transliterator utran = new UTF8Transliterator();
  std::cout << "creating transliterator 2" << std::endl;
  Transliterator *btrans = Transliterator::createInstance("NFD;Latin-Greek;NFC", 
	UTRANS_FORWARD, status);
  if (U_FAILURE(status)) 
  {
  	std::cout << "error: " << status << ":" << 
		u_errorName(status) << std::endl;
	return 0;
  } 
  std::cout << "creating transliterator 1" << std::endl;
  Transliterator *trans = Transliterator::createInstance("NFD;Latin-Gothic;NFC", 
	UTRANS_FORWARD, status);
  if (U_FAILURE(status)) 
  {
	delete btrans;
  	std::cout << "error: " << status << ":" << 
		u_errorName(status) << std::endl;
	return 0;
  } 

  std::cout << "deleting transliterator 1" << std::endl;
  delete trans;
  std::cout << "deleting transliterator 2" << std::endl;
  delete btrans;
  std::cout << "the rest" << std::endl;
  uLength = strlen(samplestring);
  conv = ucnv_open("utf-8", &status);		
  if (U_FAILURE(status)) 
  {
  	std::cout << "error: " << status << ":" << 
		u_errorName(status) << std::endl;
	return 0;
  } 
  uBufSize = (uLength/ucnv_getMinCharSize(conv));
  uBuf = (UChar*)malloc(uBufSize * sizeof(UChar));
  
  target = uBuf;
  
  ucnv_toUChars(conv, target, uLength, 
		samplestring, uLength, &status);
  if (U_FAILURE(status)) 
  {
  	std::cout << "error: " << status << ":" << 
		u_errorName(status) << std::endl;
	return 0;
  } 

  cout << samplestring << endl;

  return 0;
}