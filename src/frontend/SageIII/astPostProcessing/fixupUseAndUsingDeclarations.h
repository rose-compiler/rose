
#ifndef FIXUP_FORTRAN_USE_DECLARATIONS_H
#define FIXUP_FORTRAN_USE_DECLARATIONS_H

// DQ (9/26/2008):
/*! \brief Fixup Fortran Use declarations.

    Fortran "Use" declarations need to cause the symbol tables of
    the associated scope to be updated and new symbols added with
    external (non-local) attributes.

    \implementation This analysis is required only for Fortran support,
    but a similar analysis and transformation may be appropriate for C++
    to handle the "using" declaration and namespace aliasing.
 */
void
fixupFortranUseDeclarations ( SgNode* node );

class FixupFortranUseDeclarations : public AstSimpleProcessing
   {
     public:
          void visit ( SgNode* node );
   };

// endif for FIXUP_FORTRAN_USE_DECLARATIONS_H
#endif
