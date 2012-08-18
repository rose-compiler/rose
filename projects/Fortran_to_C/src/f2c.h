#ifndef _FORTRANTOC_H
#define _FORTRANTOC_H

#include "rose.h"
#include "sageBuilder.h"

/*
 1. void translateProgramHeaderStatement(SgProgramHeaderStatement*);
    Translate from SgProgramHeaderStatement to SgFunctionDeclaration

    Input:  program() 
            end program
    output: int main(){}

  2. void translateProcedureHeaderStatement(SgProcedureHeaderStatement*);
     Translate from SgProcedureHeaderStatement to SgFunctionDeclaration
     Input: subroutine a()            integer functin b()
            end subroutine            end function
     output: void a(){}               int b(){}
  
  3. void translateFortranDoLoop(SgFortranDo*);
     Translate Fortran do loop to C for loop
     Input:  do i=1,10,2
             enddo
     Output: int i=0;
             for(i=1;i<=10;i=i+2){}

  4. void fixFortranSymbolTable(SgNode*, bool);
     Link variable declaration for the Fortran implicit variables
     Input: program
              a = 2.
            end program
     Output: int main()
             {
               float a;
               a = 2.;
             } 

  5. void linearizeArrayDeclaration(SgArrayType*);
     Linearize Fortran Array declaration.  
     Converting multi-dimensional array into single-dimensional array
     Input:  real*4 a(i1,i2,i3);       real*4 b(il1:ih1, il2:ih2);
     output: float a[i1*i2*i3];        float b((ih1-il1+1) * (ih2-il2+1));

  6. void linearizeArraySubscript(SgPntrArrRefExp*);
     Linearize Fortran Array subscript.
     Converting multi-dimensional subscript list into single-dimensional subscript
     Input:   real*4 a(i1,i2)
              a(ia,ib) = 2.
     Output:  float a(i1*i2);
              a[(ia-1)+(ib-ia+1)*(ib-1)] = 2.
*/

namespace Fortran_to_C 
{
    void translateFileName(SgFile*);
    void translateProgramHeaderStatement(SgProgramHeaderStatement*);
    void translateProcedureHeaderStatement(SgProcedureHeaderStatement*);
    void translateFortranDoLoop(SgFortranDo*);
    void fixFortranSymbolTable(SgNode*, bool);

    void translateArrayDeclaration(SgArrayType*);
    void translateArraySubscript(SgPntrArrRefExp*);
    void linearizeArrayDeclaration(SgArrayType*);
    void linearizeArraySubscript(SgPntrArrRefExp*);
  
    SgExpression* getFortranDimensionSize(SgExpression*);
    SgExpression* get0basedIndex(SgExpression*, SgExpression*);
}

#endif  //_FORTRANTOC_H
