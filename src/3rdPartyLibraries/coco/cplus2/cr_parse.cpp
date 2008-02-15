//**********************************************************
//   CR_PARSE.CPP
//   Coco/R C++ Support Library.
//   Author: Frankie Arzu <farzu@uvg.edu.gt>
//
//   Jun 12, 1996  Version 1.06
//      Many fixes and suggestions thanks to
//      Pat Terry <p.terry@.ru.ac.za>
//**********************************************************

#include "cr_parse.hpp"
#include "cr_error.hpp"
#include <stdio.h>

const int NSETBITS = 16;

CRParser::CRParser(AbsScanner *S, CRError *E)
{
  if (!E || !S) {
    fprintf(stderr, "CRParser::CRParser: No Scanner or No Error Mgr\n");
    exit(1);
  }
  Scanner = S;
  Error = E;
  Sym = 0;
}

void CRParser::Parse()
{
  printf("Abstract CRParser::Parse() called\n"); exit(1);
}

void CRParser::GenError(int ErrorNo)
//++++ GenError is supposed to be "private" for Coco/R only.  If a user calls
//++++ it directly, that is fine, but there is no consistency check performed
{
  Error->StoreErr(ErrorNo, Scanner->NextSym);
}

void CRParser::SynError(int ErrorNo)
//++++ SynError is for users.  Note that we check for error number
//++++ clashes.  If the numbers are too small, we simply make them bigger!
{
  if (ErrorNo <= Error->MinUserError) ErrorNo = Error->MinUserError;
  Error->StoreErr(ErrorNo, Scanner->NextSym);
}

void CRParser::SemError(int ErrorNo)
//++++ SemError is for users.  Note that we check for error number
//++++ clashes.  If the numbers are too small, we simply make them bigger!
{
  if (ErrorNo <= Error->MinUserError) ErrorNo = Error->MinUserError;
  Error->StoreErr(ErrorNo, Scanner->CurrSym);
}

int CRParser::In(unsigned short int *SymbolSet, int i)
{
  return SymbolSet[i / NSETBITS] & (1 << (i % NSETBITS));
}

void CRParser::Expect(int n)
{
  if (Sym == n) Get(); else GenError(n);
}


