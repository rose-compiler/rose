#ifndef _FORTRANTOC_H
#define _FORTRANTOC_H

#include "rose.h"
#include "sageBuilder.h"

namespace Fortran_to_C 
{
    void translateFileName(SgFile*);
    void translateProgramHeaderStatement(SgProgramHeaderStatement*);
    void translateProcedureHeaderStatement(SgProcedureHeaderStatement*);
    void translateFortranDoLoop(SgFortranDo*);
    void fixFortranSymbolTable(SgNode*, bool);

    void linearizeArrayDeclaration(SgArrayType*);
    void linearizeArraySubscript(SgPntrArrRefExp*);
}

#endif  //_FORTRANTOC_H
