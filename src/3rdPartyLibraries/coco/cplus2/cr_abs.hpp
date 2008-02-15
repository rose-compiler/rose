//**********************************************************
//   CR_ABS.HPP
//   Coco/R C++ Support Library.
//   Author: Frankie Arzu <farzu@uvg.edu.gt>
//
//   Jun 12, 1996  Version 1.06
//      Many fixes and suggestions thanks to
//      Pat Terry <p.terry@.ru.ac.za>
//   May 05, 1999  Version 1.12
//      Added options to retrieve token info
//   Jun 20, 2001
//      Virtual destructor added
//**********************************************************

#ifndef CR_ABS
#define CR_ABS

#include <stdlib.h>

class AbsToken {
// Abstract Token
  public:
    virtual ~AbsToken() { }
    int  Sym;              // Token Number
    int  Line, Col;        // line and column of current Token
    int  Len;              // length of current Token
    long Pos;              // file position of current Token
    int GetSym()
      { return Sym; }
    int SetSym(int sym)
      { return Sym = sym; };
    long GetPos()
      { return Pos; };
    void Init(int sym = 0, int line = 0, int col = 0, long pos = 0, int len = 0)
      { Sym = sym; Line = line; Col = col; Pos = pos; Len = len; };
};

class AbsScanner {
// Abstract Scanner
  public:
    virtual ~AbsScanner() {}

    AbsToken CurrSym;      // current (most recently parsed) token
    AbsToken NextSym;      // next (look ahead) token

    virtual int Get() = 0;
    // Retrieves next token

    virtual void Reset() = 0;
    // Reset the scanner

    virtual unsigned char CurrentCh(long pos) = 0;
    // Returns character at position pos in source file

    virtual void GetString(AbsToken *Sym, char *Buffer, int Max) = 0;
    // Retrieves at most Max characters from Sym into Buffer

    virtual void GetString(long Pos, char *Buffer, int Max) = 0;
    // Retrieves at most Max characters from position Pos into Buffer

    virtual void GetName(AbsToken *Sym, char *Buffer, int Max) = 0;
    // Retrieves at most Max characters from Sym into Buffer
    // Buffer is capitalized if IGNORE CASE was specified

    virtual long GetLine(long Pos, char *Line, int Max) = 0;
    // Retrieves at most Max characters (or until next line break)
    // from position Pos in source file into Line
};

class AbsError {
// Abstract Error Reporting
  public:
    virtual ~AbsError() { }

    virtual void Store(int nr, int line, int col, long pos) = 0;
    // Records that error nr has been detected at specified position

    virtual void StoreErr(int nr, AbsToken &Token) = 0;
    // Associates error nr with Token

    virtual void StoreWarn(int nr, AbsToken &Token) = 0;
    // Associates warning nr with Token

    virtual void SummarizeErrors() = 0;
    // Reports on all errors recorded
};

#endif // CR_ABS



