#ifndef UNTYPED_FORTRAN_TRAVERSAL_H
#define UNTYPED_FORTRAN_TRAVERSAL_H

//-----------------------------------------------------------------------------------
// The SgUntyped::UntypedTraversal class is used to traverse SgUntypedNodes and
// convert them to regular SgNodes.
//-----------------------------------------------------------------------------------

#include "UntypedTraversal.h"

namespace Untyped {

class UntypedFortranTraversal : public UntypedTraversal
  {
    public:

      UntypedFortranTraversal(SgSourceFile* sourceFile, UntypedConverter* converter);

      virtual InheritedAttribute   evaluateInheritedAttribute   (SgNode* n, InheritedAttribute inheritedAttribute );
      virtual SynthesizedAttribute evaluateSynthesizedAttribute (SgNode* n, InheritedAttribute inheritedAttribute
                                                                          , SynthesizedAttributesList childAttrs  );
  };

} // namespace Untyped

// endif for UNTYPED_FORTRAN_TRAVERSAL_H
#endif

