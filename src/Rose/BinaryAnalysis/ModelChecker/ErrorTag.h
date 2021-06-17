#ifndef ROSE_BinaryAnalysis_ModelChecker_ErrorTag_H
#define ROSE_BinaryAnalysis_ModelChecker_ErrorTag_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/ModelChecker/Tag.h>

#include <Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/Types.h>
#include <Rose/BinaryAnalysis/SymbolicExpr.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

/** Tag that describes a model checker error. */
class ErrorTag: public NameTag {
public:
    using Ptr = ErrorTagPtr;

protected:
    const std::string mesg_;                                       // error message
    SgAsmInstruction* const insn_;                                 // instruction where the error occurs (optional)
    Sawyer::Optional<uint64_t> concrete_;                          // optional concrete value
    const SymbolicExpr::Ptr symbolic_;                             // optional symbolic value
    const InstructionSemantics2::BaseSemantics::SValuePtr svalue_; // optional semantic value

protected:
    ErrorTag() = delete;
    ErrorTag(size_t nodeStep, const std::string &name, const std::string &mesg, SgAsmInstruction*,
             const Sawyer::Optional<uint64_t>&);
    ErrorTag(size_t nodeStep, const std::string &name, const std::string &mesg, SgAsmInstruction*,
             const SymbolicExpr::Ptr&);
    ErrorTag(size_t nodeStep, const std::string &name, const std::string &mesg, SgAsmInstruction*,
             const InstructionSemantics2::BaseSemantics::SValuePtr&);

public:
    ~ErrorTag();

    /** Allocating constructor.
     *
     *  Thread safety: This constructor is thread safe.
     *
     * @{ */
    static Ptr instance(size_t nodeStep, const std::string &name, const std::string &mesg, SgAsmInstruction*,
                        uint64_t);
    static Ptr instance(size_t nodeStep, const std::string &name, const std::string &mesg, SgAsmInstruction*,
                        const SymbolicExpr::Ptr&);
    static Ptr instance(size_t nodeStep, const std::string &name, const std::string &mesg, SgAsmInstruction*,
                        const InstructionSemantics2::BaseSemantics::SValuePtr&);
    /** @} */

    /** Property: Error message.
     *
     *  Thread safety: This property access is thread safe. */
    std::string message() const;

public:
    void print(std::ostream &out, const std::string &prefix) const override;
    void toYaml(std::ostream &out, const std::string &prefix) const override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
