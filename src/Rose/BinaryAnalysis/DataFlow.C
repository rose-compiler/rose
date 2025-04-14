#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/DataFlow.h>

#include <Rose/AST/Traversal.h>
#include <Rose/Diagnostics.h>

#include <SgAsmBlock.h>
#include <SgAsmInstruction.h>

namespace Rose {
namespace BinaryAnalysis {

using namespace Diagnostics;
using namespace InstructionSemantics;

Sawyer::Message::Facility DataFlow::mlog;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DataFlow::Exception
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DataFlow::Exception::Exception(const std::string &s)
    : Rose::Exception(s) {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DataFlow::NotConverging
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DataFlow::NotConverging::NotConverging(const std::string &s)
    : Exception(s) {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DataFlow::DefaultVertexUnpacker
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DataFlow::DefaultVertexUnpacker::Instructions
DataFlow::DefaultVertexUnpacker::operator()(SgAsmInstruction *insn) {
    return Instructions(1, insn);
}

std::vector<SgAsmInstruction*>
DataFlow::DefaultVertexUnpacker::operator()(SgAsmBlock *blk) {
    return AST::Traversal::findDescendantsTyped<SgAsmInstruction>(blk);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DataFlow::SemanticsMerge
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DataFlow::SemanticsMerge::~SemanticsMerge() {}

DataFlow::SemanticsMerge::SemanticsMerge(const BaseSemantics::RiscOperators::Ptr &ops)
    : ops_(ops) {}

DataFlow::SemanticsMerge::SemanticsMerge(const BaseSemantics::DispatcherPtr &cpu)
    : ops_(cpu->operators()) {}

bool
DataFlow::SemanticsMerge::operator()(const size_t /*dstId*/, BaseSemantics::StatePtr &dst /*in,out*/,
                                     const size_t /*srcId*/, const BaseSemantics::StatePtr &src) const {
    struct PreserveCurrentState {
        BaseSemantics::RiscOperatorsPtr ops;
        BaseSemantics::StatePtr state;
        PreserveCurrentState(const BaseSemantics::RiscOperatorsPtr &ops)
            : ops(ops), state(ops->currentState()) {}
        ~PreserveCurrentState() { ops->currentState(state); }
    } t(ops_);

    if (!dst) {
        dst = src->clone();
        return true;
    } else {
        ops_->currentState(src);
        return dst->merge(src, ops_.get(), ops_.get());
    }
}

BaseSemantics::RiscOperators::Ptr
DataFlow::SemanticsMerge::operators() const {
    return ops_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DataFlow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DataFlow::~DataFlow() {}

DataFlow::DataFlow(const BaseSemantics::Dispatcher::Ptr &userDispatcher) {
    init(userDispatcher);
}

void
DataFlow::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Rose::Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::DataFlow");
        mlog.comment("solving data-flow analysis problems");
    }
}

void
DataFlow::init(const BaseSemantics::Dispatcher::Ptr &userDispatcher) {
    ASSERT_not_null(userDispatcher);
    userOps_ = userDispatcher->operators();
    ASSERT_not_null(userOps_);
    dfOps_ = DataFlowSemantics::RiscOperators::instance(userOps_);
    ASSERT_not_null(dfOps_);
    dispatcher_ = userDispatcher->create(dfOps_);       // a new dispatcher but with our operators
    ASSERT_not_null(dispatcher_);
}

DataFlow::Graph
DataFlow::buildGraph(const std::vector<SgAsmInstruction*> &insns)
{
    ASSERT_this();
    ASSERT_not_null(dispatcher_);
    dfOps_->clearGraph();

    if (insns.empty()) {
        mlog[DEBUG] <<"buildGraph: empty graph; returning empty data flows\n";
    } else {
        if (mlog[DEBUG]) {
            mlog[DEBUG] <<"buildGraph: incoming state at " <<StringUtility::addrToString(insns.front()->get_address()) <<":\n";
            userOps_->print(mlog[DEBUG], "  |");
        }

        for (SgAsmInstruction *insn: insns) {
            mlog[DEBUG] <<"  processing " <<insn->toString() <<"\n";
            dispatcher_->processInstruction(insn);
        }

        if (mlog[DEBUG]) {
            mlog[DEBUG] <<"  outgoing state:\n";
            userOps_->print(mlog[DEBUG], "  |");
        }
    }
    return dfOps_->getGraph();
}

DataFlow::VariableList
DataFlow::getUniqueVariables(const VertexFlowGraphs &graphs) {
    VariableList variables;
    for (const DataFlow::Graph &dfg: graphs.values()) {
        for (const Variable &candidateVariable: dfg.vertexValues()) {
            bool alreadyHaveIt = false;
            for (const Variable &existingVariable: variables) {
                if (candidateVariable.mustAlias(existingVariable)) {
                    alreadyHaveIt = true;
                    break;
                }
            }
            if (!alreadyHaveIt)
                variables.push_back(candidateVariable);
        }
    }
    return variables;
}

} // namespace
} // namespace

#endif
