
#ifndef FIXUP_FORTRAN_ARRAY_VS_FUNCTION_REFERENCES_H
#define FIXUP_FORTRAN_ARRAY_VS_FUNCTION_REFERENCES_H

// DQ (11/24/2007):
/*! \brief Fixup Fortran array vs. function references.

    Fortran references to arrys and functions can not be easily disambiguated 
    until after the whole program has been seen.  Part of this is because
    function may not be defined until after they are called.  To support
    this analysis, we build a special IR node (SgUnknownArrayOrFunctionReference)
    when a function call can not be resolved to a function symbol in the symbol
    table.  There may in the future be more complex senarios where we build the
    SgUnknownArrayOrFunctionReference IR nodes and defer the evaluation of a
    reference (as an array or a function) until post-processing.

    \implementation This analysis is required only for Fortran support.
 */
void
fixupFortranReferences ( SgNode* node );

class FixupFortranReferences : public AstSimpleProcessing
   {
     public:
          void visit ( SgNode* node );
   };

// endif for FIXUP_FORTRAN_ARRAY_VS_FUNCTION_REFERENCES_H
#endif
