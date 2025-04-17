#ifndef PREPOSTCONDITIONANALYSIS_H
#define PREPOSTCONDITIONANALYSIS_H

#include <string>
#include <unordered_set>

#include "CommandOptions.h"
#include "sage3.h"
#include "AstInterface_ROSE.h"

#include "PrePostCondition.h"

class PrePostConditionAnalysis {
public:
    typedef std::set<PrePostCondition> PrePostConditions;
    
    // Debugging flag for pre, postcondition analysis
    DebugLog DebugPrePostCondition = DebugLog("-debugprepost");

    // Constructor
    PrePostConditionAnalysis(AstInterface& _fa);

    const PrePostConditions& analyze(SgNode* input);

    //Printable output of collected conditions
    std::string toString() const;
protected:
    // Helpers to process specific types of expressions
    void processVariableDeclaration(const AstInterface::AstNodeList& vars, const AstInterface::AstNodeList& inits);
    void processExpression(SgNode* input);
    void processConditional(const AstNodePtr& cond, const AstNodePtr& truebody, const AstNodePtr& falsebody);

private:
    AstInterface fa;
    PrePostConditions conditions;
    std::map<SgNode*, PrePostConditions> stmtToConditionsMap;
};

#endif // PREPOSTCONDITIONANALYSIS_H
