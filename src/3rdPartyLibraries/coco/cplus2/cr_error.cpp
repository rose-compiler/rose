//**********************************************************
//   CR_ERROR.CPP
//   Coco/R C++ Support Library.
//   Author: Frankie Arzu <farzu@uvg.edu.gt>
//
//   Jun 12, 1996  Version 1.06
//      Many fixes and suggestions thanks to
//      Pat Terry <p.terry@.ru.ac.za>
//   Oct 11, 1997  Version 1.07
//      Change definition of FileName to local Storage
//**********************************************************

#include "cr_error.hpp"
#include <string>

using std::string;

CRError::CRError(string name, AbsScanner *S, int MinUserNo, int MinErr)
{
  if (!S) {
    fprintf(stderr, "CRError::CRError: No Scanner specified\n");
    exit(1);
  }
  Scanner = S;
  FileName = name;
  lst = stderr;
  Errors = 0; FirstErr = LastErr = NULL; ErrorDist = MinErr;
  MinErrorDist = MinErr;
  MinUserError = MinUserNo;
}

CRError::~CRError()
{
  if (FirstErr) delete FirstErr;
}

void CRError::Store(int nr, int line, int col, long pos)
{
  ErrDesc *NextErr = new ErrDesc(nr, line, col);
  if (FirstErr == NULL) FirstErr = NextErr;
  else LastErr->SetNext(NextErr);
  LastErr = NextErr;
}

void CRError::StoreErr(int ErrorNo, AbsToken &Token)
{
  if (ErrorDist >= MinErrorDist)
    { Errors++; Store(ErrorNo, Token.Line, Token.Col, Token.Pos); }
  ErrorDist = 0;
}

void CRError::StoreWarn(int WarnNo, AbsToken &Token)
{
  Store(WarnNo, Token.Line, Token.Col, Token.Pos);
}

void CRError::PrintErrMsg(int nr)
{
  fprintf(lst, "%s (E%d)", GetErrorMsg(nr).c_str(), nr);
}

void CRError::PrintErr(int nr, int col)
{
  fprintf(lst, "*****");
  while (col--) fprintf(lst, " ");
  fprintf(lst, " ^ ");
  PrintErrMsg(nr);
  fprintf(lst, "\n");
}

void CRError::PrintListing(AbsScanner *Scanner)
// Print a source listing with error messages
{
  char line[255];
  fprintf(lst, "Listing:\n\n");
  long srcPos = 0;
  ErrDesc *nextErr = FirstErr;
  srcPos = Scanner->GetLine(srcPos, line, sizeof(line)-1);
  int lnr = 1;
  int errC = 0;
  while (srcPos != -1) {
    fprintf(lst, "%5d  %s\n", lnr, line);
    while (nextErr != NULL && nextErr->line == lnr) {
      PrintErr(nextErr->nr, nextErr->col);
      errC++;
      nextErr = nextErr->GetNext();
    }
    srcPos=Scanner->GetLine(srcPos, line, sizeof(line)-1);
    lnr++;
  }
  if (nextErr != NULL) {
    fprintf(lst, "%5d\n", lnr);
    while (nextErr != NULL) {
      PrintErr(nextErr->nr, nextErr->col);
      errC++;
      nextErr = nextErr->GetNext();
    }
  }
  fprintf(lst, "\n\n%5d syntax errors\n", errC);
}

void CRError::SummarizeErrors()
// Print a summary of error messages
{
  ErrDesc *nextErr = FirstErr;
  while (nextErr != NULL) {
    fprintf(lst, "\"%s\", Line %d, Col %d: ",
      FileName.c_str(), nextErr->line, nextErr->col);
    PrintErrMsg(nextErr->nr);
    fprintf(lst, "\n");
    nextErr = nextErr->GetNext();
  }
}


