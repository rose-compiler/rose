#ifndef REVERSE_COMPUTATION_STORAGE_H
#define REVERSE_COMPUTATION_STORAGE_H

#include <rose.h>


//******************************************************************************
// Utility functions, which let users use those functions directly.
//******************************************************************************


SgExpression* getStackVar(SgNode* node);

std::vector<SgVariableDeclaration*> getAllStackDeclarations();

// Currently, when calling the following function, make sure the expression passed in
// has a function declaration as parent.
SgExpression* pushVal(SgExpression* exp);

// FIXME  This function should be versioned to deal with int and float values.
// Note that currently, we require that an expression which is assigned by the popped value
// should be passed to this function, then we can do type check and stack name generation.
SgExpression* popVal(SgExpression* exp);


#endif	
