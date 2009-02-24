#include "rose.h"
#include "sage3.h"
#include "AST_FILE_IO.h"

using namespace std;

std::ostream& operator<<(std::ostream& os, SgName& n) {
   return os << "\"" << n.str() << "\"";
}
std::ostream& operator<<(std::ostream& os, SgAsmStmt::AsmRegisterNameList & bv) 
   {
      for (unsigned int i=0; i < bv.size(); i++) {
        if (i != 0) os << ", ";
        os << ((long)(bv[i]));
      }
     return os;
   }
std::ostream& operator<<(std::ostream& os, SgDataStatementObjectPtrList& bv) 
   {
      return os;
   }
std::ostream& operator<<(std::ostream& os, SgDataStatementValuePtrList& bv) 
   {
      return os;
   }
std::ostream& operator<<(std::ostream& os, SgCommonBlockObjectPtrList& bv) 
   {
      return os;
   }
std::ostream& operator<<(std::ostream& os, SgDimensionObjectPtrList& bv) 
   {
      return os;
   }
std::ostream& operator<<(std::ostream& os, SgLabelSymbolPtrList& bv) 
   {
      return os;
   }
std::ostream& operator<<(std::ostream& os, SgFormatItemPtrList& bv) 
   {
      return os;
   }

void doRTI(const char* fieldNameBase, void* fieldPtr, size_t fieldSize, void* thisPtr, const char* className, const char* typeString, const char* fieldName, const std::string& fieldContents, RTIMemberData& memberData) {
#if ROSE_USE_VALGRIND
  doUninitializedFieldCheck(fieldNameBase, fieldPtr, fieldSize, thisPtr, className);
#endif
  memberData = RTIMemberData(typeString, fieldName, fieldContents);
}

