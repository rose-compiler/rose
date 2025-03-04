#ifndef ROSE_BinaryAnalysis_Partitioner2_ModulesMips_H
#define ROSE_BinaryAnalysis_Partitioner2_ModulesMips_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Partitioner2/Modules.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Disassembly and partitioning utility functions for MIPS. */
namespace ModulesMips {

/** Matches a MIPS function prologue that's a return followed by ADDIU instruction. */
class MatchRetAddiu: public FunctionPrologueMatcher {
protected:
    FunctionPtr function_;
protected:
    MatchRetAddiu();
public:
    ~MatchRetAddiu();

public:
    static Ptr instance();                              /**< Allocating constructor. */
    virtual std::vector<FunctionPtr> functions() const override;
    virtual bool match(const PartitionerConstPtr&, Address anchor) override;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
