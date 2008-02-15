//**********************************************************
//   CR_SCAN.HPP
//   Coco/R C++ Support Library.
//   Author: Frankie Arzu <farzu@uvg.edu.gt>
//
//   Jun 12, 1996  Version 1.06
//      Many fixes and suggestions thanks to
//      Pat Terry <p.terry@.ru.ac.za>
//   May 05, 1999  Version 1.12
//      Added new methods to CRScanner (GetString with long)
//**********************************************************

#ifndef CR_SCAN_H
#define CR_SCAN_H

#include "cr_abs.hpp"
#include <cctype>

const int LF_CHAR = 10;
const int CR_CHAR = 13;
const int EOF_CHAR = 0;
const int TAB_CHAR = 9;
const int TAB_SIZE = 8;

inline unsigned char Upcase(char c)
  { // JJW 10-19-2007 this is a library function
    // return (c >= 'a' && c <= 'z') ? c-32 : c;
    return (unsigned char)std::toupper(c);}

class CRScanner: public AbsScanner {
// Standard Coco/R Scanner
  public:
    CRScanner() { Buffer = NULL; };
    // Default constructor

    CRScanner(int ignoreCase);
    // Constructor allowing for IGNORE CASE

    CRScanner(int SrcFile, int ignoreCase);
    // Constructor specifying named ScFile and allowing for IGNORE CASE

    ~CRScanner();

    virtual void Reset();
    // Reads entire source code into internal buffer ready for processing

    int EqualStr(char *s);
    // Compares current lexeme for equality with s (for key word recognition)

    void SetIgnoreCase() { IgnoreCase = 1; };

    // following were specified in abstract base class
    virtual int  Get() = 0;
    virtual void GetString(AbsToken *Sym, char *Buffer, int Max);
    virtual void GetString(long Pos, char *Buffer, int Max);
    virtual void GetName(AbsToken *Sym, char *Buffer, int Max);
    virtual long GetLine(long Pos, char *Line, int Max);
  private:
    unsigned char *Buffer;
  protected:
    int   ComEols;         // number of EOLs in a comment
    long  BuffPos;         // current position in buf
    int   CurrCol;         // current Column Number
    long  InputLen;        // source file size
    int   CurrLine;        // current input line (may be higher than line)
    long  LineStart;       // start position of current line
    unsigned char  Ch;
    int   IgnoreCase;

    virtual void ReadFile(int SrcFile);
    virtual inline unsigned char CurrentCh(long Pos)
      { return Buffer[Pos]; };
    void NextCh();
};

const int BLKSIZE = 16384;

class CRDosScanner: public CRScanner {
// Standard Coco/R Scanner
  public:
    CRDosScanner(int ignoreCase);
    CRDosScanner(int SrcFile, int ignoreCase);
    ~CRDosScanner();
  private:
    unsigned char *BufferVec[32];  // source buffer for low-level access
  protected:
    virtual void ReadFile(int SrcFile);
    virtual inline unsigned char CurrentCh(long Pos)
      { return (BufferVec[Pos / BLKSIZE])[Pos % BLKSIZE]; };
};

#endif // CR_SCAN_H



