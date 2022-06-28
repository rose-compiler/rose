#ifndef ROSE_BinaryAnalysis_ModelChecker_ErrorTag_H
#define ROSE_BinaryAnalysis_ModelChecker_ErrorTag_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/ModelChecker/Tag.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Types.h>
#include <Rose/BinaryAnalysis/SymbolicExpr.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

/** Tag that describes a model checker abnormal condition.
 *
 *  Although these tags are always treated as non-fatal errors within the model checker in that they halt any further
 *  exploration along that path, they have a property that indicates the severity level so they can be printed more reasonably
 *  by tools. */
class ErrorTag: public NameTag {
public:
    using Ptr = ErrorTagPtr;

protected:
    const std::string mesg_;                                       // error message
    SgAsmInstruction* const insn_;                                 // instruction where the error occurs (optional)
    Sawyer::Optional<uint64_t> concrete_;                          // optional concrete value
    const SymbolicExpr::Ptr symbolic_;                             // optional symbolic value
    const InstructionSemantics::BaseSemantics::SValuePtr svalue_;  // optional semantic value
    Sawyer::Message::Importance importance_ = Sawyer::Message::ERROR;

protected:
    ErrorTag() = delete;
    ErrorTag(size_t nodeStep, const std::string &name, const std::string &mesg, SgAsmInstruction*,
             const Sawyer::Optional<uint64_t>&);
    ErrorTag(size_t nodeStep, const std::string &name, const std::string &mesg, SgAsmInstruction*,
             const SymbolicExpr::Ptr&);
    ErrorTag(size_t nodeStep, const std::string &name, const std::string &mesg, SgAsmInstruction*,
             const InstructionSemantics::BaseSemantics::SValuePtr&);

public:
    ~ErrorTag();

    /** Allocating constructor.
     *
     *  Thread safety: This constructor is thread safe.
     *
     * @{ */
    static Ptr instance(size_t nodeStep, const std::string &name, const std::string &mesg, SgAsmInstruction*);
    static Ptr instance(size_t nodeStep, const std::string &name, const std::string &mesg, SgAsmInstruction*,
                        uint64_t);
    static Ptr instance(size_t nodeStep, const std::string &name, const std::string &mesg, SgAsmInstruction*,
                        const SymbolicExpr::Ptr&);
    static Ptr instance(size_t nodeStep, const std::string &name, const std::string &mesg, SgAsmInstruction*,
                        const InstructionSemantics::BaseSemantics::SValuePtr&);
    /** @} */

    /** Property: Error message.
     *
     *  Thread safety: This property access is thread safe. */
    std::string message() const;

    /** Property: Importance.
     *
     *  ErrorTags are always treated as errors within the model checker in that they always halt exploration along the
     *  path to which they're associated. However, this @p importance property can be used to convey additional information
     *  to the tool that ultimately processes these tags.
     *
     * @{ */
    Sawyer::Message::Importance importance() const;
    void importance(Sawyer::Message::Importance);
    /** @} */

public:
    void print(std::ostream &out, const std::string &prefix) const override;
    void toYaml(std::ostream &out, const std::string &prefix) const override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
