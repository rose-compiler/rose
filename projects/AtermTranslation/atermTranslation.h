#ifndef ATERMTRANSLATION_H
#define ATERMTRANSLATION_H


#include "aterm1.h"
#include <iostream>
#include <sstream>
#include <iomanip>

// From astToAtermImpl.C:
ATerm convertFileInfoToAterm(Sg_File_Info* fi);
ATerm convertNodeToAterm(SgNode* n);
ATerm convertVectorToAterm(const vector<ATerm>& v,
                           int start = 0, int len = -1);

inline std::string intToHex(unsigned int x) {
  std::ostringstream s;
  s << "0x" << std::hex << std::setw(8) << std::setfill('0') << x;
  return s.str();
}

inline std::string intToHex(void* x) {
  return intToHex((unsigned int)x);
}

inline ATerm pointerAsAterm(SgNode* n) {
  return ATmake("<str>", intToHex(n).c_str());
}

inline SgNode* atermAsPointer(ATerm t) {
  char* str;
  if (!ATmatch(t, "<str>", &str)) return 0;
  std::istringstream s(str);
  void* n;
  s >> n;
  if (!s) return 0;
  return (SgNode*)n;
}

// From atermToAstImpl.C
void cleanAstMadeFromAterm(SgNode* n);
Sg_File_Info* getAtermFileInfo(ATerm term);
SgNode* convertAtermToNode(ATerm term);
std::vector<ATerm> getAtermList(ATerm ls);
#endif // ATERMTRANSLATION_H
