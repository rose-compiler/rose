//**********************************************************
//   XREF.FRM
//   Coco/R C++ Taste Example.
//   Adapted to C++ by Frankie Arzu <farzu@uvg.edu.gt>
//      from Moessenboeck's (1990) Oberon example
//
//   May 24, 1996  Version 1.06
//   Mar 31, 1998  Version 1.08
//      Support for MS VC++ 5.0
//   Oct 31, 1999   Version 1.14
//      Further compliance with latest gcc
//**********************************************************

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

#include -->ScanHeader
#include -->ParserHeader
#include "cr_error.hpp"
#include "crossref.hpp"

static FILE *lst;
static int Listinfo = 0;
static char SourceName[256];

class XREFError : public CRError {
  public:
    XREFError(char *name, AbsScanner *s) : CRError(name, s, MAXERROR) {};
    virtual char *GetUserErrorMsg(int n);
    virtual char *GetErrorMsg(int n) 
      { if (n <= MAXERROR) return ErrorMsg[n];
        else return GetUserErrorMsg(n);
      };
  private:
    static char *ErrorMsg[];
};

char *XREFError::ErrorMsg[] = {
#include -->ErrorFile
"User error number clash",
""
};

char *XREFError::GetUserErrorMsg(int n)
{ switch (n) {
    // Put your customized messages here
    default:
      return "Unknown error or conflicting error numbers used";
  }
}

void SourceListing(CRError *Error, CRScanner *Scanner)
// generate the source listing
{ int i;
  char ListName[256];

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
  -->ScanClass   *Scanner;
  -->ParserClass *Parser;
  XREFError      *Error;
  extern         RefTable Table;

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
  Scanner = new -->ScanClass(S_src, -->IgnoreCase);
  Error   = new XREFError(SourceName, Scanner);
  Parser  = new -->ParserClass(Scanner, Error);

  // parse the source
  Parser->Parse();
  close(S_src);

  if (Error->Errors) {
    printf("Parsed with errors\n");
    if (Listinfo) SourceListing(Error, Scanner);
    else Error->SummarizeErrors();
    exit(EXIT_FAILURE);
  } else {
    SourceListing(Error, Scanner);
    Table.Print(lst);
    fclose(lst);
  }

  delete Scanner;
  delete Parser;
  delete Error;
  return (EXIT_SUCCESS);
}


