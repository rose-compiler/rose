#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/ReadWriteSets.h>

#include <Rose/Affirm.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>

namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace Rose {
namespace BinaryAnalysis {

ReadWriteSets::~ReadWriteSets() {}

ReadWriteSets::ReadWriteSets(const P2::Partitioner::ConstPtr &partitioner, const Settings &settings)
    : settings_(settings), partitioner_(notnull(partitioner)) {}

ReadWriteSets::Ptr
ReadWriteSets::instance(const P2::Partitioner::ConstPtr &partitioner, const Settings &settings) {
    return Ptr(new ReadWriteSets(partitioner, settings));
}

const ReadWriteSets::Settings&
ReadWriteSets::settings() const {
    return settings_;
}

ReadWriteSets::Settings&
ReadWriteSets::settings() {
    return settings_;
}

void
ReadWriteSets::analyze(const P2::Function::Ptr &function) {
    ASSERT_not_null(function);
    auto varFinder = Variables::VariableFinder::instance(settings_.variableFinder);
    auto stackVars = varFinder->findStackVariables(partitioner_, function);
    for (const Variables::StackVariable &var: stackVars.values()) {
        std::cerr <<"ROBB: var = " <<var <<" in " <<function->printableName() <<"\n";
        for (const Variables::InstructionAccess &ia: var.instructionsAccessing())
            std::cerr <<"        insn " <<StringUtility::addrToString(ia.address())
                      <<" " <<ia.accessString() <<"\n";
    }
}

} // namespace
} // namespace

#endif
