/*
Copyright 2006 Christoph Bonitz (christoph.bonitz@gmail.com)
see LICENSE in the root folder of this project
*/
#include "TermRep.h"
#include <iostream>
#include "rose.h"
#include "PrologToRose.h"
#include "termparser.tab.h++"

extern int yyparse();
extern PrologTerm* prote;

int main() {
  yyparse();
//  cout << prote->getRepresentation();
  PrologToRose::toRose(prote);
  return 0;
}
