#ifndef ROSE_BinaryAnalysis_ModelChecker_NullDereferenceTag_H
#define ROSE_BinaryAnalysis_ModelChecker_NullDereferenceTag_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER

#include <Rose/BinaryAnalysis/ModelChecker/Tag.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/BasicTypes.h>

class SgAsmInstruction;

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

/** Tag that describes a null pointer dereference. */
class NullDereferenceTag: public Tag {
public:
    using Ptr = NullDereferenceTagPtr;

private:
    const TestMode testMode_;                                    // may or must, but not off
    const IoMode ioMode_;                                        // read or write
    SgAsmInstruction* const insn_;                               // instruction where the dereference occurs (optional)
    const InstructionSemantics::BaseSemantics::SValuePtr addr_; // memory address that is considered to be null

protected:
    NullDereferenceTag() = delete;
    NullDereferenceTag(size_t nodeStep, TestMode, IoMode, SgAsmInstruction*,
                       const InstructionSemantics::BaseSemantics::SValuePtr &addr);
public:
    ~NullDereferenceTag();

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
    virtual Sarif::ResultPtr toSarif(const Sarif::AnalysisPtr&) const override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
