#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

#include "AsmUnparser_compat.h"
#include "BinaryDataFlow.h"
#include "Diagnostics.h"

namespace Rose {
namespace BinaryAnalysis {

using namespace Diagnostics;
using namespace InstructionSemantics2;

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
DataFlow::init(const BaseSemantics::DispatcherPtr &userDispatcher) {
    ASSERT_not_null(userDispatcher);
    userOps_ = userDispatcher->get_operators();
    ASSERT_not_null(userOps_);
    dfOps_ = InstructionSemantics2::DataFlowSemantics::RiscOperators::instance(userOps_);
    ASSERT_not_null(dfOps_);
    dispatcher_ = userDispatcher->create(dfOps_);   // a new dispatcher but with our operators
    ASSERT_not_null(dispatcher_);
}

std::vector<SgAsmInstruction*>
DataFlow::DefaultVertexUnpacker::operator()(SgAsmBlock *blk) {
    return SageInterface::querySubTree<SgAsmInstruction>(blk);
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

        BOOST_FOREACH (SgAsmInstruction *insn, insns) {
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
    BOOST_FOREACH (const DataFlow::Graph &dfg, graphs.values()) {
        BOOST_FOREACH (const Variable &candidateVariable, dfg.vertexValues()) {
            bool alreadyHaveIt = false;
            BOOST_FOREACH (const Variable &existingVariable, variables) {
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
