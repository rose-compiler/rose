#ifndef _FORTRANTOC_H
#define _FORTRANTOC_H

#include "rose.h"
#include "sageBuilder.h"

namespace Fortran_to_C 
{
    void translateFileName(SgProject*);
    void translateProgramHeaderStatement(SgProject*);
    void translateProcedureHeaderStatement(SgProject*);
    void translateFortranDoLoop(SgProject*);
    void fixFortranSymbolTable(SgNode*, bool);

    void lineralizeArray(SgProject*); 
}

#endif  //_FORTRANTOC_H
