//****************************************************************
//   CPLUS2\COMPILER.FRM
//   Coco/R C++ Support Frames.
//   Author: Frankie Arzu <farzu@uvg.edu.gt>
//
//   Jun 12, 1996  Version 1.06
//      Many fixes and suggestions thanks to
//      Pat Terry <p.terry@.ru.ac.za>
//   Oct 11, 1997  Version 1.07
//      Fix frame to support relative and absolute paths for UNIX
//   Mar 31, 1998  Version 1.08
//      Fix frame to support Visual C++ 5.0
//   Oct 31, 1999  Version 1.14
//      Further compliance with latest gcc
//****************************************************************

#include  <stdio.h>
#include  <fcntl.h>
#include  <stdlib.h>
#include  <string.h>

#if __MSDOS__ || MSDOS || WIN32 || __WIN32__
#  include  <io.h>
#else
#  include <unistd.h>
#  define  O_BINARY     0
#endif

// You may need to add to the #include directives and to add other
// global declarations and definitions here

#include "agpps.hpp"
#include "agppp.hpp"
#include "cr_error.hpp"

static FILE *lst;
static int Listinfo = 0;
static char SourceName[256];

class MyError : public CRError {
  public:
    MyError(char *name, AbsScanner *S) : CRError(name, S, MAXERROR) {};
    virtual char *GetUserErrorMsg(int n);
    virtual char *GetErrorMsg(int n)
      { if (n <= MAXERROR) return ErrorMsg[n];
        else return GetUserErrorMsg(n);
      };
  private:
    static char *ErrorMsg[];
};

char *MyError::ErrorMsg[] = {
#include "agppe.hpp"
"User error number clash",
""
};

char *MyError::GetUserErrorMsg(int n)
{ switch (n) {
    // Put your customized messages here
    default:
      return "Unknown error or conflicting error numbers used";
  }
}

void SourceListing(CRError *Error, CRScanner *Scanner)
// generate the source listing
{ char ListName[256];
  int  i;

  strcpy(ListName, SourceName);
  i = strlen(ListName)-1;
  while (i>0 && ListName[i] != '.') i--;
  if (i>0) ListName[i] = '\0';

  strcat(ListName, ".lst");
  if ((lst = fopen(ListName, "w")) == NULL) {
    fprintf(stderr, "Unable to open List file %s\n", ListName);
    exit(EXIT_FAILURE);
  }
  Error->SetOutput(lst);
  Error->PrintListing(Scanner);
  fclose(lst);
}

static void param_options(int argc, char *argv[])
// scan command line for options.  You may want to add to these
{ char *s;

  for (int i = 1; i < argc; i++) {
    s = argv[i];
#if __MSDOS__ || MSDOS || WIN32 || __WIN32__
    if (*s == '-' || *s == '/') {
#else
    if (*s == '-') {
#endif
      s++;
      while (*s) {
        switch (*s) {
          case 'l' :
          case 'L' : Listinfo = 1; break;
        }
        s++;
      }
    } else strcpy(SourceName,s);
  }
}

int main(int argc, char *argv[])
{ int S_src;
  agppScanner   *Scanner;
  agppParser   *Parser;
  MyError        *Error;

  lst = stderr;
  // check on correct parameter usage
  strcpy(SourceName, "");
  param_options(argc, argv);
  if (!SourceName[0]) {
    fprintf(stderr, "No input file specified\n");
    exit(EXIT_FAILURE);
  }

  // open the source file S_src
  if ((S_src = open(SourceName, O_RDONLY | O_BINARY)) == -1) {
    fprintf(stderr, "Unable to open input file %s\n", SourceName);
    exit(EXIT_FAILURE);
  }

  // instantiate Scanner, Parser and Error handler
  Scanner = new agppScanner(S_src, 0);
  Error   = new MyError(SourceName, Scanner);
  Parser  = new agppParser(Scanner, Error);

  // parse the source
  Parser->Parse();
  close(S_src);

  // Add to the following code to suit the application
  if (Error->Errors) fprintf(stderr, "Compilation errors\n");
  if (Listinfo) SourceListing(Error, Scanner);
  else if (Error->Errors) Error->SummarizeErrors();

  if (Error->Errors) exit(EXIT_FAILURE);
  delete Scanner;
  delete Parser;
  delete Error;
  return (EXIT_SUCCESS);
}


