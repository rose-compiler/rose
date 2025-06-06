#ifndef PREPOSTCONDITIONANALYSIS_H
#define PREPOSTCONDITIONANALYSIS_H

#include <string>
#include <vector>
#include <utility>

#include "CommandOptions.h"
#include "sage3.h"
#include "AstInterface_ROSE.h"

#include "PrePostCondition.h"

class PrePostConditionAnalysis {
public:
    typedef std::vector<PrePostCondition> PrePostConditions;
    
    // Debugging flag for pre, postcondition analysis
    DebugLog DebugPrePostCondition = DebugLog("-debugprepost");

    // Constructor
    PrePostConditionAnalysis(AstInterface& _fa);

    const PrePostConditions& analyze(SgNode* input);
    void analyze(SgNode* input, PrePostConditions& collectedConds);

    //Printable output of collected conditions
    std::string toString() const;
protected:
    // Helpers to process specific types of expressions--updates collectedConds
    void processVariableDeclaration(const AstInterface::AstNodeList& vars, const AstInterface::AstNodeList& inits, 
                                    PrePostConditions& collectedConds);
    void processExpression(SgNode* input, PrePostConditions& collectedConds);
    std::pair<PrePostConditions, PrePostConditions> processConditional(const AstNodePtr& cond, 
                                                    const AstNodePtr& truebody, const AstNodePtr& falsebody, PrePostConditions& collectedConds);

private:
    AstInterface fa;
    std::map<SgNode*, PrePostConditions> stmtToConditionsMap;
};

#endif // PREPOSTCONDITIONANALYSIS_H
