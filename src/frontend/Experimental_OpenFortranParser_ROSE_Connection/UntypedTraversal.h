#ifndef UNTYPED_TRAVERSAL_H
#define UNTYPED_TRAVERSAL_H

//-----------------------------------------------------------------------------------
// The SgUntyped::UntypedTraversal class is used to traverse SgUntypedNodes and
// convert them to regular SgNodes.
//-----------------------------------------------------------------------------------

namespace Fortran {
namespace Untyped {

typedef SgScopeStatement*  InheritedAttribute;
typedef SgExpression*      SynthesizedAttribute;

class UntypedTraversal : public SgTopDownBottomUpProcessing<InheritedAttribute, SynthesizedAttribute>
  {
    public:

      virtual InheritedAttribute   evaluateInheritedAttribute   (SgNode* n, InheritedAttribute inheritedAttribute );
      virtual SynthesizedAttribute evaluateSynthesizedAttribute (SgNode* n, InheritedAttribute inheritedAttribute
                                                                          , SynthesizedAttributesList childAttrs  );

      UntypedTraversal(SgSourceFile* sourceFile);

    private:

      SgSourceFile* p_source_file;
  };

} // namespace Fortran
} // namespace Untyped

// endif for UNTYPED_TRAVERSAL_H
#endif

