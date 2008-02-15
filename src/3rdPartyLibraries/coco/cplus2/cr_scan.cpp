//**********************************************************
//   CR_SCAN.CPP
//   Coco/R C++ Support Library.
//   Author: Frankie Arzu <farzu@uvg.edu.gt>
//
//   Jun 12, 1996  Version 1.06
//      Many fixes and suggestions thanks to
//      Pat Terry <p.terry@.ru.ac.za>
//   Mar 13, 1998  Version 1.08
//      Support for MS VC++ 5.0
//   May 05, 1999  Version 1.12
//      Added support to get String with a long pos.
//   Jun 20, 2001
//      Destructor improved
//**********************************************************

#include "cr_scan.hpp"

#if __MSDOS__ || MSDOS || WIN32 || __WIN32__
#  include <io.h>
#else
#  include <unistd.h>
#endif

#ifdef NEXTSTEP   // NextStep
#  include <sys/file.h>
#  include <sys/types.h>
#  include <sys/uio.h>
#  define L_SET     0  // set the seek pointer
#  define L_INCR    1  // increment the seek pointer
#  define L_XTND    2  // extend the file size
#endif

#include <stdio.h>
#include <string.h>

CRScanner::CRScanner(int ignoreCase)
{
  Buffer = NULL; IgnoreCase = ignoreCase;
}

CRScanner::CRScanner(int SrcFile, int ignoreCase)
{
  Buffer = NULL;
  ReadFile(SrcFile); Reset(); IgnoreCase = ignoreCase;
}

CRScanner::~CRScanner()
{
  if (Buffer) delete [] Buffer;
}

void CRScanner::Reset()
{
  CurrLine = 1; LineStart = 0; BuffPos = -1; CurrCol = 0;
  ComEols = 0;
  NextSym.Init();
  NextCh();
}

void CRScanner::ReadFile(int SrcFile)
{
  long len;
  int  n;

  InputLen = len = lseek(SrcFile, 0L, SEEK_END);
  (void) lseek(SrcFile, 0L, SEEK_SET);

  Buffer = new unsigned char[len+1];
  n = read(SrcFile, (void *) Buffer, len);
  Buffer[n] = EOF_CHAR;
}

int CRScanner::EqualStr(char *s)
{
  long pos; char c;
  if (NextSym.Len != strlen(s)) return 0;
  pos = NextSym.Pos;
  while (*s) {
    c = CurrentCh(pos); pos++;
    if (IgnoreCase) c = Upcase(c);
    if (c != *s++) return 0;
  }
  return 1;
}

void CRScanner::NextCh()
{
  BuffPos++;
  Ch = CurrentCh(BuffPos);
  if (IgnoreCase) Ch = Upcase(Ch);
  if (Ch == TAB_CHAR) CurrCol += TAB_SIZE - (CurrCol % TAB_SIZE);
  else if (Ch == LF_CHAR) {
    CurrLine++; CurrCol = 0;
    LineStart = BuffPos + 1;
  }
  CurrCol++;
};

void CRScanner::GetString(long Pos, char *Buffer, int Max)
{
  while (Max--) { *Buffer++ = CurrentCh(Pos); Pos++; }
  *Buffer = '\0';
}

void CRScanner::GetString(AbsToken *Sym, char *Buffer, int Max)
{
  int Len = (Sym->Len < Max)? Sym->Len : Max;
  GetString(Sym->Pos, Buffer, Len);
}

void CRScanner::GetName(AbsToken *Sym, char *Buffer, int Max)
{
  int  Len = Sym->Len;
  long Pos = Sym->Pos;

  while (Len-- && Max--) {
    *Buffer = CurrentCh(Pos); Pos++;
    if (IgnoreCase) *Buffer = Upcase(*Buffer);
    Buffer++;
  }
  *Buffer = '\0';
}

long CRScanner::GetLine(long Pos, char *Line, int Max)
{
  int  Col = 1, i;
  char ch = CurrentCh(Pos);
  Pos++;
  while (ch && ch != LF_CHAR && ch != EOF_CHAR) {
    if (ch == TAB_CHAR) {
      i = TAB_SIZE - (Col % TAB_SIZE) + 1;
      while (i-- && Max > 0) { *Line++ = ' '; Col++; Max--; }
    }
    else if (ch != CR_CHAR && Max > 0) { *Line++ = ch; Col++; Max--; }
    ch = CurrentCh(Pos); Pos++;
  }
  *Line = 0;
  if (ch != EOF_CHAR) return Pos;
  return -1;
}

CRDosScanner::CRDosScanner(int ignoreCase)
{
  BufferVec[0] = NULL;
  IgnoreCase = ignoreCase;
}

CRDosScanner::CRDosScanner(int SrcFile, int ignoreCase)
{
  ReadFile(SrcFile); Reset(); IgnoreCase = ignoreCase;
}

CRDosScanner::~CRDosScanner()
{
  for (int i = 0; BufferVec[i]; i++) delete BufferVec[i];
}

void CRDosScanner::ReadFile(int SrcFile)
{
  long len;
  int i = 0, n;

  InputLen = len = lseek(SrcFile, 0L, SEEK_END);
  (void) lseek(SrcFile, 0L, SEEK_SET);

  while (len > BLKSIZE) {
    BufferVec[i] = new unsigned char[BLKSIZE];
    n=read(SrcFile, (void *) BufferVec[i], BLKSIZE);
    len -= n;
    i++;
  }
  BufferVec[i] = new unsigned char[len + 1];
  n = read(SrcFile, (void *) BufferVec[i], (unsigned) len);
  (BufferVec[i])[n] = EOF_CHAR;
  BufferVec[i+1] = NULL;
}


