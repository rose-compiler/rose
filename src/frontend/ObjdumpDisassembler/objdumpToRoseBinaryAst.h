#ifndef OBJDUMPTOROSEBINARYAST_H
#define OBJDUMPTOROSEBINARYAST_H

// #include <rose.h>

namespace ObjdumpToRoseBinaryAst
   {
  // SgAsmFile* objdumpToRoseBinaryAst(const std::string& fileName);
     void objdumpToRoseBinaryAst(const std::string& fileName,
                                 SgAsmFile* file,
                                 SgProject* proj);

  // Private function used by X86Disassembler
     SgAsmBlock*
       putInstructionsIntoBasicBlocks(
           const std::map<uint64_t, SgAsmBlock*>& basicBlocks,
           const std::map<uint64_t, SgAsmInstruction*>& insns);
   }

// DQ: Is a using declaration appropriate here?
using ObjdumpToRoseBinaryAst::objdumpToRoseBinaryAst;

#endif // OBJDUMPTOROSEBINARYAST_H
