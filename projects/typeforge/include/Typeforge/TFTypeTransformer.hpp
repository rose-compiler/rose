#ifndef TYPE_TRANSFORMER_H
#define TYPE_TRANSFORMER_H

#include "Typeforge/TFTransformation.hpp"
#include "Typeforge/ToolConfig.hpp"
#include "Typeforge/Analysis.hpp"
#include "Typeforge/CommandList.hpp"

#include <string>

namespace Typeforge {

void makeAllCastsExplicit();
void annotateImplicitCastsAsComments();

class TFTypeTransformer {

// Old API

 public:
  void setTraceFlag(bool);
  bool getTraceFlag();
  static void trace(std::string s);

  void setConfigFile(std::string fileName);
  void writeConfig();

 private:
  static bool _traceFlag;
  int _totalNumChanges=0;
  ToolConfig* _outConfig = nullptr;
  std::string _writeConfig = "";

// Sorted API

  private:
    std::map<SgNode *, SgType *> transformations;

  public:
    int changeType(SgNode * node, SgType * type, bool base, bool listing);
    void addTransformation(SgNode * node, SgType * type, bool base);
    void addToActionList(SgNode * node, SgType* toType, bool base);

  public:
//  void analyze(CommandList const & commandList);
    void execute();

  public:
    static void generateCsvTransformationStats(std::string fileName, int numTypeReplace, TFTypeTransformer & tt, TFTransformation & tfTransformation);
    static void printTransformationStats(int numTypeReplace, TFTypeTransformer & tt, TFTransformation & tfTransformation);

  friend class TransformDirective;
  friend class TypeTransformDirective;
  friend class NameTransformDirective;
  friend class HandleTransformDirective;
};

extern TFTypeTransformer transformer;

}

#endif
