#ifndef ROSE_BinaryAnalysis_ModelChecker_OobTag_H
#define ROSE_BinaryAnalysis_ModelChecker_OobTag_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/ModelChecker/Tag.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/Types.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

/** Tag that describes an out-of-bounds memory access. */
class OobTag: public Tag {
public:
    using Ptr = OobTagPtr;

private:
    const TestMode testMode_;                                    // may or must, but not off
    const IoMode ioMode_;                                        // read or write
    const SgAsmInstruction *insn_;                               // instruction where the oob access occurs (optional)
    const InstructionSemantics2::BaseSemantics::SValuePtr addr_; // memory address that is accessed

protected:
    OobTag() = delete;
    OobTag(size_t nodeStep, TestMode, IoMode, SgAsmInstruction*,
           const InstructionSemantics2::BaseSemantics::SValuePtr &addr);

public:
    ~OobTag();

    /** Allocating constructor.
     *
     *  Thread safety: This constructor is thread safe. */
    static Ptr instance(size_t nodeStep, TestMode, IoMode, SgAsmInstruction*,
                        const InstructionSemantics2::BaseSemantics::SValuePtr &addr);

public:
    virtual std::string printableName() const override;
    virtual void print(std::ostream&, const std::string &prefix) const override;
    virtual void toYaml(std::ostream&, const std::string &prefix) const override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
