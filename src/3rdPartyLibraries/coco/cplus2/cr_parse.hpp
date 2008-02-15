//**********************************************************
//   CR_PARSE.HPP
//   Coco/R C++ Support Library.
//   Author: Frankie Arzu <farzu@uvg.edu.gt>
//
//   Jun 12, 1996  Version 1.06
//      Many fixes and suggestions thanks to
//      Pat Terry <p.terry@.ru.ac.za>
//   Jun 20, 2001
//      Virtual destructors added
//**********************************************************

#ifndef CR_PARSER_H
#define CR_PARSER_H

#include "cr_abs.hpp"
#include "cr_error.hpp"

class CRParser {
// Abstract Parser
  public:
    CRParser(AbsScanner *S = NULL, CRError *E = NULL);
    // Constructs abstract parser, and associates it with scanner S and
    // customized error reporter E

    virtual ~CRParser() { }

    void Parse();
    // Abstract parser

    void SynError(int ErrorNo);
    // Records syntax error ErrorNo

    void SemError(int ErrorNo);
    // Records semantic error ErrorNo

  protected:
    virtual void Get() = 0;
    int  In(unsigned short int *SymbolSet, int i);
    void Expect(int n);
    void GenError(int ErrorNo);
    AbsScanner *Scanner;
    CRError    *Error;
    int        Sym;
};

#endif // CR_PARSER_H



