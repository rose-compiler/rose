#ifndef OBJDUMPTOROSEBINARYAST_H
#define OBJDUMPTOROSEBINARYAST_H

// #include <rose.h>

namespace ObjdumpToRoseBinaryAst
   {
  // SgAsmFile* objdumpToRoseBinaryAst(const std::string& fileName);
     void objdumpToRoseBinaryAst(const std::string& fileName, SgAsmFile* file);
   }

// DQ: Is a using declaration appropriate here?
using ObjdumpToRoseBinaryAst::objdumpToRoseBinaryAst;

#endif // OBJDUMPTOROSEBINARYAST_H
