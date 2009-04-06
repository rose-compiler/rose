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

SgAsmBlock *
putInstructionsIntoBasicBlocks(const std::map<uint64_t, SgAsmBlock*>& basicBlocks,
                               const std::map<uint64_t, SgAsmInstruction*>& insns)
{
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
	// tps (6 Apr 2009) -- The following ASSERT was commented out. It is needed because otherwise
	// theBlock is NULL and insn->set_parent(theBlock) crashes.
        ROSE_ASSERT (theBlockIter != insnToBlock.end());
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

/** Using the list of function entry addresses, create an SgAsmBlock containing a list of SgAsmFunctionDeclaration objects,
 *  each of which has the list of basic blocks for the particular function. */
SgAsmBlock *
putInstructionsIntoFunctions(SgAsmBlock* blk, const DisassemblerCommon::FunctionStarts& functionStarts)
{
    map<uint64_t, SgAsmFunctionDeclaration*> funcs;
    SgAsmBlock* newBlk = new SgAsmBlock();
    for (DisassemblerCommon::FunctionStarts::const_iterator i = functionStarts.begin(); i != functionStarts.end(); ++i) {
        SgAsmFunctionDeclaration* f = new SgAsmFunctionDeclaration();
        f->set_address(i->first);
        f->set_name(i->second.name);
        f->set_reason(i->second.reason);
        funcs.insert(make_pair(i->first, f));
        newBlk->get_statementList().push_back(f);
        f->set_parent(newBlk);
    }

    const SgAsmStatementPtrList& bbs = blk->get_statementList();
    for (size_t i = 0; i < bbs.size(); ++i) {
        SgAsmBlock* bb = isSgAsmBlock(bbs[i]);
        if (!bb) continue;
        uint64_t addr = bb->get_address();
        // This is based on the algorithm in putInstructionsIntoBasicBlocks, with
        // the same restrictions.  This is not quite as good as what IDA does --
        // I've read that it takes control flow into account so functions can be
        // non-contiguous in memory.
        map<uint64_t, SgAsmFunctionDeclaration*>::const_iterator j = funcs.upper_bound(addr);
        SgAsmFunctionDeclaration* f = NULL;
        if (j == funcs.begin()) {
            f = new SgAsmFunctionDeclaration();
            f->set_address(addr);
            funcs.insert(make_pair(addr, f));
            newBlk->get_statementList().push_back(f);
            f->set_parent(newBlk);
        } else {
            --j;
            f = j->second;
        }
        ROSE_ASSERT (f);
        f->get_statementList().push_back(bb);
        bb->set_parent(f);
    }
    return newBlk;
}

} /*namespace*/

