// DQ (6/27/2005):
/*! \brief Fixup SgFunctionDefinition (missing body).

    \implementation This was a problem compiling KULL, but I could not reproduce it in a test code!
 */
#ifndef __fixupFuncDefinition
#define  __fixupFuncDefinition
void fixupFunctionDefinitions ( SgNode* node );

class FixupFunctionDefinitions : public AstSimpleProcessing
   {
  // This class uses a traversal to test the values of the definingDeclaration and
  // firstNondefiningDeclaration pointers in each SgDeclarationStatement.  See code for
  // details, since both of these pointers are not always set.

     public:
          void visit ( SgNode* node );
   };

#endif
