#ifndef ROSE_BinaryAnalysis_ModelChecker_NullDerefTag_H
#define ROSE_BinaryAnalysis_ModelChecker_NullDerefTag_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/ModelChecker/Tag.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Types.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

/** Tag that describes a null pointer dereference. */
class NullDerefTag: public Tag {
public:
    using Ptr = NullDerefTagPtr;

private:
    const TestMode testMode_;                                    // may or must, but not off
    const IoMode ioMode_;                                        // read or write
    SgAsmInstruction* const insn_;                               // instruction where the dereference occurs (optional)
    const InstructionSemantics::BaseSemantics::SValuePtr addr_; // memory address that is considered to be null

protected:
    NullDerefTag() = delete;
    NullDerefTag(size_t nodeStep, TestMode, IoMode, SgAsmInstruction*,
                 const InstructionSemantics::BaseSemantics::SValuePtr &addr);
public:
    ~NullDerefTag();

    /** Allocating constructor.
     *
     *  Thread safety: This constructor is thread safe. */
    static Ptr instance(size_t nodeStep, TestMode, IoMode, SgAsmInstruction*,
                        const InstructionSemantics::BaseSemantics::SValuePtr &addr);

public:
    virtual std::string name() const override;
    virtual std::string printableName() const override;
    virtual void print(std::ostream&, const std::string &prefix) const override;
    virtual void toYaml(std::ostream&, const std::string &prefix) const override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
