#ifndef ROSE_BinaryAnalysis_ModelChecker_BasicBlockUnit_H
#define ROSE_BinaryAnalysis_ModelChecker_BasicBlockUnit_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER

#include <Rose/BinaryAnalysis/ModelChecker/ExecutionUnit.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

/** An execution unit for a basic block.
 *
 *  This is a model checker execution unit representing a complete basic block. */
class BasicBlockUnit: public ExecutionUnit {
public:
    using Ptr = BasicBlockUnitPtr;

private:
    Partitioner2::PartitionerConstPtr partitioner_;     // not null
    Partitioner2::BasicBlockPtr bblock_;

protected:
    BasicBlockUnit() = delete;
    BasicBlockUnit(const Partitioner2::PartitionerConstPtr &partitioner, const Partitioner2::BasicBlockPtr&);
public:
    ~BasicBlockUnit();

public:
    /** Allocating constructor.
     *
     *  Constructs a new execution unit that holds the specified basic block, which must not be null.
     *
     *  Thread safety: This constructor is thread safe. */
    static Ptr instance(const Partitioner2::PartitionerConstPtr&, const Partitioner2::BasicBlockPtr&);

public:
    /** Property: Basic block.
     *
     *  This is the basic block associated with this execution unit. This property is read-only and initialized when this
     *  execution unit is created.
     *
     *  Thread safety: This property accessor is thread safe. */
    Partitioner2::BasicBlockPtr basicBlock() const;

public:
    virtual std::string printableName() const override;
    virtual void printSteps(const SettingsPtr&, std::ostream&, const std::string &prefix,
                            size_t stepOrigin, size_t maxSteps) const override;
    virtual void toYamlHeader(const SettingsPtr&, std::ostream&, const std::string &prefix) const override;
    virtual void toYamlSteps(const SettingsPtr&, std::ostream&, const std::string &prefix,
                             size_t stepOrigin, size_t maxSteps) const override;
    virtual std::vector<Sarif::LocationPtr> toSarif(size_t maxSteps) const override;
    virtual size_t nSteps() const override;
    virtual Sawyer::Optional<Address> address() const override;
    virtual bool containsUnknownInsn() const override;

    virtual std::vector<TagPtr>
    execute(const SettingsPtr&, const SemanticCallbacksPtr&,
            const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) override;
};


} // namespace
} // namespace
} // namespace

#endif
#endif
