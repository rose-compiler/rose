#ifndef ROSE_Partitioner2_ModulesMips_H
#define ROSE_Partitioner2_ModulesMips_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <Partitioner2/Modules.h>

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
    virtual std::vector<Function::Ptr> functions() const ROSE_OVERRIDE {
        return std::vector<Function::Ptr>(1, function_);
    }
    virtual bool match(const Partitioner&, rose_addr_t anchor) ROSE_OVERRIDE;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
