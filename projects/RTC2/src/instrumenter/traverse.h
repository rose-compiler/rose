#include "util.h"

#define LOCK_KEY_INSERT
//#define JUST_TRAVERSE
//#define TRAVERSE_INPUT_FILE_TOP
//#define SYNTH_DEBUG

namespace Trav
{
  extern NodeContainer NodesToInstrument;
  extern NodeContainer DeclsToClone;
  extern SgProject* project;

  bool candidateFnCall(SgNode* node);
  bool candidateFnDecl(SgFunctionDeclaration* fn_decl);
  void CheckExprSanity(SgExpression* expr);
  bool NeedsToBeOverloaded(SgNode* node);
  bool isQualifyingLibCall(SgFunctionCallExp* fncall);


  struct TopBotTrack2 : AstTopDownBottomUpProcessing<Util::nodeType, Util::nodeType>
  {
    protected:
      Util::nodeType
      evaluateSynthesizedAttribute(SgNode* node, Util::nodeType inh, SynthesizedAttributesList synList) ROSE_OVERRIDE;

      Util::nodeType evaluateInheritedAttribute(SgNode* node, Util::nodeType inh) ROSE_OVERRIDE;
  };

  void traverse(SgProject* proj, SgFile* file);
}
