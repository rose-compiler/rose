#ifndef _JOVIAL_TO_C_H
#define _JOVIAL_TO_C_H

#include "rose.h"
#include "sageBuilder.h"

/*
 1. void translateProgramHeaderStatement(SgProgramHeaderStatement*);
    Translate from SgProgramHeaderStatement to SgFunctionDeclaration

    Input:  START
            PROGRAM main ;
            TERM
    output: int main(){}

*/

namespace Jovial_to_C 
{
    void translateFileName(SgFile*);
    void translateProgramHeaderStatement(SgProgramHeaderStatement*);
    //    void updateVariableDeclaration(SgVariableDeclaration*);
    //    void fixJovialSymbolTable(SgNode*, bool);

    //    std::string emulateParameterValue(SgExpression*, SgExpression**);

    //    SgType* translateType(SgType*);
    void translateDoubleVal(SgFloatVal*);

    //    void replaceScalarArgs(std::vector<SgInitializedName*> & list, SgNode* root);

    //    SgExpression* foldBinaryOp(SgExpression*);
}

#endif  //_JOVIAL_TO_C_H
