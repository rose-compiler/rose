#ifndef ATERM_TRANSLATION_H
#define ATERM_TRANSLATION_H

#include "aterm1.h"
#include <iostream>
#include <sstream>
#include <iomanip>

// From astToAtermImpl.C:
ATerm convertFileInfoToAterm(Sg_File_Info* fi);
ATerm convertNodeToAterm(SgNode* n);
ATerm convertVectorToAterm(const std::vector<ATerm>& v, int start = 0, int len = -1);

inline std::string intToHex(unsigned long x) 
   {
#if 0
     printf ("In intToHex(unsigned int x): TOP: x = %zu = %p \n",x,(void*)x);
#endif

     std::ostringstream s;
  // s << "0x" << std::hex << std::setw(8) << std::setfill('0') << x;
     s << "0x" << std::hex << std::setw(8) << std::setfill('0') << x;

     std::string str = s.str();

#if 0
     printf ("In intToHex(unsigned int x): str = %s \n",str.c_str());
#endif

     return s.str();
   }

inline std::string intToHex(void* x) 
   {
  // return intToHex((unsigned int)x);
  // return intToHex((unsigned long)x);
     std::string s = intToHex((unsigned long)x);

#if 0
     printf ("In intToHex(void* x): s = %s \n",s.c_str());
#endif

     return s;
   }

inline ATerm pointerAsAterm(SgNode* n) 
   {
#if 0
     printf ("In pointerAsAterm(): n = %p = %s \n",n,n->class_name().c_str());
#endif

     return ATmake("<str>", intToHex(n).c_str());
   }

inline SgNode* atermAsPointer(ATerm t) 
   {
     char* str;

     if (!ATmatch(t, "<str>", &str)) return 0;
     std::istringstream s(str);
     void* n;
     s >> n;
     if (!s) return 0;
     return (SgNode*)n;
   }

#if 0
// DQ (10/25/2013): I could not make this work...
template <class T>
T strToNum(const std::string &inputString, std::ios_base &(*f)(std::ios_base&) = std::dec)
   {
  // DQ (10/24/2013): I can't seem to make this work, it always fails as an invalide conversion.
     T t;
     std::istringstream stringStream(inputString);

     if ((stringStream >> f >> t).fail())
        {
       // throw runtime_error("Invalid conversion");
          printf ("Error: Invalid conversion \n");
          ROSE_ASSERT(false);
        }

     return t;
   }
#endif

#if 0
// Example usage
unsigned long ulongValue = strToNum<unsigned long>(strValue);
int intValue             = strToNum<int>(strValue);

int intValueFromHex      = strToNum<int>(strHexValue,std::hex);
unsigned long ulOctValue = strToNum<unsigned long>(strOctVal, std::oct);
#endif

// From atermToAstImpl.C
void cleanAstMadeFromAterm(SgNode* n);
Sg_File_Info* getAtermFileInfo(ATerm term);
SgNode* convertAtermToNode(ATerm term);
std::vector<ATerm> getAtermList(ATerm ls);

// endif for ATERM_TRANSLATION_H
#endif
