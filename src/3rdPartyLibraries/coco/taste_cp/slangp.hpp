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

#ifndef slangParser_INCLUDE
#define slangParser_INCLUDE

#include "cr_parse.hpp"

const int MAXSYM = 1;
const int MAXERROR = 10;

class slangParser : CRParser
{
  public:
    slangParser(AbsScanner *S=NULL, CRError *E=NULL) : CRParser(S,E) {};
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

  protected:
    static unsigned short int SymSet[][MAXSYM];
    virtual void Get();
    void ExpectWeak (int n, int follow);
    int  WeakSeparator (int n, int syFol, int repFol);
  private:
    void Slang();
    void Expression();
    void SimExpr();
    void Term();
    void AddOp();
    void Factor();
    void MulOp();
    
};

#endif /* slangParser_INCLUDE */


