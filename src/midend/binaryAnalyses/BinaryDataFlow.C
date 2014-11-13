#include "sage3basic.h"
#include "BinaryDataFlow.h"
#include "Diagnostics.h"

namespace rose {
namespace BinaryAnalysis {

using namespace Diagnostics;
using namespace InstructionSemantics2;

Sawyer::Message::Facility DataFlow::mlog;

void
DataFlow::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        mlog = Sawyer::Message::Facility("rose::BinaryAnalysis::DataFlow", Diagnostics::destination);
        rose::Diagnostics::mfacilities.insertAndAdjust(mlog);
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

void
DataFlow::buildGraphProcessInstruction(SgAsmInstruction *insn)
{
    mlog[DEBUG] <<"  processing " <<unparseInstructionWithAddress(insn) <<"\n";
    dispatcher_->processInstruction(insn);
}

DataFlow::Graph
DataFlow::buildGraph(SgAsmInstruction *insn) {
    ASSERT_this();
    ASSERT_not_null(insn);
    ASSERT_not_null(dispatcher_);
    dfOps_->clearGraph();

    if (mlog[DEBUG]) {
        mlog[DEBUG] <<"buildGraph: incoming state at " <<StringUtility::addrToString(insn->get_address()) <<":\n";
        userOps_->print(mlog[DEBUG], "  |");
    }

    buildGraphProcessInstruction(insn);

    if (mlog[DEBUG]) {
        mlog[DEBUG] <<"  outgoing state:\n";
        userOps_->print(mlog[DEBUG], "  |");
    }

    return dfOps_->getGraph();
}

DataFlow::Graph
DataFlow::buildGraph(SgAsmBlock *bb)
{
    ASSERT_this();
    ASSERT_not_null(bb);
    ASSERT_not_null(dispatcher_);
    dfOps_->clearGraph();

    if (mlog[DEBUG]) {
        mlog[DEBUG] <<"buildGraph: incoming state at " <<StringUtility::addrToString(bb->get_address()) <<":\n";
        userOps_->print(mlog[DEBUG], "  |");
    }

    BOOST_FOREACH (SgAsmInstruction *insn, SageInterface::querySubTree<SgAsmInstruction>(bb))
        buildGraphProcessInstruction(insn);

    if (mlog[DEBUG]) {
        mlog[DEBUG] <<"  outgoing state:\n";
        userOps_->print(mlog[DEBUG], "  |");
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
