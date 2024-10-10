#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/ReadWriteSets.h>

#include <Rose/Affirm.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/CommandLine.h>

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

Sawyer::CommandLine::SwitchGroup
ReadWriteSets::commandLineSwitches(Settings &settings) {
    Sawyer::CommandLine::SwitchGroup switches("Read-write set switches");
    switches.name("rwsets");
    switches.doc("These switches control the read/write set analysis, that finds variables and determines whether they're "
                 "read or written by a particular function and which instructions do the reading and/or writing.");

    CommandLine::insertBooleanSwitch(switches, "find-global-variables", settings.findingGlobalVariables,
                                     "Whether to search for global variables. Searching for global variables is generally an "
                                     "expensive whole-program analysis.");

    CommandLine::insertBooleanSwitch(switches, "find-local-variables", settings.findingLocalVariables,
                                     "Whether to search for local variables.");

    return switches;
}


void
ReadWriteSets::analyze(const P2::Function::Ptr &function) {
    ASSERT_not_null(function);
    auto varFinder = Variables::VariableFinder::instance(settings_.variableFinder);

    if (settings_.findingLocalVariables) {
        stackVariables_ = varFinder->findStackVariables(partitioner_, function);
    } else {
        stackVariables_.clear();
    }

    if (settings_.findingGlobalVariables) {
        globalVariables_ = varFinder->findGlobalVariables(partitioner_);
    } else {
        globalVariables_.clear();
    }
}

void
ReadWriteSets::print(std::ostream &out, const std::string &prefix) const {
    for (const Variables::StackVariable &var: stackVariables_.values()) {
        Variables::AccessFlags access;
        for (const Variables::InstructionAccess &ia: var.instructionsAccessing())
            access.set(ia.access());
        out <<var <<" " <<Variables::InstructionAccess::accessString(access) <<"\n";
    }

    for (const Variables::GlobalVariable &var: globalVariables_.values())
        out <<var <<"\n";
}

std::ostream&
operator<<(std::ostream &out, const ReadWriteSets &rwSets) {
    rwSets.print(out);
    return out;
}

} // namespace
} // namespace

#endif
