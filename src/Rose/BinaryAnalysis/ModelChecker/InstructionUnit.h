#ifndef ROSE_BinaryAnalysis_ModelChecker_InstructionUnit_H
#define ROSE_BinaryAnalysis_ModelChecker_InstructionUnit_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER

#include <Rose/BinaryAnalysis/Address.h>
#include <Rose/BinaryAnalysis/ModelChecker/ExecutionUnit.h>

class SgAsmInstruction;

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

/** An execution unit for a single instruction.
 *
 *  This is a model checker execution unit representing a single instruction. */
class InstructionUnit: public ExecutionUnit {
public:
    using Ptr = InstructionUnitPtr;

private:
    SgAsmInstruction *insn_;

protected:
    InstructionUnit() = delete;
    InstructionUnit(SgAsmInstruction*, const SourceLocation&);
public:
    ~InstructionUnit();

public:
    /** Allocating constructor.
     *
     *  Constructs a new execution unit that holds a single instruction which must not be null.
     *
     *  Thread safety: This constructor is thread safe. */
    static Ptr instance(SgAsmInstruction*, const SourceLocation&);

public:
    /** Property: Instruction.
     *
     *  This is the instruction associated with this execution unit. The property is read-only and initialized when this
     *  object is created.
     *
     *  Thread safety: This method is thread safe, although the object it returns does not have a thread-safe API. */
    SgAsmInstruction* instruction() const;

public:
    virtual std::string printableName() const override;
    virtual void printSteps(const SettingsPtr&, std::ostream&, const std::string &prefix,
                            size_t stepOrigin, size_t maxSteps) const override;
    virtual void toYamlHeader(const SettingsPtr&, std::ostream&, const std::string &prefix) const override;
    virtual void toYamlSteps(const SettingsPtr&, std::ostream&, const std::string &prefix,
                             size_t stepOrigin, size_t maxSteps) const override;
    virtual std::vector<Sarif::LocationPtr> toSarif(size_t maxSteps) const override;
    virtual size_t nSteps() const override;
    virtual Sawyer::Optional<rose_addr_t> address() const override;

    virtual std::vector<TagPtr>
    execute(const SettingsPtr&, const SemanticCallbacksPtr&,
            const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
