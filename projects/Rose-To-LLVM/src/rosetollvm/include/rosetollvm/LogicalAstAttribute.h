#ifndef LOGICAL_AND_AST_ATTRIBUTE
#define LOGICAL_AND_AST_ATTRIBUTE

#include <rose.h>
#include <rosetollvm/RootAstAttribute.h>

class LogicalAstAttribute : public RootAstAttribute { 
public: 
    LogicalAstAttribute(std::string r, std::string e) : rhsLabel(r),
                                                        endLabel(e)
    {} 

    LogicalAstAttribute(LogicalAstAttribute *a) : rhsLabel(a -> rhsLabel),
                                                  endLabel(a -> endLabel)
    {} 

    void setLastLhsLabel(const std::string last_lhs_label_) { lastLhsLabel = last_lhs_label_; }
    const std::string getLastLhsLabel() { return lastLhsLabel; }
    const std::string getRhsLabel() { return rhsLabel; }
    void setLastRhsLabel(const std::string last_rhs_label_) { lastRhsLabel = last_rhs_label_; }
    const std::string getLastRhsLabel() { return lastRhsLabel; }
    const std::string getEndLabel() { return endLabel; }

protected:

    std::string lastLhsLabel,
                rhsLabel, 
                lastRhsLabel,
                endLabel;
};

#endif
