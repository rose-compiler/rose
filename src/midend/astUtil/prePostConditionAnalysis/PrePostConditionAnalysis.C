#include <sstream>
#include "PrePostConditionAnalysis.h"

PrePostConditionAnalysis::PrePostConditionAnalysis(AstInterface& _fa)
    : fa(_fa) {}

const PrePostConditionAnalysis::PrePostConditions& PrePostConditionAnalysis::analyze(const AstNodePtr& input) {
    AstInterface::AstNodeList params, children, vars, inits;
    std::string functionName;
    AstNodePtr functionBody, cond, truebody, falsebody;
    static PrePostConditionAnalysis::PrePostConditions collectedConds;

    DebugPrePostCondition([&input](){ return "Analyzing node in analyze(): " + AstInterface::AstToString(input);});

    // Compute function-level conditions
    if (AstInterface::IsFunctionDefinition(input, &functionName, &params, 0, &functionBody, 0, 0, true) && functionBody != 0) {
        //TODO: Add type-based preconditions for params
        analyze(functionBody, collectedConds);
        stmtToConditionsMap[input.get_ptr()] = collectedConds;
        collectedConds.clear();
    }else if (AstInterface::IsBlock(input, 0, &children)) {
        for (AstInterface::AstNodeList::const_iterator p = children.begin(); p != children.end(); ++p) {
            AstNodePtr current = *p;
            analyze(current, collectedConds);
        };
    }
    return collectedConds;
}

void PrePostConditionAnalysis::analyze(const AstNodePtr& input, PrePostConditionAnalysis::PrePostConditions& collectedConds){
    AstInterface::AstNodeList params, children, vars, inits;
    std::string functionName;
    AstNodePtr functionBody, cond, truebody, falsebody;

    DebugPrePostCondition([&input](){ return "Analyzing node in overloaded analyze(): " + AstInterface::AstToString(input); });

    if (AstInterface::IsFunctionDefinition(input, &functionName, &params, 0, &functionBody, 0, 0, true) && functionBody != 0) {
        //TODO: Add type-based preconditions for params
        analyze(functionBody, collectedConds);
        stmtToConditionsMap[input.get_ptr()] = collectedConds;
        collectedConds.clear();
    }else if (AstInterface::IsBlock(input, 0, &children)) {
        for (AstInterface::AstNodeList::const_iterator p = children.begin(); p != children.end(); ++p) {
            AstNodePtr current = *p;
            analyze(current, collectedConds);
        };
    } else if (fa.IsVariableDecl(input, &vars, &inits)) {
        processVariableDeclaration(vars, inits, collectedConds);
    } else if (AstInterface::IsIf(input, &cond, &truebody, &falsebody)) {
        auto conditions = processConditional(cond, truebody, falsebody, collectedConds);
        stmtToConditionsMap[cond.get_ptr()] = conditions.first;
        //TODO: Save negated cond and collected conditions from falsebody in map
        // stmtToConditionsMap[cond.get_ptr()] = conditions.second; 
    } else if (AstInterface::IsExpression(input,0,0)) {
        processExpression(input, collectedConds);
    } else {
        DebugPrePostCondition([&input](){ return "Warning: Unhandled type of node"; });
    }
}

void PrePostConditionAnalysis::processVariableDeclaration(const AstInterface::AstNodeList& vars, const AstInterface::AstNodeList& inits, PrePostConditionAnalysis::PrePostConditions& collectedConds) {
    AstInterface::AstNodeList::const_iterator pv = vars.begin();
    AstInterface::AstNodeList::const_iterator pi = inits.begin();
    PrePostCondition cond;

    while (pv != vars.end()) {
        AstNodePtr lhs = *pv;
        AstNodePtr rhs = *pi;
        if(rhs != AstInterface::AST_NULL){
            cond.addPostcondition(ApplyBinOP(SYMOP_EQ, SymbolicValGenerator::GetSymbolicVal(fa, lhs), SymbolicValGenerator::GetSymbolicVal(fa, rhs)));
        }
        ++pv;
        ++pi;
    }
    if(cond.hasConditions()){
        collectedConds.push_back(cond);
    }
}

void PrePostConditionAnalysis::processExpression(const AstNodePtr& input, PrePostConditionAnalysis::PrePostConditions& collectedConds) {
    AstNodePtr lhs, rhs, strippedExp;
    PrePostCondition cond;
    bool readlhs = false;

    if(!AstInterface::IsExpression(input,0,&strippedExp)){
        ROSE_ABORT();
    }
    
    if(fa.IsAssignment(input, &lhs, &rhs, &readlhs)){
        DebugPrePostCondition([&lhs](){return "Processing assignment -- lhs: " + AstInterface::AstToString(lhs);});
        DebugPrePostCondition([&rhs](){return "rhs: " + AstInterface::AstToString(rhs);});
        cond.addPostcondition(ApplyBinOP(SYMOP_EQ, SymbolicValGenerator::GetSymbolicVal(fa, lhs), SymbolicValGenerator::GetSymbolicVal(fa, rhs)));
    }
    
    if(cond.hasConditions()){
        collectedConds.push_back(cond);
    }
}

std::pair<PrePostConditionAnalysis::PrePostConditions, PrePostConditionAnalysis::PrePostConditions> 
    PrePostConditionAnalysis::processConditional(const AstNodePtr& cond, const AstNodePtr& truebody, const AstNodePtr& falsebody, PrePostConditionAnalysis::PrePostConditions& collectedConds) {
    // Store conditions for true and false branches
    PrePostConditionAnalysis::PrePostConditions trueBodyConds, falseBodyConds; 
    PrePostCondition trueCond, falseCond; // Test condition for the true and false branches

    if(cond != 0){
        AstNodePtr strippedCond;
        if(AstInterface::IsExpression(cond, 0, &strippedCond)){
            DebugPrePostCondition([&strippedCond](){return "Condition: " + AstInterface::AstToString(strippedCond);});
            trueCond.addPrecondition(SymbolicValGenerator::GetSymbolicVal(fa, strippedCond));
            
            //Negate condition and accumulate precondition
            falseCond.addPrecondition(ApplyUnaryOP(SYMOP_NOT, SymbolicValGenerator::GetSymbolicVal(fa, strippedCond)));
        }
    }
    if(truebody != 0){
        DebugPrePostCondition([&truebody](){return "true: " + AstInterface::AstToString(truebody);});
        trueBodyConds.push_back(trueCond);
        analyze(truebody, trueBodyConds);
        collectedConds.insert(collectedConds.end(), trueBodyConds.begin(), trueBodyConds.end());
    }
    if(falsebody != 0){
        DebugPrePostCondition([&falsebody](){return "false: " + AstInterface::AstToString(falsebody);});
        falseBodyConds.push_back(falseCond);
        analyze(falsebody, falseBodyConds);
        collectedConds.insert(collectedConds.end(), falseBodyConds.begin(), falseBodyConds.end());
    }
    return std::make_pair(trueBodyConds, falseBodyConds);
}

std::string PrePostConditionAnalysis::toString() const {  
    std::ostringstream oss;
    for (auto it = stmtToConditionsMap.cbegin(); it != stmtToConditionsMap.cend(); ) {
        oss << "\n=======================================\n" 
        << AstInterface::AstToString(it->first) 
        << "\n=======================================";
        for (const auto& element : it->second) {  
            oss << element.toString();  
        }
        if (++it != stmtToConditionsMap.cend()) {  
            oss << "\n";  
        }  
    }
    oss << "\n";
    return oss.str();  
}
