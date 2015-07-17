#include "MatlabParserInterface.h"
#include <cstdio>
#include <iostream>
#include "lex.h"

 void yyerror (char const *s) {
 	std::cerr << s << std::endl;
 }

extern int yy_flex_debug;
extern int beginParse(SgProject* &);
extern "C" FILE *yyin;

extern int yydebug;
extern void prep_lexer_for_script_file ();

SgProject* MatlabParserInterface::Parse(std::string file)
  {
    FILE *matlabFile = fopen(file.c_str(), "r");

    if (!matlabFile)
      {
	std::cout << "File Cannot be opened" << std::endl;
	return NULL;
      }

    yydebug = 1;

    prep_lexer_for_script_file ();
    
    // set flex to read from it instead of defaulting to STDIN:
    yyin = matlabFile;

    std::cout << "Going to read file\n" << std::flush;

    SgProject* project = NULL;
    beginParse(project);

    fclose(matlabFile);
   
    ROSE_ASSERT(project != NULL);
    
    return project;
  }

