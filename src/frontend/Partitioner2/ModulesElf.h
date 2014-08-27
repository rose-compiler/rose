#ifndef ROSE_Partitioner2_ModulesElf_H
#define ROSE_Partitioner2_ModulesElf_H

#include <Partitioner2/BasicTypes.h>
#include <Partitioner2/Function.h>
#include <Partitioner2/Modules.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace ModulesElf {

/** Reads ELF .eh_frames to find function entry addresses.
 *
 *  Performs an AST traversal rooted at the specified @p ast to find ELF .eh_frames sections and returns a sorted list of
 *  functions at unique starting addresses.  The functions are not attached to the CFG/AUM.
 *
 * @{ */
std::vector<Function::Ptr> findErrorHandlingFunctions(SgAsmElfFileHeader*);
std::vector<Function::Ptr> findErrorHandlingFunctions(SgAsmInterpretation*);
size_t findErrorHandlingFunctions(SgAsmElfFileHeader*, std::vector<Function::Ptr>&);
/** @} */

/** Reads ELF PLT sections and returns a list of functions.
 *
 * @{ */
std::vector<Function::Ptr> findPltFunctions(const Partitioner&, SgAsmElfFileHeader*);
std::vector<Function::Ptr> findPltFunctions(const Partitioner&, SgAsmInterpretation*);
size_t findPltFunctions(const Partitioner&, SgAsmElfFileHeader*, std::vector<Function::Ptr>&);
/** @} */

/** Matches an ELF PLT entry.  The address through which the PLT entry branches is remembered. This address is typically an
 *  RVA which is added to the initial base address. */
struct PltEntryMatcher: public InstructionMatcher {
    rose_addr_t baseVa_;                                // base address for computing memAddress_
    rose_addr_t memAddress_;                            // address through which an indirect branch branches
    size_t nBytesMatched_;                              // number of bytes matched for PLT entry
public:
    PltEntryMatcher(rose_addr_t base): baseVa_(base), memAddress_(0), nBytesMatched_(0) {}
    static Ptr instance(rose_addr_t base) { return Ptr(new PltEntryMatcher(base)); }
    virtual bool match(const Partitioner*, rose_addr_t anchor);
    rose_addr_t memAddress() const { return memAddress_; }
    size_t nBytesMatched() const { return nBytesMatched_; }
};



} // namespace
} // namespace
} // namespace
} // namespace

#endif
