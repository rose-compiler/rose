#ifndef FINITEDIFFERENCING_H
#define FINITEDIFFERENCING_H

// #include "config.h"


#include "inlinerSupport.h"
#include "replaceExpressionWithStatement.h"
#include "expressionTreeEqual.h"
#include "patternRewrite.h"

//! Do finite differencing on one expression within one context.  The expression
//! must be defined and valid within the entire body of root.  The rewrite rules
//! are used to simplify expressions.  When a variable var is updated from
//! old_value to new_value, an expression of the form (var, (old_value,
//! new_value)) is created and rewritten.  The rewrite rules may either produce
//! an arbitrary expression (which will be used as-is) or one of the form (var,
//! (something, value)) (which will be changed to (var = value)).
void doFiniteDifferencingOne(SgExpression* e, 
                             SgBasicBlock* root,
			     RewriteRule* rules);

//! Propagate definitions of a variable to its uses.
//! Assumptions: var is only assigned at the top level of body
//!              nothing var depends on is assigned within body
//! Very simple algorithm designed to only handle simplest cases
void simpleUndoFiniteDifferencingOne(SgBasicBlock* body,
                                     SgExpression* var);

//! Move variables declared in a for statement to just outside that statement.
void moveForDeclaredVariables(SgNode* root);

//! Do a simple form of finite differencing on all functions contained within
//! root (which should be a project, file, or function definition).
void simpleIndexFiniteDifferencing(SgNode* root);

#endif // FINITEDIFFERENCING_H
