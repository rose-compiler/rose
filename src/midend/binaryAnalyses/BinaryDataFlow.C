#include "BinaryDataFlow.h"

namespace BinaryAnalysis {

using namespace BinaryAnalysis::InstructionSemantics2;

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

DataFlow::Graph
DataFlow::buildGraph(SgAsmInstruction *insn) {
    ASSERT_this();
    ASSERT_not_null(insn);
    ASSERT_not_null(dispatcher_);
    
    dfOps_->clearGraph();
    dispatcher_->processInstruction(insn);
    return dfOps_->getGraph();
}

DataFlow::Graph
DataFlow::buildGraph(SgAsmBlock *bb)
{
    ASSERT_this();
    ASSERT_not_null(bb);
    ASSERT_not_null(dispatcher_);
    
    dfOps_->clearGraph();
    BOOST_FOREACH (SgAsmInstruction *insn, SageInterface::querySubTree<SgAsmInstruction>(bb))
        dispatcher_->processInstruction(insn);
    return dfOps_->getGraph();
}

DataFlow::VariableList
DataFlow::getUniqueVariables(const VertexFlowIndex &index) {
    VariableList variables;
    BOOST_FOREACH (const DataFlow::Graph &dfg, index.values()) {
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
