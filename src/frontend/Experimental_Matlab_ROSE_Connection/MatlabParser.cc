#include "rose.h"

#include "MatlabParser.h"
#include <cstdio>
#include <iostream>
#include <string>
#include "lex.h"

extern int yy_flex_debug;
extern int beginParse(SgProject* &, int argc, char* argv[]);
extern "C" FILE *yyin;

extern int yydebug;
extern void prep_lexer_for_script_file ();


void yyerror (char const *s) {
  std::cerr << s << std::endl;
 }

SgProject* MatlabParser::frontend(int argc, char* argv[])
{
  ROSE_ASSERT(argc > 1);

  FILE*       matlabFile = fopen(argv[argc-1], "r");

  if (!matlabFile)
  {
    std::cout << "File " << argv[1] << " cannot be opened." << std::endl;
    ROSE_ASSERT(false);
    return NULL;
  }

  SgProject* project = NULL;

  yydebug = 0;
  prep_lexer_for_script_file ();

  // set flex to read from it instead of defaulting to STDIN:
  yyin = matlabFile;

  beginParse(project, argc, argv);
  fclose(matlabFile);

  ROSE_ASSERT(project != NULL);
  project->skipfinalCompileStep(true);
  return project;
}
