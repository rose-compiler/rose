// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany, Adrian Prantl, Viktor Pavlu
// $Id: AnalyzerOptions.h,v 1.5 2008-01-25 16:09:17 adrian Exp $

// Author: Markus Schordan, 2006.

#ifndef ANALYZEROPTIONS_H
#define ANALYZEROPTIONS_H

#include <string>
#include <list>
#include <vector>

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
  bool optionsError();

  void clearCommandLine();
  void appendCommandLine(std::string);
  std::vector<std::string> getCommandLineList();
  std::string getCommandLine();
  char** getCommandLineCarray();

  int getCommandLineNum(); // is computed according to appendCommandLine

  bool retFuncUsed();

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

};

#endif
