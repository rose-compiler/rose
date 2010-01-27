// Author: Viktor Pavlu, 2007, 2009

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
          scl->convertToNNHOn();
          scl->gdlShowIndividualGraphsOn();
      } else if (optionMatch(argv[i], "--no-nnh-graphs")) {
          scl->convertToNNHOff();
          scl->gdlShowIndividualGraphsOff();
      } else if (optionMatch(argv[i], "--srw-graphs")) {
          scl->convertToSRWOn();
          scl->gdlShowSummaryGraphOn();
      } else if (optionMatch(argv[i], "--no-srw-graphs")) {
          scl->convertToSRWOff();
          scl->gdlShowSummaryGraphOff();
      } else if (optionMatch(argv[i], "--foldgraphs")) {
          scl->gdlFoldGraphsOn();
      } else if (optionMatch(argv[i], "--no-foldgraphs")) {
          scl->gdlFoldGraphsOff();
      } else if (optionMatch(argv[i], "--annotate-aliases")) {
          scl->aliasesAnnotateOn();
      } else if (optionMatch(argv[i], "--output-alias-text")) {
          scl->aliasesAnnotateOn();
          scl->aliasesOutputTextOn();
      } else if (optionMatch(argv[i], "--output-alias-source")) {
          scl->aliasesAnnotateOn();
          scl->aliasesOutputSourceOn();
      } else if (optionMatchPrefix(argv[i], "--alias-statistics=")) {
          if (strlen(argv[i]+prefixLength) == 0) {
              scl->setOptionsErrorMessage("empty alias statistics filename");
              return 1;
          }
          scl->aliasesAnnotateOn();
          scl->setaliasStatisticsFile(strdup(argv[i]+prefixLength));

      // Variants for Precision/Runtime tradeoff evaluation

      } else if (optionMatch(argv[i], "--var-alias-from-graphset")) {
          scl->variantAliasesFromGraphSetOn();
      } else if (optionMatch(argv[i], "--var-alias-from-summary-graph")) {
          scl->variantAliasesFromGraphSetOff();

      } else if (optionMatch(argv[i], "--var-keep-tempvars")) {
          scl->variantKeepTempvarsOn();
      } else if (optionMatch(argv[i], "--var-delete-tempvars")) {
          scl->variantKeepTempvarsOff();

      } else if (optionMatch(argv[i], "--var-keep-helpvars")) {
          scl->variantKeepHelpvarsOn();
      } else if (optionMatch(argv[i], "--var-delete-helpvars")) {
          scl->variantKeepHelpvarsOff();

      } else if (optionMatch(argv[i], "--var-aliases-common-tail")) {
          scl->variantAliasesCommonTailOn();
      } else if (optionMatch(argv[i], "--var-aliases-no-common-tail")) {
          scl->variantAliasesCommonTailOff();

      // remove {}->{} garbage rings when unreachable

      } else if (optionMatch(argv[i], "--shape-gc")) {
          scl->performShapeGCOn();
      } else if (optionMatch(argv[i], "--no-shape-gc")) {
          scl->performShapeGCOff();

      // debug output flags
      } else if (optionMatch(argv[i], "--debug-astmatch")) {
          scl->debugmessageAstmatchOn();
      } else if (optionMatch(argv[i], "--debug-assign")) {
          scl->debugmessageAssignOn();
      } else if (optionMatch(argv[i], "--debug-tassign")) {
          scl->debugmessageTassignOn();
      } else if (optionMatch(argv[i], "--debug-lowlevel")) {
          scl->debugmessageLowlevelOn();
      } else if (optionMatch(argv[i], "--debug-materialisation")) {
          scl->debugmessageMaterialisationOn();

      } else {
          // pass argument to parent for parsing
          return CommandLineParser::handleOption(cl, i, argc, argv);
      }
      return i+1-old_i;
  }

};

#endif
