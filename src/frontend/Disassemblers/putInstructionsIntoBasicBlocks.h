/* Included by rose.h */
#ifndef PutInstructionsIntoBasicBlocks_H
#define PutInstructionsIntoBasicBlocks_H

namespace PutInstructionsIntoBasicBlocks {

SgAsmBlock *putInstructionsIntoBasicBlocks(const std::map<uint64_t, SgAsmBlock*>& basicBlocks,
                                           const std::map<uint64_t, SgAsmInstruction*>& insns);

SgAsmBlock * putInstructionsIntoFunctions(SgAsmBlock* blk, const DisassemblerCommon::FunctionStarts& functionStarts);

}

#endif /*PutInstructionsIntoBasicBlocks_H*/
