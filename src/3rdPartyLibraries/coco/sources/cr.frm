#include  <stdio.h>
#include  <stdlib.h>
#include  <fcntl.h>

#if __MSDOS__ || MSDOS || WIN32 || __WIN32__
#  include  <io.h>
#else
#  include <unistd.h>
#  define  O_BINARY     0
#endif

#if NEXTSTEP
#   include <sys/file.h>
#endif

#include  <string.h>
#include  "crt.h"
#include  "crx.h"
#include  "cra.h"
#include  "crf.h"
#include  "crp.h"
#include  "crs.h"

struct ErrDesc {
  int nr, line, col;
  struct ErrDesc *next;
};

static FILE *lst;
static struct ErrDesc *FirstErr = NULL, *LastErr;
int Errors = 0;
extern Error_Func Custom_Error;

static char *ErrorMsg[] = {
#include -->ErrorFile
"User error number clash",
""
};

static char Version[]     = "1.17 (C/C++ Version)";
static char ReleaseDate[] = "Sep 17 2002";

/* --------------------------- Help ------------------------------------------ */

static void Help(void)
{ fprintf(stdout,
    "Coco/R - Compiler-Compiler V%s "
    "(Frankie Arzu, %s)\n\n"
    "Usage: COCOR  [(/|-)Options] <Grammar.atg>   for Dos/Windows\n"
    "Usage: COCOR  [-Options] <Grammar.atg>       for Unix\n"
    "Example: COCOR -C -S Test.atg\n\n"
    "Options:\n"
    "A  - Trace automaton               C  - Generate compiler module\n"
    "D  - Include source debugging information (#line)\n"
    "F  - Give Start and Follower sets  G  - Print top-down graph\n"
    "L  - Force listing                 O  - Terminal conditions use OR only\n"
    "P  - Generate parser only          S  - Print symbol table\n"
    "T  - Grammar tests only            Q  - Quiet (Edit) mode\n"
    "X  - Generate C++ with classes     Z  - Force extensions to .hpp and .cpp\n\n"
    "Environment variables:\n"
    "CRFRAMES:  Search directory for frames file. If not specified,\n"
    "           frames must be in the working directory.\n"
    "CRHEXT:    Extension for the '.h' generated files. If not specified,\n"
    "           '.h' for C, '.hpp' for C++ (Dos and Unix). \n"
    "CRCEXT:    Extension for the '.c' generated files. If not specified,\n"
    "           '.c' for C, '.cpp' for C++ (Dos and Unix). \n"
    "Frame and extension options may also be set using -Dvarname=value\n",
    Version, ReleaseDate);
}

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
{
  if (nr <= MAXERROR) fprintf(lst, "%s", ErrorMsg[nr]);
  else
    switch (nr) {
    case 101:
      fprintf(lst, "character set may not be empty");
      break;
    case 102:
      fprintf(lst, "string literal may not extend over line end");
      break;
    case 103:
      fprintf(lst, "a literal must not have attributes");
      break;
    case 104:
      fprintf(lst, "this symbol kind not allowed in production");
      break;
    case 105:
      fprintf(lst, "attribute mismatch between declaration and use");
      break;
    case 106:
      fprintf(lst, "undefined string in production");
      break;
    case 107:
      fprintf(lst, "name declared twice");
      break;
    case 108:
      fprintf(lst, "this type not allowed on left side of production");
      break;
    case 109:
      fprintf(lst, "earlier semantic action was not terminated");
      break;
    case 111:
      fprintf(lst, "no production found for grammar name");
      break;
    case 112:
      fprintf(lst, "grammar symbol must not have attributes");
      break;
    case 113:
      fprintf(lst, "a literal must not be declared with a structure");
      break;
    case 114:
      fprintf(lst, "semantic action not allowed here");
      break;
    case 115:
      fprintf(lst, "undefined name");
      break;
    case 116:
      fprintf(lst, "attributes not allowed in token declaration");
      break;
    case 117:
      fprintf(lst, "name does not match name in heading");
      break;
    case 118:
      fprintf(lst, "unacceptable constant value");
      break;
    case 119:
      fprintf(lst, "may not ignore CHR(0)");
      break;
    case 120:
      fprintf(lst, "token might be empty");
      break;
    case 121:
      fprintf(lst, "token must not start with an iteration");
      break;
    case 122:
      fprintf(lst, "comment delimiters may not be structured");
      break;
    case 123:
      fprintf(lst, "only terminals may be weak");
      break;
    case 124:
      fprintf(lst, "literal tokens may not contain white space");
      break;
    case 125:
      fprintf(lst, "comment delimiter must be 1 or 2 characters long");
      break;
    case 126:
      fprintf(lst, "character set contains more than one character");
      break;
    case 127:
      fprintf(lst, "\ncould not make deterministic automaton, check previous token errors");
      break;
    case 129:
      fprintf(lst, "literal tokens may not be empty");
      break;
    case 130:
      fprintf(lst, "IGNORE CASE must appear earlier");
      break;
    }
  fprintf(lst, " (E%d)",nr);
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
    if (Q_option)
      fprintf(lst, "%s (%d, %d) ",
              source_name, nextErr->line, nextErr->col);
    else
      fprintf(lst, "\"%s\", Line %d, Col %d: ",
              source_name, nextErr->line, nextErr->col);
    PrintErrMsg(nextErr->nr);
    fprintf(lst, "\n");
    nextErr = nextErr->next;
  }
}

