#ifndef UNTYPED_COBOL_TRAVERSAL_H
#define UNTYPED_COBOL_TRAVERSAL_H

//-----------------------------------------------------------------------------------
// The SgUntyped::UntypedTraversal class is used to traverse SgUntypedNodes and
// convert them to regular SgNodes.
//-----------------------------------------------------------------------------------

#include "UntypedCobolConverter.h"

namespace Cobol {
namespace Untyped {

typedef SgScopeStatement*  InheritedAttribute;
typedef SgExpression*      SynthesizedAttribute;

class UntypedTraversal : public SgTopDownBottomUpProcessing<InheritedAttribute, SynthesizedAttribute>, public CobolBuilderInterface
  {
    public:

      virtual InheritedAttribute   evaluateInheritedAttribute   (SgNode* n, InheritedAttribute inheritedAttribute );
      virtual SynthesizedAttribute evaluateSynthesizedAttribute (SgNode* n, InheritedAttribute inheritedAttribute
                                                                          , SynthesizedAttributesList childAttrs  );

      virtual ~UntypedTraversal();

      UntypedTraversal(SgSourceFile* sourceFile);


      std::string getCurrentFilename()
         {
             return p_source_file->get_sourceFileNameWithPath();
         }

    private:

      SgSourceFile* p_source_file;
      UntypedConverter* pConverter;
  };

} // namespace Cobol
} // namespace Untyped

// endif for UNTYPED_COBOL_TRAVERSAL_H
#endif

