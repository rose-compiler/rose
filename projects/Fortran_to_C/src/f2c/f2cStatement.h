#ifndef _FORTRANTOC_H
#define _FORTRANTOC_H

#include "rose.h"
#include "sageBuilder.h"

namespace Fortran_to_C 
{
    void transFileName(SgFile*);
    void transProgramHeaderStatement(SgProgramHeaderStatement*);
    void transProcedureHeaderStatement(SgProcedureHeaderStatement*);
}

#endif  //_FORTRANTOC_H
