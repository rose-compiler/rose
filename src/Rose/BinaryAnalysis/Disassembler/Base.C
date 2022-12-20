#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>

#include <Assembler.h>
#include <AssemblerX86.h>
#include <Rose/BinaryAnalysis/BinaryLoader.h>
#include <Rose/BinaryAnalysis/Disassembler/Exception.h>
#include <Rose/Diagnostics.h>

#include <integerOps.h>
#include <stringify.h>

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/once.hpp>
#include <stdarg.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Disassembler {

using namespace Diagnostics;
using namespace StringUtility;

size_t
Base::instructionAlignment() const {
    ASSERT_require(instructionAlignment_ > 0);
    return instructionAlignment_;
}

/* Disassemble one instruction. */
SgAsmInstruction *
Base::disassembleOne(const unsigned char *buf, rose_addr_t buf_va, size_t buf_size, rose_addr_t start_va,
                             AddressSet *successors)
{
    MemoryMap::Ptr map = MemoryMap::instance();
    map->insert(AddressInterval::baseSize(buf_va, buf_size),
                MemoryMap::Segment::staticInstance(buf, buf_size, MemoryMap::READABLE|MemoryMap::EXECUTABLE,
                                                   "disassembleOne temp"));
    return disassembleOne(map, start_va, successors);
}

SgAsmInstruction *
Base::find_instruction_containing(const InstructionMap &insns, rose_addr_t va)
{
    const size_t max_insns_size = 16;
    InstructionMap::const_iterator ii=insns.upper_bound(va);
    if (ii==insns.begin())
        return NULL;
    while (1) {
        --ii;
        ASSERT_require(ii->first <= va);
        if (ii->first + max_insns_size < va)
            return NULL;
        if (ii->first + ii->second->get_size() > va)
            return ii->second;
        if (ii==insns.begin())
            return NULL;
    }
}

/* Re-read instruction bytes from file if necessary in order to mark them as referenced. */
void
Base::mark_referenced_instructions(SgAsmInterpretation *interp, const MemoryMap::Ptr &map, const InstructionMap &insns)
{
    unsigned char buf[32];
    SgAsmGenericFile *file = NULL;
    const SgAsmGenericFilePtrList &files = interp->get_files();
    bool was_tracking = false; // only valid when file!=NULL  (value here is to shut of used-before-defined warnings from GCC)
    MemoryMap::ConstNodeIterator si = map->nodes().end();

    /* Re-read each instruction so the file has a chance to track the reference. */
    try {
        for (InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
            SgAsmInstruction *insn = ii->second;
            ASSERT_require(insn->get_size()<=sizeof buf);
            rose_addr_t va = insn->get_address();
            size_t nbytes = insn->get_size();

            while (nbytes>0) {
                /* Find the memory map segment and the file that goes with that segment (if any) */
                if (si==map->nodes().end() || !si->key().contains(va)) {
                    if (file) {
                        file->set_tracking_references(was_tracking);
                        file = NULL;
                    }
                    si = map->at(va).findNode();
                    if (si==map->nodes().end()) {
                        /* This byte of the instruction is not mapped. Perhaps the next one is. */
                        ++va;
                        --nbytes;
                        continue;
                    }

                    /* Find the file that goes with this segment. */
                    for (size_t i=0; i<files.size(); i++) {
                        if (&(files[i]->get_data()[0]) == si->value().buffer()->data()) {
                            file = files[i];
                            was_tracking = file->get_tracking_references();
                            file->set_tracking_references(true);
                            break;
                        }
                    }
                }

                /* Read the file for its reference tracking side effect. */
                size_t sgmt_offset = va - si->key().least();
                size_t n = std::min(nbytes, (size_t)si->key().size()-sgmt_offset);
                if (file) {
                    size_t file_offset = si->value().offset() + sgmt_offset;
                    file->read_content(file_offset, buf, n, false);
                }
                nbytes -= n;
                va += n;
            }
        }
        if (file)
            file->set_tracking_references(was_tracking);
    } catch(...) {
        if (file)
            file->set_tracking_references(was_tracking);
        throw;
    }
}

/* Add last instruction's successors to returned successors. */
AddressSet
Base::get_block_successors(const InstructionMap& insns, bool &complete)
{
    std::vector<SgAsmInstruction*> block;
    for (InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii)
        block.push_back(ii->second);
    AddressSet successors = block.front()->getSuccessors(block, complete);

    /* For the purposes of disassembly, assume that a CALL instruction eventually executes a RET that causes execution to
     * resume at the address following the CALL. This is true 99% of the time.  Higher software layers (e.g., Partitioner) may
     * make other assumptions, which is why this code is not in SgAsmX86Instruction::getSuccessors(). [RPM 2010-05-09] */
    rose_addr_t target, return_va;
    SgAsmInstruction *last_insn = block.back();
    if (last_insn->isFunctionCallSlow(block, &target, &return_va))
        successors.insert(return_va);

    return successors;
}

} // namespace
} // namespace
} // namespace

#endif
