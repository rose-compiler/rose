#ifndef FIXUP_CXX_TO_USE_ALIASING_SYMBOLS_H
#define FIXUP_CXX_TO_USE_ALIASING_SYMBOLS_H

// DQ (4/14/2010):
/*! \brief Fixup C++ symbol tables to use aliasing symbols for using declarations.

    C++ using declarations and use directives shold have an effect on the
    symbol table for each scope that will cause a new symbol to be added
    to support the visability.  However within this design the the symbol
    should be an alias to the original symbol in the original scope.

    \implementation This analysis is required only for C++ support,
    and is similar to work that is currently supporting "use"
    declarations in Fortran 90.
 */
void fixupAstSymbolTablesToSupportAliasedSymbols (SgNode* node);

class FixupAstSymbolTablesToSupportAliasedSymbols : public AstSimpleProcessing
   {
     public:
          void visit ( SgNode* node );

       // This inserts an alias for each of the symbols in referencedScope into the symbol table of currentScope
          void injectSymbolsFromReferencedScopeIntoCurrentScope ( SgScopeStatement* referencedScope, SgScopeStatement* currentScope, SgAccessModifier::access_modifier_enum accessLevel);
   };

// endif for FIXUP_CXX_TO_USE_ALIASING_SYMBOLS_H
#endif
