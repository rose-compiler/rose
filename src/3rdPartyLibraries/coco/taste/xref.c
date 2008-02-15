/**********************************************************
**   XREF.FRM
**   Coco/R C Taste Cross Reference Generator Example.
**   Author: Frankie Arzu <farzu@uvg.edu.gt>
**      from Moessenboeck's (1990) Oberon example
**
**   May 24, 1996  Version 1.06
**   Mar 31, 1998  Version 1.08
**      Support for MS VC++ 5.0
**   Oct 31, 1999   Version 1.14
**      Further compliance with latest gcc
**********************************************************/

#include  <stdio.h>
#include  <fcntl.h>
#include  <stdlib.h>
#include  <string.h>
#include  "xrefs.h"
#include  "xrefp.h"
#include  "crossref.h"

#if __MSDOS__ || MSDOS || WIN32 || __WIN32__
#  include  <io.h>
#else
#  include <unistd.h>
#  define  O_BINARY     0
#endif

struct ErrDesc {
  int nr, line, col;
  struct ErrDesc *next;
};

static char SourceName[256], ListName[256];
static FILE *lst;
static int Listinfo = 0;
struct ErrDesc *FirstErr = NULL, *LastErr;
static int Errors = 0;
extern Error_Func Custom_Error;

static char *ErrorMsg[] = {
#include "xrefe.h"
"User error number clash",
""
};

void StoreError(int nr, int line, int col, long pos)
/* Store an error message for later printing */
{ struct ErrDesc *NextErr;

  NextErr = (struct ErrDesc *) malloc(sizeof(struct ErrDesc));
  NextErr->nr = nr; NextErr->line = line; NextErr->col = col;
  NextErr->next = NULL;
  if (FirstErr == NULL) FirstErr = NextErr;
  else LastErr->next = NextErr;
  LastErr = NextErr;
  Errors++;
}

static long GetLine(long pos, char *line)
/* Read a source line.  Return empty line if eof */
{ char ch;
  int i;
  int col = 1;

  ch = S_CurrentCh(pos);
  while (ch && ch != LF_CHAR && ch != EOF_CHAR) {
    if (ch == TAB_CHAR) {
      i = TAB_SIZE - (col % TAB_SIZE) + 1;
      while (i--) { *line++ = ' '; col++; }
    }
    else if (ch != CR_CHAR) { *line++ = ch; col++; }
    pos++;
    ch = S_CurrentCh(pos);
  }
  *line = 0;
  if (ch != EOF_CHAR) return pos + 1;
  else return -1;
}

static void PrintErrMsg(int nr)
/* Modify this appropriately if semantic errors are to be handled by the */
/* switch (nr) statement */
{ if (nr <= MAXERROR) fprintf(lst, "%s", ErrorMsg[nr]);
  else switch (nr) {
    /* add your customized error messages here */
    default : fprintf(lst, "Compiler error");
  }
  fprintf(lst, " (E%d)", nr);
}

static void PrintErr(int nr, int col)
{ fprintf(lst, "*****");
  while (col--) fprintf(lst, " ");
  fprintf(lst, " ^ ");
  PrintErrMsg(nr);
  fprintf(lst, "\n");
}

static void PrintListing(void)
/* Print a source listing with error messages */
{ struct ErrDesc *nextErr;
  int lnr, errC;
  long srcPos;
  char line[255];

  fprintf(lst, "Listing:\n\n");
  srcPos = 0; nextErr = FirstErr;
  srcPos = GetLine(srcPos, line); lnr = 1; errC = 0;
  while (srcPos != -1) {
    fprintf(lst, "%5d  %s\n", lnr, line);
    while (nextErr != NULL && nextErr->line == lnr) {
      PrintErr(nextErr->nr, nextErr->col); errC++;
      nextErr = nextErr->next;
    }
    srcPos = GetLine(srcPos, line); lnr++;
  }
  if (nextErr != NULL) {
    fprintf(lst, "%5d\n", lnr);
    while (nextErr != NULL) {
      PrintErr(nextErr->nr, nextErr->col); errC++;
      nextErr = nextErr->next;
    }
  }
  fprintf(lst, "\n\n%5d syntax errors\n", errC);
}

void SummarizeErrors(void)
/* Summarize error messages */
{ struct ErrDesc *nextErr;
  nextErr = FirstErr;
  while (nextErr != NULL) {
    fprintf(lst, "\"%s\", Line %d, Col %d: ",
    SourceName, nextErr->line, nextErr->col);
    PrintErrMsg(nextErr->nr);
    fprintf(lst, "\n");
    nextErr = nextErr->next;
  }
}

void SourceListing(void)
/* Generate the source listing */
{ int i;

  strcpy(ListName, SourceName);
  i = strlen(ListName)-1;
  while (i>0 && ListName[i] != '.') i--;
  if (i>0) ListName[i] = '\0';

  strcat(ListName, ".lst");
  if ((lst = fopen(ListName, "w")) == NULL) {
    fprintf(stderr, "Unable to open List file %s\n", ListName);
    exit(EXIT_FAILURE);
  }
  PrintListing();
}

static void param_options(int argc, char *argv[])
{ int i;
  char *s;

  for (i = 1; i < argc; i++) {
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
{ lst = stderr;
  /* check on correct parameter usage */
  strcpy(SourceName, "");
  param_options(argc, argv);
  if (!SourceName[0]) {
    fprintf(stderr, "No input file specified\n");
    exit(EXIT_FAILURE);
  }
  /* open the source file (Scanner.S_src)  */
  if ((S_src = open(SourceName, O_RDONLY|O_BINARY)) == -1) {
    fprintf(stderr, "Unable to open input file %s\n", SourceName);
    exit(EXIT_FAILURE);
  }

  /* install error reporting procedure */
  Custom_Error = (Error_Func) StoreError;

  /* parse the source */
  Parse();
  close(S_src);

  if (Errors) {
    printf("Parsed with errors\n");
    if (Listinfo) SourceListing();
    else SummarizeErrors();
    exit(EXIT_FAILURE);
  } else {
    SourceListing();
    List(lst);
    fclose(lst);
    exit(EXIT_SUCCESS);
  }
  return EXIT_SUCCESS;
}


