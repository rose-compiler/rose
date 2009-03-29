// Copyright 2005,2006,2007 Viktor Pavlu
// $Id:$

// Author: Viktor Pavlu, 2007

#ifndef SHAPECOMMANDLINEPARSER_H
#define SHAPECOMMANDLINEPARSER_H

#include <string>
#include <fstream>
#include <iostream>

#include "ShapeAnalyzerOptions.h"

using namespace std;

class ShapeCommandLineParser : public CommandLineParser {
public:
  ShapeCommandLineParser() {;}
  virtual ~ShapeCommandLineParser() {;}

  virtual int handleOption(AnalyzerOptions *cl, int i, int argc, char **argv) {
      if (i >= argc) 
          return 0;

      ShapeAnalyzerOptions *scl = (ShapeAnalyzerOptions*)cl;

      int old_i = i;

      if (optionMatch(argv[i], "--nnh-graphs")) {
          scl->gdlShowIndividualGraphsOn();
      } else if (optionMatch(argv[i], "--no-nnh-graphs")) {
          scl->gdlShowIndividualGraphsOff();
      } else if (optionMatch(argv[i], "--srw-graphs")) {
          scl->gdlShowSummaryGraphOn();
      } else if (optionMatch(argv[i], "--no-srw-graphs")) {
          scl->gdlShowSummaryGraphOff();
      } else if (optionMatch(argv[i], "--foldgraphs")) {
          scl->gdlFoldGraphsOn();
      } else if (optionMatch(argv[i], "--no-foldgraphs")) {
          scl->gdlFoldGraphsOff();
      } else {
          // pass argument to parent for parsing
          return CommandLineParser::handleOption(cl, i, argc, argv);
      }
      return i+1-old_i;
  }

};

#endif