static void param_options(int argc,char *argv[])
{ int i;
  for (i = 1; i < argc; i++)
#if __MSDOS__ || MSDOS || WIN32 || __WIN32__
    if (*argv[i] == '-' || *argv[i] == '/') SetOptions(argv[i]);
#else
    if (*argv[i] == '-') SetOptions(argv[i]);
#endif
    else strcpy(source_name, argv[i]);
}

static void FileGetName(char *name, char *filename)
{  int i;
   strcpy(name, filename);
   i = strlen(name)-1;
   while (i>0 && name[i] != '.') i--;
   name[i]='\0';
}

int main(int argc, char *argv[])
{
  char name[100];
  lst = stderr;
  if (argc == 1) {
    Help();
    exit(EXIT_FAILURE);
  }

  InitFrameVars();
  source_name[0] = '\0';
  param_options(argc, argv);

  /* check on correct parameter usage */
  if (source_name[0] == 0) {
    fprintf(stderr, "No input file specified");
    exit(EXIT_FAILURE);
  }

  /* open the Source file (Scanner.S_src)  */
  if ((S_src = open(source_name, O_RDONLY|O_BINARY)) == -1) {
    fprintf(stderr, "Unable to open input file %s\n", source_name);
    exit(EXIT_FAILURE);
  }

  /* open the listing file */
  if (L_option) {
    FileGetName(name, source_name);
    strcat(name, ".lst");
    lstfile = OpenFile(name, "w", 1);
  } else if (!Q_option) lstfile = stderr; else lstfile = stdout;

  /* install error reporting procedure */
  Custom_Error = (Error_Func) StoreError;

  InitTab();
  InitScannerTab();
  Parse();
  close(S_src);

  lst = lstfile;
  if (Errors && !L_option) SummarizeErrors();
  else if (Errors || L_option) PrintListing();

  if (Errors) {  /* Check for Syntax Errors */
    fclose(lstfile);
    exit(EXIT_FAILURE);
  }

  MakeScanner();
  MakeParser();

  if (Errors) {  /* Check for LL(1) Errors, Undeclared Symbols, etc */
    fclose(lstfile);
    exit(EXIT_FAILURE);
  }

  SetupFrameVars();
  if (!T_option) {
    if (!P_option) GenScanner();
    GenParser();
    GenHeaders();
    if (C_option) GenCompiler();
  }

  if (L_option) {
    if (S_option) {
      ShowClassTab();
      ShowTermTab();
      ShowCommentTab();
      ShowSymSetTab();
      ShowNTermTab();
    }
    if (A_option) ShowDFA();
    fclose(lstfile);
  }
  DoneTab();
  DoneScannerTab();
  return (EXIT_SUCCESS);
}


