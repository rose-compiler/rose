// Copyright 2007,2008,2009 Viktor Pavlu

// Author: Viktor Pavlu, 2007.

// FIXME: cmdline handling needs refactoring.
// it is awkward, that for a new cmdlne option
// if have to edit in two places
//   - Parser:  to detect the new options, ok
//   - Options: to have a matching --help text
// both should be in the same place, making it
// easier to introduce new options.

#ifndef SHAPEANALYZEROPTIONS_H
#define SHAPEANALYZEROPTIONS_H

#include <string>
#include <list>
#include <vector>

#include "AnalyzerOptions.h"

class ShapeAnalyzerOptions : public AnalyzerOptions {
public:
  virtual ~ShapeAnalyzerOptions() {;}

  ShapeAnalyzerOptions() {
    /* set default values */
    #define STRING_ATTR(attrname,defaultval) \
    set##attrname(defaultval);
    #define INT_ATTR(attrname,defaultval) \
    set##attrname(defaultval);
    #define INT_ATTR_NOSTUB(attrname,defaultval) \
    set##attrname(defaultval);
    #define BOOL_ATTR(attrname,defaultval) \
    attrname##defaultval();
    #include "attributes"
    #undef STRING_ATTR
    #undef INT_ATTR
    #undef INT_ATTR_NOSTUB
    #undef BOOL_ATTR
  }

  /* include getter/setter methods */
#define STRING_ATTR(attrname,defaultval) \
  std::string get##attrname() { return _##attrname; } \
  void set##attrname(std::string s) { _##attrname=s; }
#define INT_ATTR(attrname,defaultval) \
  int get##attrname() { return _##attrname; } \
  void set##attrname(int i) { _##attrname=i; }
#define INT_ATTR_NOSTUB(attrname,defaultval) \
  int get##attrname() { return _##attrname; }
#define BOOL_ATTR(attrname,defaultval) \
  void attrname##On()  { _##attrname=true;  } \
  void attrname##Off() { _##attrname=false; } \
  bool attrname() { return _##attrname; }
#include "attributes"
#undef STRING_ATTR
#undef INT_ATTR
#undef INT_ATTR_NOSTUB
#undef BOOL_ATTR

  virtual std::string getOptionsInfo() {
      return "\n Usage: "+getProgramName()+" [OPTION]... <filename1> <filename2> ... \n\n"
           + " "+getHelpOptionsText()
           + "\n"
             " Shape Analysis Options:\n" 
//             "   --output-alias                output the alias pairs for each statement on stdout\n"
//             "   --output-alias-annotation     add alias pairs to each statement in generated source file\n"
//             "\n"

#ifdef ANALYSIS_nnh99
             "   --nnh-graphs       set of compatible shapegraphs (NNH) in .gdl [default]\n"
             "   --no-nnh-graphs    no set of compatible shape graphs in .gdl\n"
             "   --srw-graphs       static shapegraph (SRW) in .gdl\n"
             "   --no-srw-graphs    no static shapegraph in .gdl [default]\n"
#else
             "   --nnh-graphs       set of compatible shapegraphs (NNH) in .gdl\n"
             "   --no-nnh-graphs    no set of compatible shape graphs in .gdl [default]\n"
             "   --srw-graphs       static shapegraph (SRW) in .gdl [default]\n"
             "   --no-srw-graphs    no static shapegraph in .gdl\n"
#endif
             "   --foldgraphs       fold all shape graphs\n"
             "   --no-foldgraphs    do not fold shape graphs\n"
//             "   --output-graph-statistics=<FILENAME>  write graph statistics to <FILENAME>\n"
             ;
  }

protected:
  /* add attributes for cmdline options */
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
