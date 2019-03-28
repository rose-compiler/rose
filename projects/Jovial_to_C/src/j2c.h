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
    void translateJovialCompoolStatement (SgJovialCompoolStatement*);
    void translateProgramHeaderStatement (SgProgramHeaderStatement*);
    void translateJovialTableStatement   (SgJovialTableStatement*);
    void translateStopOrPauseStatement   (SgStopOrPauseStatement*);

    void translateInitializedName(SgInitializedName*);

 // SgType* translateType(SgType*);
    void translateDoubleVal(SgFloatVal*);

    PreprocessingInfo* translateJovialDefineDeclaration (SgJovialDefineDeclaration*);

    PreprocessingInfo* attachIncludeDeclaration(SgLocatedNode* target, const std::string & content,
                                                PreprocessingInfo::RelativePositionType position=PreprocessingInfo::before);

 // void replaceScalarArgs(std::vector<SgInitializedName*> & list, SgNode* root);
 // std::string emulateParameterValue(SgExpression*, SgExpression**);
 // SgExpression* foldBinaryOp(SgExpression*);
}

#endif  //_JOVIAL_TO_C_H
