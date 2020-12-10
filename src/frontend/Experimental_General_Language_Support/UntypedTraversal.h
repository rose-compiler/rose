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

   // For variable declarations with an anonymous type
      bool p_has_base_type_name;
      std::string p_base_type_name;
      std::string p_var_name;
      SgInitializer* p_var_initializer;
  };

} // namespace Untyped

// endif for UNTYPED_TRAVERSAL_H
#endif

