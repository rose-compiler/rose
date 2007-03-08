// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: AnalyzerOptions.h,v 1.3 2007-03-08 15:36:48 markus Exp $

// Author: Markus Schordan, 2006.

#ifndef ANALYZEROPTIONS_H
#define ANALYZEROPTIONS_H

#include <string>
#include <list>

class AnalyzerOptions {
public:
  AnalyzerOptions();
  std::string toString();
  std::string getAnimationDirectoryName();
  void setAnimationDirectoryName(std::string);
  std::string getProgramName();
  void setInputFileName(std::string);
  std::string getInputFileName();
  void setProgramName(std::string);
  void setCfgOrdering(int ordering);
  int getCfgOrdering();
  void setCallStringLength(int length);
  int getCallStringLength();
  void setGdlFileName(std::string fileName);
  std::string getGdlFileName();
  void setOptionsInfo(std::string);
  std::string getOptionsInfo();
  void setGcLow(int perc);
  void setGcHigh(int perc);
  int getGcLow();
  int getGcHigh();

  void setStartBank(int start);
  int getStartBank();
  void setShareMin(int min);
  int getShareMin();
  void setShareNum(int num);
  int getShareNum();

  void animationGenerationOn();
  void animationGenerationOff();
  bool animationGeneration();

  void resultGenerationOn();
  void resultGenerationOff();
  bool resultGeneration();

  void helpMessageRequestedOn();
  void helpMessageRequestedOff();
  bool helpMessageRequested();

  void pagVerboseOn();
  void pagVerboseOff();
  bool pagVerbose();

  void preInfoOn();
  void preInfoOff();
  bool preInfo();

  void postInfoOn();
  void postInfoOff();
  bool postInfo();

  void statisticsOn();
  void statisticsOff();
  bool statistics();

  void gdlProcedureSubgraphsOn();
  void gdlProcedureSubgraphsOff();
  bool gdlProcedureSubgraphs();

  void analysisResultsTextOutputOn();
  void analysisResultsTextOutputOff();
  bool analysisResultsTextOutput();

  void analysisResultsSourceOutputOn();
  void analysisResultsSourceOutputOff();
  bool analysisResultsSourceOutput();

  void vivuOn(); // callstring length defines chop size
  void vivuOff();
  bool vivu();

  void setVivuLoopUnrolling(int start);
  int getVivuLoopUnrolling();

  void setVivu4MaxUnrolling(int start);
  int getVivu4MaxUnrolling();

  std::string getOptionsErrorMessage();
  bool optionsError();

  void clearCommandLine();
  void appendCommandLine(std::string);
  std::list<std::string> getCommandLineList();
  std::string getCommandLine();
  char** getCommandLineCarray();

  int getCommandLineNum(); // is computing according to appendCommandLine

  bool retFuncUsed();

private:
  void setCommandLineNum(int);
  void addCommandLineNum(int);

private:
  void optionsError(std::string message);

  std::string _animationDirectoryName;
  std::string _gdlFileName, _inputFileName, _programName;
  std::string _optionsErrorMessage;
  std::string _optionsInfo;
  int _cfgOrdering;
  int _callStringLength;
  int _startBank, _shareMin, _shareNum, _gcLow, _gcHigh;
  int _vivuLoopUnrolling, _vivu4MaxUnrolling;
  bool _animationGeneration, _resultGeneration, _helpMessageRequested, _pagVerbose;
  bool _postInfo, _preInfo, _gdlProcedureSubgraphs, _statistics, _analysisResultsTextOutput, _analysisResultsSourceOutput;
  bool _vivu;

  std::list<std::string> _commandLine;
  int _commandLineNum;
};

#endif
