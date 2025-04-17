#ifndef PREPOSTCONDITION_H
#define PREPOSTCONDITION_H

#include <string>
#include "SymbolicVal.h"

// Class representing preconditions and postconditions
class PrePostCondition {
    SymbolicVal precondition;
    SymbolicVal postcondition;
public:
    PrePostCondition(){}

    void addPrecondition(const SymbolicVal& cond);
    void addPostcondition(const SymbolicVal& cond);

    const SymbolicVal& getPrecondition() const { return precondition; }
    const SymbolicVal& getPostcondition() const { return postcondition; }

    std::string toString() const;
};

#endif // PREPOSTCONDITION_H
