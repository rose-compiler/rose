#include <sstream>
#include "PrePostConditionAnalysis.h"

PrePostConditionAnalysis::PrePostConditionAnalysis(AstInterface& _fa)
    : fa(_fa) {}

const PrePostConditionAnalysis::PrePostConditions& PrePostConditionAnalysis::analyze(SgNode* input){
    AstInterface::AstNodeList params, children, vars, inits;
    std::string functionName;
    AstNodePtr functionBody, cond, truebody, falsebody;
    static PrePostConditionAnalysis::PrePostConditions collectedConds;

    DebugPrePostCondition([&input](){ return "Analyzing node in analyze(): " + AstInterface::AstToString(input);});

    // Compute function-level conditions
    if (AstInterface::IsFunctionDefinition(input, &functionName, &params, 0, &functionBody, 0, 0, true) && functionBody != 0) {
        //TODO: Add type-based preconditions for params
        analyze(functionBody.get_ptr(), collectedConds);
        stmtToConditionsMap[input] = collectedConds;
        collectedConds.clear();
    }else if (AstInterface::IsBlock(input, 0, &children)) {
        for (AstInterface::AstNodeList::const_iterator p = children.begin(); p != children.end(); ++p) {
            AstNodePtr current = *p;
            analyze(current.get_ptr(), collectedConds);
        };
    }
    return collectedConds;
}

void PrePostConditionAnalysis::analyze(SgNode* input, PrePostConditionAnalysis::PrePostConditions& collectedConds){
    AstInterface::AstNodeList params, children, vars, inits;
    std::string functionName;
    AstNodePtr functionBody, cond, truebody, falsebody;

    DebugPrePostCondition([&input](){ return "Analyzing node in overloaded analyze(): " + AstInterface::AstToString(input); });

    if (AstInterface::IsFunctionDefinition(input, &functionName, &params, 0, &functionBody, 0, 0, true) && functionBody != 0) {
        //TODO: Add type-based preconditions for params
        analyze(functionBody.get_ptr(), collectedConds);
        stmtToConditionsMap[input] = collectedConds;
        collectedConds.clear();
    }else if (AstInterface::IsBlock(input, 0, &children)) {
        for (AstInterface::AstNodeList::const_iterator p = children.begin(); p != children.end(); ++p) {
            AstNodePtr current = *p;
            analyze(current.get_ptr(), collectedConds);
        };
    } else if (fa.IsVariableDecl(input, &vars, &inits)) {
        processVariableDeclaration(vars, inits, collectedConds);
    } else if (AstInterface::IsIf(input, &cond, &truebody, &falsebody)) {
        auto conditions = processConditional(cond, truebody, falsebody, collectedConds);
        stmtToConditionsMap[cond.get_ptr()] = conditions.first;
        // stmtToConditionsMap[cond.get_ptr()] = conditions.second; //TODO: Save negated cond and falsebody conditions in map
    } else if (AstInterface::IsExpression(input,0,0)) {
        processExpression(input, collectedConds);
    } else {
        // TODO: Enumerate and process other types of nodes
    }
}

void PrePostConditionAnalysis::processVariableDeclaration(const AstInterface::AstNodeList& vars, const AstInterface::AstNodeList& inits, PrePostConditionAnalysis::PrePostConditions& collectedConds) {
    AstInterface::AstNodeList::const_iterator pv = vars.begin();
    AstInterface::AstNodeList::const_iterator pi = inits.begin();
    PrePostCondition cond;
    while (pv != vars.end()) {
        AstNodePtr lhs = *pv;
        AstNodePtr rhs = (pi != inits.end()) ? *pi : nullptr;

        cond.addPostcondition(ApplyBinOP(SYMOP_EQ, SymbolicValGenerator::GetSymbolicVal(fa, lhs), SymbolicValGenerator::GetSymbolicVal(fa, rhs)));

        pv++;
        if (pi != inits.end()) {
            pi++;
        };
    }
    collectedConds.push_back(cond);
}

void PrePostConditionAnalysis::processExpression(SgNode* input, PrePostConditionAnalysis::PrePostConditions& collectedConds) {
    AstNodePtr lhs, rhs;
    PrePostCondition cond;
    bool readlhs = false;
    if(fa.IsAssignment(input, &lhs, &rhs, &readlhs)){
        cond.addPostcondition(ApplyBinOP(SYMOP_EQ, SymbolicValGenerator::GetSymbolicVal(fa, lhs), SymbolicValGenerator::GetSymbolicVal(fa, rhs)));
    }
    collectedConds.push_back(cond);
}

std::pair<PrePostConditionAnalysis::PrePostConditions, PrePostConditionAnalysis::PrePostConditions> 
    PrePostConditionAnalysis::processConditional(const AstNodePtr& cond, const AstNodePtr& truebody, const AstNodePtr& falsebody, PrePostConditionAnalysis::PrePostConditions& collectedConds) {
    PrePostConditionAnalysis::PrePostConditions trueConds, falseConds;
    if(truebody != 0){
        DebugPrePostCondition([&truebody](){return "true: " + AstInterface::AstToString(truebody);});
        PrePostCondition t;
        t.addPrecondition(SymbolicValGenerator::GetSymbolicVal(fa, cond));
        trueConds.push_back(t);
        analyze(truebody.get_ptr(), trueConds);
        collectedConds.insert(collectedConds.end(), trueConds.begin(), trueConds.end());
    }
    if(falsebody != 0){
        DebugPrePostCondition([&falsebody](){return "false: " + AstInterface::AstToString(falsebody);});
        PrePostCondition f;
        //TODO: Need to negate cond -- add SYMOP_NOT operator
        f.addPrecondition(SymbolicValGenerator::GetSymbolicVal(fa, cond));
        falseConds.push_back(f);
        analyze(falsebody.get_ptr(), falseConds);
        collectedConds.insert(collectedConds.end(), falseConds.begin(), falseConds.end());
    }
    return std::make_pair(trueConds, falseConds);
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
