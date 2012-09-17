#ifndef CODETHORN_H
#define CODETHORN_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <sstream>

#include "rose.h"

#include "CFAnalyzer.h"
#include "Analyzer.h"
#include "Visualizer.h"
#include "LanguageRestrictor.h"

using namespace std;

/* general auxiliary functions */
string int_to_string(int x);
void write_file(std::string filename, std::string data);
string color(string name);

class CodeThornLanguageRestrictor : public LanguageRestrictor {
public:
  bool checkIfAstIsAllowed(SgNode* node);
};

#endif
