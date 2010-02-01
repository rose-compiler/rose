// DQ (6/27/2005):
/*! \brief Fixup SgTemplateDeclarations to not be marked as compiler generated.

\implementation Not sure how this happens, but no template declaration should be a 
      part of compiler generated code. (e.g. g++_HEADERS/hdrs1/bits/stl_pair.h at line 67 
      is marked as a compiler generated template)
 */
#ifndef __fixupTemplate
#define  __fixupTemplate
void fixupTemplateDeclarations ( SgNode* node );

class FixupTemplateDeclarations : public AstSimpleProcessing
   {
     public:
          void visit ( SgNode* node );
   };

#endif
