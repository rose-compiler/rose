#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Partitioner2/Utility.h>

#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/Diagnostics.h>
#include <rose_strtoull.h>

#include <SgAsmBlock.h>
#include <SgAsmFunction.h>
#include <SgAsmInstruction.h>

#include <Cxx_GrammarDowncast.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

Sawyer::Message::Facility mlog;

void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::Partitioner2");
        mlog.comment("partitioning insns to basic blocks and functions");
    }
}

bool
sortBasicBlocksByAddress(const BasicBlock::Ptr &a, const BasicBlock::Ptr &b) {
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    return a->address() < b->address();
}

bool
sortDataBlocks(const DataBlock::Ptr &a, const DataBlock::Ptr &b) {
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    if (a!=b) {
        if (a->address() != b->address())
            return a->address() < b->address();
        if (a->size() < b->size())
            return true;
    }
    return false;
}

bool
sortFunctionsByAddress(const Function::Ptr &a, const Function::Ptr &b) {
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    return a->address() < b->address();
}

bool
sortFunctionNodesByAddress(const SgAsmFunction *a, const SgAsmFunction *b) {
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    return a->get_entryVa() < b->get_entryVa();
}


// Sort by the successor expressions. If both expressions are concrete then disregard their widths and treat them as unsigned
// when comparing.
bool
sortByExpression(const BasicBlock::Successor &a, const BasicBlock::Successor &b) {
    auto aval = a.expr()->toUnsigned();
    auto bval = b.expr()->toUnsigned();
    if (aval && bval)
        return *aval < *bval;
    if (aval || bval)
        return aval ? true : false;                     // concrete values are less than abstract expressions
    return a.expr()->get_expression()->compareStructure(b.expr()->get_expression()) < 0;
}

bool
sortBlocksForAst(SgAsmBlock *a, SgAsmBlock *b) {
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    if (a->get_address() != b->get_address())
        return a->get_address() < b->get_address();
    if (a->get_statementList().size()>0 && b->get_statementList().size()>0) {
        // Sort so basic blocks come before data blocks when they start at the same address, regardless of size
        bool a_isBasicBlock = NULL!=isSgAsmInstruction(a->get_statementList()[0]);
        bool b_isBasicBlock = NULL!=isSgAsmInstruction(b->get_statementList()[0]);
        if (a_isBasicBlock != b_isBasicBlock)
            return a_isBasicBlock;
    }
    return false;
}

bool
sortInstructionsByAddress(SgAsmInstruction *a, SgAsmInstruction *b) {
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    return a->get_address() < b->get_address();
}

size_t
serialNumber() {
    static size_t nCalls = 0;
    return ++nCalls;
}

// class method
std::string
AddressIntervalParser::docString() {
    return Rose::CommandLine::IntervalParser<AddressInterval>::docString("address interval", "address");
}

AddressIntervalParser::Ptr
addressIntervalParser(AddressInterval &storage) {
    return AddressIntervalParser::instance(Sawyer::CommandLine::TypedSaver<AddressInterval>::instance(storage));
}

AddressIntervalParser::Ptr
addressIntervalParser(std::vector<AddressInterval> &storage) {
    return AddressIntervalParser::instance(Sawyer::CommandLine::TypedSaver<std::vector<AddressInterval> >::instance(storage));
}

AddressIntervalParser::Ptr
addressIntervalParser(AddressIntervalSet &storage) {
    return AddressIntervalParser::instance(Sawyer::CommandLine::TypedSaver<AddressIntervalSet>::instance(storage));
}

AddressIntervalParser::Ptr
addressIntervalParser() {
    return AddressIntervalParser::instance();
}

// class method
Sawyer::CommandLine::SwitchGroup
Trigger::switches(Settings &settings) {
    using namespace Sawyer::CommandLine;
    using namespace StringUtility;
    SwitchGroup switches;
    switches.insert(Switch("activate")
                    .argument("interval", addressIntervalParser(settings.when))
                    .doc("Restricts when this action should be triggered in terms of number of calls.  When an action's "
                         "other settings indicate that the action should be performed, the specified interval is consulted "
                         "to determine whether to actually invoke the action.  Whether the action is invoked or not, its "
                         "number-of-calls counter is incremented.  The default is to " +
                         std::string(settings.when.isEmpty() ? "never invoke the action." :
                                     settings.when.isSingleton() ? ("invoke the action only on call " +
                                                                    numberToString(settings.when.least()) + ".") :
                                     settings.when.least()==0 ?("invoke the action " + plural(settings.when.size(), "times") +
                                                                " beginning immediately.") :
                                     ("invoke the action on calls " + numberToString(settings.when.least()) + " through " +
                                      numberToString(settings.when.greatest()) + ", inclusive."))));
    return switches;
}

boost::logic::tribool
hasAnyCalleeReturn(const Partitioner::ConstPtr &partitioner, const ControlFlowGraph::ConstVertexIterator &caller) {
    ASSERT_not_null(partitioner);
    bool hasIndeterminateCallee = false;
    for (ControlFlowGraph::ConstEdgeIterator edge=caller->outEdges().begin(); edge != caller->outEdges().end(); ++edge) {
        if (edge->value().type() == E_FUNCTION_CALL) {
            bool mayReturn = false;
            if (!partitioner->basicBlockOptionalMayReturn(edge->target()).assignTo(mayReturn)) {
                hasIndeterminateCallee = true;
            } else if (mayReturn) {
                return true;
            }
        }
    }
    if (hasIndeterminateCallee)
        return boost::logic::indeterminate;
    return false;
}

bool
hasCallReturnEdges(const ControlFlowGraph::ConstVertexIterator &vertex) {
    for (const ControlFlowGraph::Edge &edge: vertex->outEdges()) {
        if (edge.value().type() == E_CALL_RETURN)
            return true;
    }
    return false;
}


// class method
std::string
Trigger::docString() {
    Settings settings;
    return Sawyer::CommandLine::Parser().with(switches(settings)).docForSwitches();
}

std::ostream&
operator<<(std::ostream &out, const AddressUser &x) {
    x.print(out);
    return out;
}

std::ostream&
operator<<(std::ostream &out, const AddressUsers &x) {
    x.print(out);
    return out;
}

std::ostream&
operator<<(std::ostream &out, const AddressUsageMap &x) {
    x.print(out);
    return out;
}


} // namespace
} // namespace
} // namespace

#endif
