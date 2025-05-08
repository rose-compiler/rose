#include "sage3basic.h"
#include "tokenStreamMapping.h"
#include "previousAndNextNode.h"

using namespace std;

void
detectMacroExpansionsToBeUnparsedAsAstTransformations(SgSourceFile* /*sourceFile*/)
   {
  // Note that we have the macroExpansions and there associated statements, but if these are replaced 
  // by other statements then we have to have a way to detect that they were transformed by removal 
  // where they were not transformed by being modified.

  // This is now handled in the SageInterface::resetInternalMapsForTargetStatement() function.
   }
