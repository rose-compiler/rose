#ifndef CODETHORN_H
#define CODETHORN_H

/*********************************
 * Author: Markus Schordan, 2012 *
 *********************************/

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

class CodeThornLanguageRestrictor : public LanguageRestrictor {
public:
  bool checkIfAstIsAllowed(SgNode* node);
};

#endif
