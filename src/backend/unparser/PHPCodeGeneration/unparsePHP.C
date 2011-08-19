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
