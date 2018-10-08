#ifndef UNTYPED_TRAVERSAL_H
#define UNTYPED_TRAVERSAL_H

//-----------------------------------------------------------------------------------
// The SgUntyped::UntypedTraversal class is used to traverse SgUntypedNodes and
// convert them to regular SgNodes.
//-----------------------------------------------------------------------------------

#include "UntypedConverter.h"

namespace Untyped {

typedef SgScopeStatement*  InheritedAttribute;
typedef SgLocatedNode*     SynthesizedAttribute;

class UntypedTraversal : public SgTopDownBottomUpProcessing<InheritedAttribute, SynthesizedAttribute>
  {
    public:

      UntypedTraversal(SgSourceFile* sourceFile, UntypedConverter* converter);

      virtual InheritedAttribute   evaluateInheritedAttribute   (SgNode* n, InheritedAttribute inheritedAttribute );
      virtual SynthesizedAttribute evaluateSynthesizedAttribute (SgNode* n, InheritedAttribute inheritedAttribute
                                                                          , SynthesizedAttributesList childAttrs  );
      std::string getCurrentFilename()
         {
             return p_source_file->get_sourceFileNameWithPath();
         }

    protected:

      SgSourceFile* p_source_file;
      UntypedConverter* pConverter;
  };

} // namespace Untyped

// endif for UNTYPED_TRAVERSAL_H
#endif

