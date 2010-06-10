
#include "rose.h"
#include "fada.h"

#ifndef __ROSE_TO_FADA_AST_
#define __ROSE_TO_FADA_AST_

namespace RoseToFada {

fada::Statement * parseSgFunctionDeclarationToFadaAST(SgFunctionDeclaration * n);
void parseSgStatementToFadaAST(SgStatement * stmt, fada::Statement * parent);

fada::Control *  parseSgForStatementToFadaControl(SgForStatement * n);
fada::Assignment * parseSgExpressionToFadaAssignment(SgExpression * exp);
fada::Expression * parseSgExpressionToFadaExpression(SgExpression * n);

std::string getSgForStatementIterator(SgForStatement * for_stmt);
fada::Expression * getSgForStatementInitExp(SgForStatement * for_stmt);
fada::Expression * getSgForStatementLimitExp(SgForStatement * for_stmt);
bool checkSgForStatementIsNormalized(SgForStatement * for_stmt);

} 

#endif /* __ROSE_TO_FADA_AST_ */
