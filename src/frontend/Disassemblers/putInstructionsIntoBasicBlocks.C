#include <rose.h>
#include <string>
#include <stack>
#include <map>
#include <cstdio>
#include <stdint.h>
#include <errno.h>

#include "RoseBin_support.h"

using namespace std;

namespace PutInstructionsIntoBasicBlocks {

  SgAsmBlock* putInstructionsIntoBasicBlocks(
      const std::map<uint64_t, SgAsmBlock*>& basicBlocks,
      const std::map<uint64_t, SgAsmInstruction*>& insns) {
    // Put each instruction into the proper block, and link the blocks together
    // SgAsmFile* file = new SgAsmFile();
    SgAsmBlock* mainBlock = new SgAsmBlock();

    map<uint64_t, SgAsmBlock*> insnToBlock;

    // fprintf(stderr, "Putting blocks in file\n");
    for (map<uint64_t, SgAsmBlock*>::const_iterator i = basicBlocks.begin(); i != basicBlocks.end(); ++i) {
      mainBlock->get_statementList().push_back(i->second);
      i->second->set_parent(mainBlock);
      // Every basic block start is in its own block
      insnToBlock[i->first] = i->second;
      // fprintf(stderr, "Found basic block start 0x%08X as %p\n", (unsigned int)(i->first), i->second);
    }

    // fprintf(stderr, "Putting instructions in blocks\n");
    // This stuff relies on the maps being sorted -- hash_maps won't work
    for (map<uint64_t, SgAsmInstruction*>::const_iterator i = insns.begin(); i != insns.end(); ++i) {
      uint64_t addr = i->first;
      SgAsmInstruction* insn = i->second;
      map<uint64_t, SgAsmBlock*>::const_iterator theBlockIter = insnToBlock.lower_bound(addr);
      if (theBlockIter == insnToBlock.end()) {
        cerr << "Found instruction that is after the last block: " << addr << endl;
        abort();
      }
      // ROSE_ASSERT (theBlockIter != insnToBlock.end());
      SgAsmBlock* theBlock = theBlockIter->second;
      insn->set_parent(theBlock);
      theBlock->get_statementList().push_back(insn);
      // fprintf(stderr, "Put insn 0x%08X into block %p\n", addr, theBlock);
      // ROSE_ASSERT (nextAddress.find(addr) != nextAddress.end());
      uint64_t next_addr = addr + insn->get_raw_bytes().size();
      if (next_addr != 0 && insnToBlock.find(next_addr) == insnToBlock.end()) {
        // Set the value for the next instruction if it does not start its own basic block
        insnToBlock[next_addr] = theBlock;
      }
    }
    return mainBlock;
  }

}
