//****************************************************************
//   CPLUS2\PARSER_H.FRM
//   Coco/R C++ Support Frames.
//   Author: Frankie Arzu <farzu@uvg.edu.gt>
//
//   Jun 12, 1996  Version 1.06
//      Many fixes and suggestions thanks to
//      Pat Terry <p.terry@.ru.ac.za>
//   May 05, 1999  Version 1.12
//      Added methods to parser to retrieve token position easily
//****************************************************************

#ifndef OmpLoopRATransformationParser_INCLUDE
#define OmpLoopRATransformationParser_INCLUDE

#include "cr_parse.hpp"
#include <string>
#include "OmpLoopRATransformationParser_Declarations.h"

// MSTEST
class OmpLoopRATransformationParser : CRParser
{
  public:
    string iVarName;
    string iContName;

  public:
    static const int MAXSYM = 13;
    static const int MAXERROR = 217;

    OmpLoopRATransformationParser(AbsScanner *S=NULL, CRError *E=NULL) : CRParser(S,E) {};
    void Parse();
    inline void LexString(char *lex, int size)
    { Scanner->GetString(&Scanner->CurrSym, lex, size); };
    inline void LexName(char *lex, int size)
    { Scanner->GetName(&Scanner->CurrSym, lex, size); };
    inline long LexPos()
    { return Scanner->CurrSym.GetPos(); };
    inline void LookAheadString(char *lex, int size)
    { Scanner->GetString(&Scanner->NextSym, lex, size); };
    inline void LookAheadName(char *lex, int size)
    { Scanner->GetName(&Scanner->NextSym, lex, size); };
    inline long LookAheadPos()
    { return Scanner->NextSym.GetPos(); };
    inline int Successful()
    { return Error->Errors == 0; }

    inline void InitSubLanguage() {
	Get();
    }
  protected:
    static unsigned short int SymSet[][MAXSYM];
    virtual void Get();
    void ExpectWeak (int n, int follow);
    int  WeakSeparator (int n, int syFol, int repFol);
  public:
    void SgNode();
    void SgSupport();
    void SgLocatedNode();
    void SgSymbol();
    void SgInitializedNameNT();
    void SgFileNT();
    void SgClassDeclarationNT();
    void SgAttribute();
    void SgGlobalNT();
    void SgDeclarationStatement();
    void SgInitializer();
    void SgBitAttribute();
    void SgStatement(unsigned int forNestingLevel);
    void SgExpression();
    void SgExpressionRootNT();
    void SgBasicBlockNT(unsigned int forNestingLevel);
    void SgFunctionCallExpNT();
    void SgC_PreprocessorDirectiveStatem();
    void SgCommentStatement();
    void SgScopeStatement(unsigned int forNestingLevel);
    void SgForInitStatementNT(unsigned int forNestingLevel);
    void SgUnsignedLongValNT();
    void SgFunctionParameterListNT();
    void SgCtorInitializerListNT();
    void SgFunctionDeclarationNT();
    void SgClassDefinitionNT();
    void SgFunctionDefinitionNT();
    void SgExprListExpNT();
    void SgConstructorInitializerNT();
    void SgValueExp();
    void SgBinaryOp();
    void SgUnaryOp();
    void SgClassSymbol();
    void SgFunctionSymbolNT();
    
};

#endif /* OmpLoopRATransformationParser_INCLUDE */


