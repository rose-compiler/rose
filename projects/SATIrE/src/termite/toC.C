/*
Copyright 2006 Christoph Bonitz (christoph.bonitz@gmail.com)
see LICENSE in the root folder of this project
*/
#include "TermRep.h"
#include <iostream>
#include <stdio.h>
#include "rose.h"
#include "PrologToRose.h"
#include "termparser.tab.h++"

extern int yyparse();
extern FILE* yyin;
extern PrologTerm* prote;

int main(int argc, char** argv) {
  //  cout << prote->getRepresentation();
  if (argc != 3) {
    cerr << "Usage: " << argv[0] << " termfile.pl sourcefile" << endl;
    return 1;
  }
  yyin = fopen( argv[1], "r" );
  yyparse();

  PrologToRose unparser(argv[2]);
  unparser.toRose(prote);
  return 0;
}
