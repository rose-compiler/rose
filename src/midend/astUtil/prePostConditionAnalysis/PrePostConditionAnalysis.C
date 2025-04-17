#include <sstream>
#include "PrePostConditionAnalysis.h"

PrePostConditionAnalysis::PrePostConditionAnalysis(AstInterface& _fa)
    : fa(_fa) {}

const PrePostConditionAnalysis::PrePostConditions& PrePostConditionAnalysis::analyze(SgNode* input){
    AstInterface::AstNodeList params, children, vars, inits;
    std::string functionName;
    AstNodePtr functionBody, cond, truebody, falsebody;

    DebugPrePostCondition([&input](){ return "Analyzing node " + AstInterface::AstToString(input); });

    // Compute function-level conditions
    if (AstInterface::IsFunctionDefinition(input, &functionName, &params, 0, &functionBody, 0, 0, true) && functionBody != 0) {
        //TODO: Add preconditions from the given parameter types
        analyze(functionBody.get_ptr());
    // Compute block-level conditions
    } else if (AstInterface::IsBlock(input, 0, &children)) {
        for (AstInterface::AstNodeList::const_iterator p = children.begin(); p != children.end(); ++p) {
            AstNodePtr current = *p;
            analyze(current.get_ptr());
        };
    } else if (fa.IsVariableDecl(input, &vars, &inits)) {
        processVariableDeclaration(vars, inits);
    } else if (AstInterface::IsIf(input, &cond, &truebody, &falsebody)) {
        processConditional(cond, truebody, falsebody);
    } else if (AstInterface::IsExpression(input,0,0)) {
        processExpression(input);
    } else {
        // TODO: Enumerate and process other types of nodes
    }
    if(!conditions.empty()){
        stmtToConditionsMap[input] = conditions;
    }
    return conditions;
}

void PrePostConditionAnalysis::processVariableDeclaration(const AstInterface::AstNodeList& vars, const AstInterface::AstNodeList& inits) {
    AstInterface::AstNodeList::const_iterator pv = vars.begin();
    AstInterface::AstNodeList::const_iterator pi = inits.begin();
    while (pv != vars.end()) {
        AstNodePtr lhs = *pv;
        AstNodePtr rhs = (pi != inits.end()) ? *pi : nullptr;

        // conditions.addPostcondition(ApplyBinOP(SYMOP_EQ, SymbolicValGenerator::GetSymbolicVal(fa, lhs), SymbolicValGenerator::GetSymbolicVal(fa, rhs)));

        pv++;
        if (pi != inits.end()) {
            pi++;
        };
    }
}

void PrePostConditionAnalysis::processExpression(SgNode* input) {
    AstNodePtr lhs, rhs;
    bool readlhs = false;
    if(fa.IsAssignment(input, &lhs, &rhs, &readlhs)){
        // conditions.addPostcondition(ApplyBinOP(SYMOP_EQ, SymbolicValGenerator::GetSymbolicVal(fa, lhs), SymbolicValGenerator::GetSymbolicVal(fa, rhs)));
    }
}

void PrePostConditionAnalysis::processConditional(const AstNodePtr& cond, const AstNodePtr& truebody, const AstNodePtr& falsebody) {
    if(truebody != 0){
        DebugPrePostCondition([&truebody](){return "true: " + AstInterface::AstToString(truebody);});
        analyze(truebody.get_ptr()); 
        //TODO: store pre, post conditions of the if and the else blocks separately
    }
    if(falsebody != 0){
        DebugPrePostCondition([&falsebody](){return "false: " + AstInterface::AstToString(falsebody);});
        analyze(falsebody.get_ptr());
    }
}

std::string PrePostConditionAnalysis::toString() const {  
    std::ostringstream oss;
    for (auto it = stmtToConditionsMap.cbegin(); it != stmtToConditionsMap.cend(); ) {  
        // oss << AstInterface::AstToString(it->first) << "\n" << it->second.toString();
        oss << AstInterface::AstToString(it->first) << "\n";
        for (const auto& element : it->second) {  
            oss << element.toString();  
        }
        if (++it != stmtToConditionsMap.cend()) {  
            oss << "\n";  
        }  
    } 
    return oss.str();  
} 
