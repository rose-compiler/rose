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
    Function::Ptr function_;
public:
    /** Allocating constructor. */
    static Ptr instance() { return Ptr(new MatchRetAddiu); }
    virtual std::vector<Function::Ptr> functions() const override {
        return std::vector<Function::Ptr>(1, function_);
    }
    virtual bool match(const Partitioner&, rose_addr_t anchor) override;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
