#ifndef PREPROCESSAST_H
#define PREPROCESSAST_H

#include "util.h"

namespace PPA
{
  extern NodeContainer NodesToProcess;
  extern SgProject* project;

  struct TopBotTrack3 : AstTopDownBottomUpProcessing<Util::nodeType, Util::nodeType>
  {
    protected:
      Util::nodeType
      evaluateSynthesizedAttribute(SgNode* node, Util::nodeType inh, SynthesizedAttributesList synList) ROSE_OVERRIDE;

      Util::nodeType virtual evaluateInheritedAttribute(SgNode* node, Util::nodeType inh) ROSE_OVERRIDE;
  };

  void pushGlobalVarInitsToMain();

  void modifyForAndWhileLoops();

  void replacePlusMinusAssigns(SgBinaryOp* op, bool plus=true);
  void splitPlusMinusAssigns();

  void convertArrowThisToDot();

  void functionCallArgCastHandler();

  SgFunctionDeclaration* findDeclParent(SgNode* node);
  SgClassDeclaration* duplicateStruct(SgClassDeclaration* orig, SgName new_name, SgScopeStatement* scope, SgStatement* pos);
  void evictStructsFromFunctions();

  void castReturnExps();

  SgVariableDeclaration* pushToLocalVar(SgExpression* exp, SgScopeStatement* scope);
  SgVariableDeclaration* takeRefAndPushToLocalVar(SgExpression* exp);
  void simplifyDotExp(SgDotExp* dot);
  void simplifyArrowExp(SgArrowExp* arrow);
  void pushToLocalVarAndReplace(SgExpression* exp);

  void simplifyNodes();

  void preProcess(SgProject* proj);
}

#endif
