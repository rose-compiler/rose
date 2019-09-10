#ifndef ROSE_Partitioner2_ModulesPowerpc_H
#define ROSE_Partitioner2_ModulesPowerpc_H

#include <Partitioner2/Modules.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Disassembly and partitioning utilities for PowerPC. */
namespace ModulesPowerpc {

/** Matches a PowerPC function prolog using STWU. */
class MatchStwuPrologue: public FunctionPrologueMatcher {
protected:
    Function::Ptr function_;
public:
    static Ptr instance() { return Ptr(new MatchStwuPrologue); } /**< Allocating constructor. */
    virtual std::vector<Function::Ptr> functions() const ROSE_OVERRIDE;
    virtual bool match(const Partitioner&, rose_addr_t anchor) ROSE_OVERRIDE;
};

/** Matches a PowerPC ELF dynamic function stub.
 *
 *  If the function's instructions make it look like an ELF dynamic linking stub and it references an entry in the Procedure
 *  Loookup Table (defined by the PLT location in memory), then return the PLT entry address, otherwise return nothing. */
Sawyer::Optional<rose_addr_t>
matchElfDynamicStub(const Partitioner&, const Function::Ptr&, const AddressIntervalSet &pltAddresses);

/** Give names to thunks for dynamically linked functions. */
void nameImportThunks(const Partitioner&, SgAsmInterpretation*);

} // namespace
} // namespace
} // namespace
} // namespace

#endif
