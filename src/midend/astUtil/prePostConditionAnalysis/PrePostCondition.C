#include "PrePostCondition.h"

void PrePostCondition::addPrecondition(const SymbolicVal& cond){
    if(precondition.IsNIL()){
        precondition = cond;
    }else{
        precondition = ApplyBinOP(SYMOP_AND, precondition, cond);
    }
}

void PrePostCondition::addPostcondition(const SymbolicVal& cond){
    if(postcondition.IsNIL()){
        postcondition = cond;
    }else{
        postcondition = ApplyBinOP(SYMOP_AND, postcondition, cond);
    }
}

bool PrePostCondition::hasConditions() {
    return !precondition.IsNIL() || !postcondition.IsNIL();
}

std::string PrePostCondition::toString() const{
    std::string constraints;
    constraints.append("\n=== Conditions ===");
    constraints.append("\nPrecondition: " + precondition.toString());
    constraints.append("\nPostcondition: " + postcondition.toString());
    return constraints;
}
