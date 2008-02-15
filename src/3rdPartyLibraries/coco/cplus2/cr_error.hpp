//**********************************************************
//   CR_ERROR.HPP
//   Coco/R C++ Support Library.
//   Author: Frankie Arzu <farzu@uvg.edu.gt>
//
//   Jun 12, 1996  Version 1.06
//      Many fixes and suggestions thanks to
//      Pat Terry <p.terry@.ru.ac.za>
//   Oct 11, 1997   Version 1.07
//      Change definition of FileName to local storage
//**********************************************************

#ifndef CR_ERROR_H
#define CR_ERROR_H

#include "cr_abs.hpp"
#include <stdio.h>
#include <string>

const int MINERRORNO = 1000;   // Error numbers > MINERRORNO are user numbers

class ErrDesc {
  public:
    ErrDesc(int n, int l = 0, int c = 0)
      { nr = n; line = l; col = c; next = NULL; };
    ~ErrDesc()
      { if (next) delete next; };
    void SetNext(ErrDesc *n)
      { next = n; };
    ErrDesc *GetNext()
      { return next; };
    int nr, line, col;
  private:
    ErrDesc *next;
};

class CRError : public AbsError {
  public:
    CRError(std::string name, AbsScanner *S,
            int MinUserNo = MINERRORNO, int MinErr = 2);
    // Create error reporter and associate with SourceFile and scanner S
    // Set MinErr as a limit on error message clustering, and MinUserNo as
    // a barrier for distinguishing private and user defined numbers

    ~CRError();

    virtual void ReportError(int nr)
    // Associates error nr with most recently parsed token
      { if (nr <= MinUserError) nr = MinUserError;
        StoreErr(nr, Scanner->CurrSym); };

    virtual std::string GetErrorMsg(int n) { return ""; };
    // Retrieves automatically derived error message text for error n

    virtual std::string GetUserErrorMsg(int n) { return ""; };
    // Retrieves user supplied error message text for error n

    void SetOutput(FILE *file) { lst = file; };
    // Specifies file for error listing and reporting

    void PrintListing(AbsScanner *Scanner);
    // Generates source listing with error messages merged into it

    virtual void SummarizeErrors();
    // Generates listing of errors only

    int MinUserError;  // Barrier for distinguishing private error numbers
    int ErrorDist;     // Number of tokens parsed since last error detected
    int Errors;        // Number of errors reported

    // following were specified in abstract base class
    virtual void Store(int nr, int line, int col, long pos);
    virtual void StoreErr(int nr, AbsToken &Token);
    virtual void StoreWarn(int nr, AbsToken &Token);
  protected:
    AbsScanner *Scanner;
    FILE *lst;
    std::string FileName;
    int MinErrorDist;
    void PrintErrMsg(int nr);
    void PrintErr(int nr, int col);
    ErrDesc *FirstErr, *LastErr;
};

#endif // CR_ERROR_H



