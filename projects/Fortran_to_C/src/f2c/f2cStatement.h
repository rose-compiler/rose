#ifndef _FORTRANTOC_H
#define _FORTRANTOC_H

#include "rose.h"
#include "sageBuilder.h"

namespace Fortran_to_C 
{
    void translateFileName(SgFile*);
    void translateProgramHeaderStatement(SgProgramHeaderStatement*);
    void translateProcedureHeaderStatement(SgProcedureHeaderStatement*);
}

#endif  //_FORTRANTOC_H
