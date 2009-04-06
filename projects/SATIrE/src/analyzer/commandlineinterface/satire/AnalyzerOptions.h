// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany, Adrian Prantl, Viktor Pavlu
// $Id: AnalyzerOptions.h,v 1.7 2008-06-02 11:26:01 gergo Exp $

// Author: Markus Schordan, 2006.

#ifndef ANALYZEROPTIONS_H
#define ANALYZEROPTIONS_H

#include <string>
#include <list>
#include <vector>

namespace SATIrE {
  class DataFlowAnalysis;
}

class AnalyzerOptions {
public:
  AnalyzerOptions();
  virtual ~AnalyzerOptions();

  std::string toString();

  enum Language {Language_C89,Language_C99,Language_CPP};
  void setLanguage(Language lang);
  Language getLanguage();

  /* declare getter/setter methods */
#define STRING_ATTR(attrname,defaultval) \
  std::string get##attrname(); \
  void set##attrname(std::string);

#define INT_ATTR(attrname,defaultval) \
  int get##attrname(); \
  void set##attrname(int);

#define INT_ATTR_NOSTUB(attrname,defaultval) \
  int get##attrname(); \
  void set##attrname(int);

#define BOOL_ATTR(attrname,defaultval) \
  void attrname##On(); \
  void attrname##Off(); \
  bool attrname();

#include "attributes"
#undef STRING_ATTR
#undef INT_ATTR
#undef INT_ATTR_NOSTUB
#undef BOOL_ATTR

  virtual std::string getOptionsInfo();
  void setOptionsInfo(std::string infoString);

  std::string getOptionsErrorMessage();
  void setOptionsErrorMessage(std::string);
  bool optionsError();

  void clearCommandLine();
  void appendCommandLine(std::string);
  void appendInputFile(std::string);
  std::vector<std::string> getCommandLineList();
  std::string getCommandLine();
  char** getCommandLineCarray();

  int getCommandLineNum(); // is computed according to appendCommandLine

  int getNumberOfInputFiles();

  bool retFuncUsed();

  void appendDataFlowAnalysis(SATIrE::DataFlowAnalysis *);
  const std::vector<SATIrE::DataFlowAnalysis *> &getDataFlowAnalyzers() const;

protected:
  void setCommandLineNum(int);
  void addCommandLineNum(int);

protected:
  void optionsError(std::string message);

  std::string _optionsErrorMessage;
  std::string _optionsInfo;
  
  std::vector<std::string> _commandLine;
  int _commandLineNum;
  Language _language;

  int _numberOfInputFiles;

#define STRING_ATTR(attrname,defaultval) \
  std::string _##attrname;
#define INT_ATTR(attrname,defaultval) \
  int _##attrname;
#define INT_ATTR_NOSTUB(attrname,defaultval) \
  int _##attrname;
#define BOOL_ATTR(attrname,defaultval) \
  bool _##attrname;
#include "attributes"
#undef STRING_ATTR
#undef INT_ATTR
#undef INT_ATTR_NOSTUB
#undef BOOL_ATTR

  std::vector<SATIrE::DataFlowAnalysis *> dataFlowAnalyzers;

};

#endif
