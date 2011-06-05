#include "sage3basic.h"
#include "unparsePython.h"

#include <iostream>

Unparse_Python::Unparse_Python(Unparser* unp, std::string fname) :
    UnparseLanguageIndependentConstructs(unp, fname)
{
}

Unparse_Python::~Unparse_Python()
{
}

void
Unparse_Python::unparseStatement(SgStatement* stmt, SgUnparse_Info& info)
{
    Sg_File_Info *finfo = stmt->get_file_info();
    std::cout << finfo->get_line() << ":" << finfo->get_col() << std::endl;
}

void
Unparse_Python::unparseLanguageSpecificStatement(SgStatement*, SgUnparse_Info&)
{
}

void
Unparse_Python::unparseLanguageSpecificExpression(SgExpression*, SgUnparse_Info&)
{
}

void
Unparse_Python::unparseStringVal(SgExpression*, SgUnparse_Info&)
{
}
