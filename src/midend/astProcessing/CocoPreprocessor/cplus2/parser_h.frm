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

#ifndef -->ParserClass_INCLUDE
#define -->ParserClass_INCLUDE

#include "cr_parse.hpp"

const int MAXSYM = -->MaxSymSets;
const int MAXERROR = -->MaxErr;

class -->ParserClass : CRParser
{
  public:
    -->ParserClass(AbsScanner *S=NULL, CRError *E=NULL) : CRParser(S,E) {};
    void Parse();

    // MS: added for easier source-to-source translation
    inline void GetString(long Pos, char *Buffer, int Max) 
    { Scanner->GetString(Pos,Buffer,Max); }

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
    -->ProdHeaders
};

#endif /* -->ParserClass_INCLUDE */


