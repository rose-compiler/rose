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
    dfOps_ = InstructionSemantics::DataFlowSemantics::RiscOperators::instance(userOps_);
    ASSERT_not_null(dfOps_);
    dispatcher_ = userDispatcher->create(dfOps_);       // a new dispatcher but with our operators
    ASSERT_not_null(dispatcher_);
}

std::vector<SgAsmInstruction*>
DataFlow::DefaultVertexUnpacker::operator()(SgAsmBlock *blk) {
    return AST::Traversal::findDescendantsTyped<SgAsmInstruction>(blk);
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
