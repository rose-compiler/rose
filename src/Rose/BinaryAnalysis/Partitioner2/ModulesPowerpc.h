#ifndef ROSE_BinaryAnalysis_Partitioner2_ModulesPowerpc_H
#define ROSE_BinaryAnalysis_Partitioner2_ModulesPowerpc_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Partitioner2/Modules.h>

#include <boost/logic/tribool.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Disassembly and partitioning utilities for PowerPC. */
namespace ModulesPowerpc {

/** Matches a PowerPC function prolog using STWU. */
class MatchStwuPrologue: public FunctionPrologueMatcher {
protected:
    FunctionPtr function_;
protected:
    MatchStwuPrologue();
public:
    ~MatchStwuPrologue();

public:
    static Ptr instance();                              /**< Allocating constructor. */
    virtual std::vector<FunctionPtr> functions() const override;
    virtual bool match(const PartitionerConstPtr&, Address anchor) override;
};

/** Matches a PowerPC ELF dynamic function stub.
 *
 *  If the function's instructions make it look like an ELF dynamic linking stub and it references an entry in the Procedure
 *  Loookup Table (defined by the PLT location in memory), then return the PLT entry address, otherwise return nothing. */
Sawyer::Optional<Address>
matchElfDynamicStub(const PartitionerConstPtr&, const FunctionPtr&, const AddressIntervalSet &pltAddresses);

/** Give names to thunks for dynamically linked functions. */
void nameImportThunks(const PartitionerConstPtr&, SgAsmInterpretation*);

/** Determines whether basic block is a function return.
 *
 *  Returns true if the basic block appears to be a function return, semantically. Indeterminate if semantics are disabled. */
boost::logic::tribool isFunctionReturn(const PartitionerConstPtr&, const BasicBlockPtr&);
    
} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
