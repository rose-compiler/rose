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
       // DQ (7/23/2011): Linkup namespaces so that we can build alias sysmbol from previously matching namespaces into subsequent matching namespaces.
          std::map<SgName,std::vector<SgNamespaceDefinitionStatement*> > namespaceMap;

          void visit ( SgNode* node );

       // DQ (7/12/2014): Added support to record the causalDeclaration (the declaration that is causing the SgAliasSymbol to be introduced).
       // This is important for the name qualification to be generated correctly before and after the causalDeclaration has been seen.
       // It is also helpful in knowing which SgAliassymbols to remove from the AST when a SgUsingDirective or SgUsingDeclaration is 
       // removed from the AST.  This functionality has not yet been implemented.
       // This inserts an alias for each of the symbols in referencedScope into the symbol table of currentScope
       // static void injectSymbolsFromReferencedScopeIntoCurrentScope ( SgScopeStatement* referencedScope, SgScopeStatement* currentScope, SgAccessModifier::access_modifier_enum accessLevel);
          static void injectSymbolsFromReferencedScopeIntoCurrentScope ( SgScopeStatement* referencedScope, SgScopeStatement* currentScope, SgNode* causalNode, SgAccessModifier::access_modifier_enum accessLevel);

       // DQ (3/24/2016): Adding Robb's meageage mechanism (data member and function).
          static Sawyer::Message::Facility mlog;
          static void initDiagnostics();

       // DQ (1/21/2019): Added to support Cxx_tests/test2019_21.C (symbol aliasing of data member in private base class of base class of derived class).
          static bool isDefinedThroughPrivateBaseClass ( SgClassDeclaration* classDeclaration, SgSymbol* symbol );
   };

// endif for FIXUP_CXX_TO_USE_ALIASING_SYMBOLS_H
#endif
