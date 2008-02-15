#if 0
Compiling the following code with ROSE:
namespace OpenAnalysis {
#pragma X
};
gives the following error:
Case declarationList.size() == 0: parent of newDeclarationStatement may
not be set properly (same for scope)
Error: base class SgNode::set_attribute(AstAttributeMechanism*) called on
SgPragma
identityTranslator: Cxx_Grammar.C:600: virtual void
SgNode::set_attribute(AstAttributeMechanism*): Assertion `false' failed.

Andreas
#endif


namespace OpenAnalysis
   {
#pragma X
   };
