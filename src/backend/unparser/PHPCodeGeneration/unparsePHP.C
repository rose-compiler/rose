#include "sage3basic.h"
#include "unparser.h"

#include <iostream>

Unparse_PHP::Unparse_PHP(Unparser* unp, std::string fname) :
    UnparseLanguageIndependentConstructs(unp, fname)
{
}

Unparse_PHP::~Unparse_PHP()
{
}

void
Unparse_PHP::unparseStatement(SgStatement* stmt, SgUnparse_Info& info)
{
    Sg_File_Info *finfo = stmt->get_file_info();
    std::cout << finfo->get_line() << ":" << finfo->get_col() << std::endl;
}

void
Unparse_PHP::unparseLanguageSpecificStatement(SgStatement*, SgUnparse_Info&)
{
}

void
Unparse_PHP::unparseLanguageSpecificExpression(SgExpression*, SgUnparse_Info&)
{
}

void
Unparse_PHP::unparseStringVal(SgExpression*, SgUnparse_Info&)
{
}

bool
Unparse_PHP::requiresParentheses(SgExpression* expr, SgUnparse_Info& info) {
    ROSE_ASSERT(!"UnparsePHP::requiresParentheses");
    return true;
}

PrecedenceSpecifier
Unparse_PHP::getPrecedence(SgExpression* exp) {
    ROSE_ASSERT(!"UnparsePHP::getPrecedence");
    return 0;
}

AssociativitySpecifier
Unparse_PHP::getAssociativity(SgExpression* exp) {
    ROSE_ASSERT(!"UnparsePHP::getAssociativity");
    return e_assoc_left;
}
