#ifndef UNTYPED_FORTRAN_TRAVERSAL_H
#define UNTYPED_FORTRAN_TRAVERSAL_H

//-----------------------------------------------------------------------------------
// The SgUntyped::UntypedTraversal class is used to traverse SgUntypedNodes and
// convert them to regular SgNodes.
//-----------------------------------------------------------------------------------

#include "UntypedFortranConverter.h"

namespace Fortran {
namespace Untyped {

typedef SgScopeStatement*  InheritedAttribute;
typedef SgExpression*      SynthesizedAttribute;

class UntypedFortranTraversal : public SgTopDownBottomUpProcessing<InheritedAttribute, SynthesizedAttribute>, public FortranBuilderInterface
  {
    public:

      virtual InheritedAttribute   evaluateInheritedAttribute   (SgNode* n, InheritedAttribute inheritedAttribute );
      virtual SynthesizedAttribute evaluateSynthesizedAttribute (SgNode* n, InheritedAttribute inheritedAttribute
                                                                          , SynthesizedAttributesList childAttrs  );

      virtual ~UntypedFortranTraversal();

      UntypedFortranTraversal(SgSourceFile* sourceFile);


      std::string getCurrentFilename()
         {
             return p_source_file->get_sourceFileNameWithPath();
         }

    private:

      SgSourceFile* p_source_file;
      UntypedFortranConverter* pConverter;
  };

} // namespace Fortran
} // namespace Untyped

// endif for UNTYPED_FORTRAN_TRAVERSAL_H
#endif

