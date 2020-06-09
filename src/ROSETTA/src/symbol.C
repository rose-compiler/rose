
#include "grammar.h"
#include "ROSETTA_macros.h"
#include "AstNodeClass.h"

// What should be the behavior of the default constructor for Grammar


void
Grammar::setUpSymbols ()
   {
  // This function sets up the symbol system for the grammar.  In this case it implements the
  // C++ grammar, but this will be modified to permit all grammars to contain elements of the
  // C++ grammar.  Modified grammars will add and subtract elements from this default C++ grammar.

  // DQ (12/27/2011): Added more support for template declaration details in the AST.
  // NEW_TERMINAL_MACRO ( VariableSymbol,       "VariableSymbol",       "VARIABLE_NAME" );
     NEW_TERMINAL_MACRO ( TemplateVariableSymbol, "TemplateVariableSymbol", "TEMPLATE_VARIABLE_NAME" );
     NEW_NONTERMINAL_MACRO ( VariableSymbol, TemplateVariableSymbol, "VariableSymbol", "VARIABLE_NAME", true);

  //
  // [DT] 5/3/2000 -- Added TypeSymbol (as an experiment).
  //
  // NEW_TERMINAL_MACRO ( TypeSymbol,           "TypeSymbol",           "TYPE_NAME" );
     NEW_TERMINAL_MACRO ( FunctionTypeSymbol,   "FunctionTypeSymbol",   "FUNCTYPE_NAME" );

  // DQ (12/26/2011): Added TemplateClassSymbol and changed ClassSymbol to be a non-AstNodeClass.
  // DQ (5/7/2004): ClassSymbol is not longer a nonterminal (change to be a AstNodeClass)
  // NEW_TERMINAL_MACRO ( ClassSymbol,          "ClassSymbol",          "CLASS_NAME" );
     NEW_TERMINAL_MACRO    ( TemplateClassSymbol,  "TemplateClassSymbol",  "TEMPLATE_CLASS_NAME" );
     NEW_NONTERMINAL_MACRO ( ClassSymbol, TemplateClassSymbol, "ClassSymbol", "CLASS_NAME", true);

  // [DT] 5/10/2000
     NEW_TERMINAL_MACRO ( TemplateSymbol,       "TemplateSymbol",       "TEMPLATE_NAME" );

#if 0
     //
     // [DT] 5/11/2000 -- Added TemplateInstantiationSymbol.
     //
     NEW_TERMINAL_MACRO ( TemplateInstantiationSymbol, 
                         "TemplateInstantiationSymbol", 
                         "TEMPLATE_INSTANTIATION_NAME" );
#endif

     NEW_TERMINAL_MACRO ( EnumSymbol,           "EnumSymbol",           "ENUM_NAME" );
     NEW_TERMINAL_MACRO ( EnumFieldSymbol,      "EnumFieldSymbol",      "FIELD_NAME" );

  // NEW_TERMINAL_MACRO ( TypedefSymbol,        "TypedefSymbol",        "TYPEDEF_NAME" );
     NEW_TERMINAL_MACRO    ( TemplateTypedefSymbol,  "TemplateTypedefSymbol",  "TEMPLATE_TYPEDEF_NAME" );
     NEW_NONTERMINAL_MACRO ( TypedefSymbol, TemplateTypedefSymbol, "TypedefSymbol", "TYPEDEF_NAME", true);

  // NEW_TERMINAL_MACRO ( MemberFunctionSymbol, "MemberFunctionSymbol", "MEMBER_FUNC_NAME" );
     NEW_TERMINAL_MACRO ( TemplateFunctionSymbol, "TemplateFunctionSymbol", "TEMPLATE_FUNC_NAME" );
     NEW_TERMINAL_MACRO ( TemplateMemberFunctionSymbol, "TemplateMemberFunctionSymbol", "TEMPLATE_MEMBER_FUNC_NAME" );

     NEW_TERMINAL_MACRO ( LabelSymbol,          "LabelSymbol",          "LABEL_NAME" );

  // DQ (9/9/2011): Added support for JavaLabelStatement (which has a different type of support for labels than C/C++ or Fortran).
     NEW_TERMINAL_MACRO ( JavaLabelSymbol,      "JavaLabelSymbol",      "JAVA_LABEL_NAME" );

  // [DT] 6/14/2000 -- Added DefaultSymbol.
     NEW_TERMINAL_MACRO ( DefaultSymbol,        "DefaultSymbol",        "DEFAULT_NAME" );

  // DQ (5/3/2004): Added namespace support
     NEW_TERMINAL_MACRO ( NamespaceSymbol,      "NamespaceSymbol",      "NAMESPACE_NAME" );

#if USE_FORTRAN_IR_NODES
  // DQ (2/2/2006): Support for Fortran IR nodes (contributed by Rice)
     NEW_TERMINAL_MACRO ( IntrinsicSymbol,      "IntrinsicSymbol",      "INTRINSIC_SYMBOL" );
     NEW_TERMINAL_MACRO ( ModuleSymbol,         "ModuleSymbol",         "MODULE_SYMBOL" );
     NEW_TERMINAL_MACRO ( InterfaceSymbol,      "InterfaceSymbol",      "INTERFACE_SYMBOL" );
     NEW_TERMINAL_MACRO ( CommonSymbol,         "CommonSymbol",         "COMMON_SYMBOL" );
#endif

  // DQ (10/11/2008): Move SgRenameSymbol to be derived from SgFunctionSymbol
  // DQ (10/10/2008): Added to support renamed representations of constructs that have non-SgAliasSymbols.
  // This is used in fortran interfaces where functions are renamed to interface names, this was
  // where we used to use the SgAliasSymbol, but we needed a different symbol so that chains of 
  // SgAliasSymbol IR nodes could be properly evaluated.
     NEW_TERMINAL_MACRO ( RenameSymbol,         "RenameSymbol",        "RENAME_SYMBOL");

  // DQ (12/27/2011): Added more support for template declaration details in the AST.
  // NEW_NONTERMINAL_MACRO ( FunctionSymbol,MemberFunctionSymbol | RenameSymbol,"FunctionSymbol","FUNCTION_NAME", true);
     NEW_NONTERMINAL_MACRO ( MemberFunctionSymbol,TemplateMemberFunctionSymbol,"MemberFunctionSymbol","MEMBER_FUNC_NAME", true);
     NEW_NONTERMINAL_MACRO ( FunctionSymbol, MemberFunctionSymbol | TemplateFunctionSymbol | RenameSymbol,"FunctionSymbol","FUNCTION_NAME", true);

#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
  // DQ (5/3/2010): Added symbol table support to the binary analysis within ROSE.  Values that
  // are addresses or references to data will have symbols in a function symbol table.  All other 
  // values are assumed to be literals and will not have associated symbols.
     NEW_TERMINAL_MACRO ( AsmBinaryAddressSymbol,  "AsmBinaryAddressSymbol",  "BINARY_ADDRESS_SYMBOL" );
     NEW_TERMINAL_MACRO ( AsmBinaryDataSymbol,     "AsmBinaryDataSymbol",     "BINARY_DATA_SYMBOL" );
#endif

  // DQ (9/26/2008): Added support for references to symbols to support: "use" declaration in F90, "using" declaration in C++, and "namespace aliasing" in C++.
     NEW_TERMINAL_MACRO ( AliasSymbol,          "AliasSymbol",         "ALIAS_SYMBOL" );

     NEW_TERMINAL_MACRO ( NonrealSymbol, "NonrealSymbol", "NONREAL_SYMBOL" );
     
  // PP (06/03/2020) Adding Ada support   
     NEW_TERMINAL_MACRO ( AdaPackageSymbol, "AdaPackageSymbol", "ADA_PACKAGE_SYMBOL" );
     NEW_TERMINAL_MACRO ( AdaTaskSymbol,    "AdaTaskSymbol", "ADA_TASK_SYMBOL" );

#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
     NEW_NONTERMINAL_MACRO (Symbol,
          VariableSymbol   | NonrealSymbol          | FunctionSymbol         | FunctionTypeSymbol | 
          ClassSymbol      | TemplateSymbol         | EnumSymbol             | EnumFieldSymbol    | 
          TypedefSymbol    | LabelSymbol            | DefaultSymbol          | NamespaceSymbol    |
          IntrinsicSymbol  | ModuleSymbol           | InterfaceSymbol        | CommonSymbol       | 
          AliasSymbol      | AsmBinaryAddressSymbol | AsmBinaryDataSymbol    | JavaLabelSymbol    | 
          AdaPackageSymbol | AdaTaskSymbol /* | RenameSymbol */, 
          "Symbol","SymbolTag", false);
#else
     NEW_NONTERMINAL_MACRO (Symbol,
          VariableSymbol   | NonrealSymbol          | FunctionSymbol         | FunctionTypeSymbol | 
          ClassSymbol      | TemplateSymbol         | EnumSymbol             | EnumFieldSymbol    | 
          TypedefSymbol    | LabelSymbol            | DefaultSymbol          | NamespaceSymbol    |
          IntrinsicSymbol  | ModuleSymbol           | InterfaceSymbol        | CommonSymbol       | 
          AliasSymbol      |                                                   JavaLabelSymbol    |
          AdaPackageSymbol | AdaTaskSymbol /* | RenameSymbol */,
          "Symbol","SymbolTag", false);
#endif

  // ***********************************************************************
  // ***********************************************************************
  //                       Header Code Declaration
  // ***********************************************************************
  // ***********************************************************************

  // Symbol.setSubTreeFunctionPrototype ( "HEADER", "../Grammar/sageCommon.code" );
  // Symbol.excludeFunctionPrototype    ( "HEADER", "../Grammar/sageCommon.code" );

     Symbol.setFunctionPrototype ( "HEADER", "../Grammar/Symbol.code" );

     Symbol.setSubTreeFunctionPrototype ( "HEADER_GET_NAME", "../Grammar/Symbol.code" );
     Symbol.excludeFunctionPrototype    ( "HEADER_GET_NAME", "../Grammar/Symbol.code" );

     Symbol.setSubTreeFunctionPrototype ( "HEADER_GET_TYPE", "../Grammar/Symbol.code" );
     Symbol.excludeFunctionPrototype    ( "HEADER_GET_TYPE", "../Grammar/Symbol.code" );
     Symbol.setAutomaticGenerationOfCopyFunction(false);

#if 1
  // DQ (11/1/2015): Build the access functions, but don't let the set_* access function set the "p_isModified" flag.
  // DQ (1/24/2006): Added attribute via ROSETTA (changed to pointer to AstAttributeMechanism)
  // Modified implementation to only be at specific IR nodes.  Beata appears to use attributes
  // on SgTypes (even though they are shared; doesn't appear to be a problem for them).
  // Symbol.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
  //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     Symbol.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
            NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);
     Symbol.setFunctionPrototype      ( "HEADER_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
     Symbol.setFunctionSource         ( "SOURCE_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
#endif

  // Skip building a parse function for this AstNodeClass/nonterminal of the Grammar
     if (isRootGrammar() == false)
        {
          Symbol.excludeFunctionPrototype ( "HEADER_PARSER", "../Grammar/Node.code" );
          Symbol.excludeFunctionSource   ( "SOURCE_PARSER", "../Grammar/parserSourceCode.macro" );
        }
     
  // VariableSymbol.excludeFunctionPrototype ( "HEADER_VARIABLE_SYMBOL",      "../Grammar/Symbol.code" );
  // VariableSymbol.excludeFunctionPrototype ( "HEADER_GET_NAME",             "../Grammar/Symbol.code" );
  // VariableSymbol.setFunctionPrototype     ( "HEADER_GET_NAME_DECLARATION", "../Grammar/Symbol.code" );
     VariableSymbol.setDataPrototype   ( "SgInitializedName*", "declaration", "= NULL",
                                         CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // TypeSymbol.setDataPrototype   ( "SgInitializedName*", "declaration", "= NULL");

     FunctionSymbol.setDataPrototype   ( "SgFunctionDeclaration*", "declaration", "= NULL",
                                         CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     FunctionTypeSymbol.excludeFunctionPrototype ( "HEADER_GET_NAME", "../Grammar/Symbol.code" );
     FunctionTypeSymbol.excludeFunctionPrototype ( "HEADER_GET_TYPE", "../Grammar/Symbol.code" );
  // We do not traverse the following data member for the moment!
     FunctionTypeSymbol.setDataPrototype   ( "SgName" , "name", "= \"\"",
                    CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     FunctionTypeSymbol.setDataPrototype   ( "SgType*", "type", "= NULL",
                                             CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (9/25/2004): This function should be modified (customized) to return p_declaration->firstNondefiningDeclaration() 
  // instead of p_declaration!  This way only nondefining declarations are shared (though all declarations share a 
  // single definition).  The symbol can contain either the defining or nondefining declaration, since
  // the logic to return the correct declaration is in the SgClassSymbol.  Actually is we made sure that
  // only the first non-defining declaration were stored in the SgClass Symbol then we would not have to modifiy
  // the SgClassSymbol's implementation of get_declaration. It would always return the correct non-defining declaration!
     ClassSymbol.setDataPrototype    ( "SgClassDeclaration*",   "declaration", "= NULL",
                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  // ClassSymbol.setDataPrototype    ( "SgClassDeclaration*",   "declaration", "= NULL",
  //               CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL);
     TemplateSymbol.setDataPrototype ( "SgTemplateDeclaration*","declaration", "= NULL",
                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE); // [DT] 5/10/2000

  // TV (04/11/2018): Introducing representation for non-real "stuff" (template parameters)
     NonrealSymbol.setDataPrototype ( "SgNonrealDecl*", "declaration", "= NULL",
                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AdaPackageSymbol.setDataPrototype    ( "SgDeclarationStatement*",   "declaration", "= NULL",
                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AdaTaskSymbol.setDataPrototype    ( "SgDeclarationStatement*",   "declaration", "= NULL",
                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);


  // DQ (2/29/2004): Header file support code for template declaration support
  // TemplateInstantiationSymbol.setFunctionPrototype( "HEADER_TEMPLATE_INSTANTIATION_DECLARATION", "../Grammar/Symbol.code" );
  // TemplateInstantiationSymbol.setDataPrototype ( "SgTemplateInstantiationDecl*","declaration", "= NULL");

  // DQ (2/29/2004): Removed in favor of putting name in declaration instead of symbol
  // DQ (2/29/2004): Support for templates (save the original name of the template not just the mangled class name)
  // TemplateInstantiationSymbol.setDataPrototype ( "SgName","templateName", "= \"\"",
  //               CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     EnumSymbol.setDataPrototype     ( "SgEnumDeclaration*",    "declaration", "= NULL",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     EnumFieldSymbol.setDataPrototype( "SgInitializedName*",    "declaration", "= NULL",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     TypedefSymbol.setDataPrototype  ( "SgTypedefDeclaration*", "declaration", "= NULL",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     MemberFunctionSymbol.setFunctionPrototype ( "HEADER_DECLARATION", "../Grammar/Symbol.code" );
  // TemplateMemberFunctionSymbol.setFunctionPrototype ( "HEADER_DECLARATION", "../Grammar/Symbol.code" );

  // This is depricated (it is not used in Fortran, but still used in C/C++).
     LabelSymbol.setFunctionPrototype     ( "HEADER_LABEL_SYMBOL", "../Grammar/Symbol.code" );
     LabelSymbol.setDataPrototype         ( "SgLabelStatement*", "declaration", "= NULL",
                                            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     LabelSymbol.setDataPrototype         ( "SgStatement*", "fortran_statement", "= NULL",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     LabelSymbol.setDataPrototype         ( "SgInitializedName*", "fortran_alternate_return_parameter", "= NULL",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     LabelSymbol.setDataPrototype         ( "int", "numeric_label_value", "= -1",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     LabelSymbol.setDataPrototype         ( "SgLabelSymbol::label_type_enum", "label_type", "= SgLabelSymbol::e_unknown_label_type",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // LabelSymbol.setDataPrototype         ( "bool", "elseLabel", "= false",
  //               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // LabelSymbol.setDataPrototype         ( "bool", "endLabel", "= false",
  //               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // LabelSymbol.excludeFunctionPrototype ( "HEADER_GET_TYPE",       "../Grammar/Symbol.code" );
  // LabelSymbol.setFunctionPrototype     ( "HEADER_EMPTY_GET_TYPE", "../Grammar/Symbol.code" );
  // LabelSymbol.setDataPrototype         ( "SgLabelStatement*", "declaration", "= NULL");


  // DQ (9/9/2011): Added support for JavaLabelStatement (which has a different type of support for labels than C/C++ or Fortran).
     JavaLabelSymbol.setFunctionPrototype     ( "HEADER_JAVA_LABEL_SYMBOL", "../Grammar/Symbol.code" );
     JavaLabelSymbol.setDataPrototype         ( "SgJavaLabelStatement*", "declaration", "= NULL",
                                            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DefaultSymbol.excludeFunctionPrototype ( "HEADER_GET_NAME", "../Grammar/Symbol.code" );
     DefaultSymbol.excludeFunctionPrototype ( "HEADER_GET_TYPE", "../Grammar/Symbol.code" );

  // We do not traverse the following data member for the moment!
  // DQ (12/23/2005): Removed this SgName since it does not appear to be used or required!
  // DefaultSymbol.setDataPrototype       ( "SgName", "name", "= SgdefaultName",
  //               CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     DefaultSymbol.setDataPrototype       ( "SgType*", "type", "= NULL",
                                            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (8/30/2009): Added support for namespace alias to the NamespaceSymbol.
  // DQ (12/23/2005): This has been here for a long time, but in trying to remove unused SgName 
  // objects I have realized that we need the SgName here to support symbols for namespace 
  // aliases (SgNamespaceAliasDeclaration).
     NamespaceSymbol.setFunctionPrototype  ( "HEADER_NAMESPACE_SYMBOL", "../Grammar/Symbol.code" );
     NamespaceSymbol.setDataPrototype   ( "SgName" , "name", "= \"\"",
                   CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     NamespaceSymbol.setDataPrototype     ( "SgNamespaceDeclarationStatement*", "declaration", "= NULL",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     NamespaceSymbol.setDataPrototype     ( "SgNamespaceAliasDeclarationStatement*", "aliasDeclaration", "= NULL",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     NamespaceSymbol.setDataPrototype     ( "bool", "isAlias", "= false",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

#if USE_FORTRAN_IR_NODES
  // DQ (3/19/2007): Support for Fortran IR nodes (contributed by Rice)
     IntrinsicSymbol.setFunctionPrototype ( "HEADER_GET_NAME", "../Grammar/Symbol.code" );
     ModuleSymbol.setFunctionPrototype    ( "HEADER_GET_NAME", "../Grammar/Symbol.code" );
     InterfaceSymbol.setFunctionPrototype ( "HEADER_GET_NAME", "../Grammar/Symbol.code" );
     CommonSymbol.setFunctionPrototype    ( "HEADER_GET_NAME", "../Grammar/Symbol.code" );

     IntrinsicSymbol.setFunctionPrototype ( "HEADER_GET_TYPE", "../Grammar/Symbol.code" );
     ModuleSymbol.setFunctionPrototype    ( "HEADER_GET_TYPE", "../Grammar/Symbol.code" );
     InterfaceSymbol.setFunctionPrototype ( "HEADER_GET_TYPE", "../Grammar/Symbol.code" );
     CommonSymbol.setFunctionPrototype    ( "HEADER_GET_TYPE", "../Grammar/Symbol.code" );

  // DQ (3/20/2007): Added data members based on Rice work by Gina).
     IntrinsicSymbol.setDataPrototype   ( "SgInitializedName*", "declaration", "= NULL",
                                         CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     InterfaceSymbol.setDataPrototype   ( "SgInterfaceStatement*", "declaration", "= NULL",
                                         CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     ModuleSymbol.setDataPrototype      ( "SgModuleStatement*", "declaration", "= NULL",
                                         CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     CommonSymbol.setDataPrototype      ( "SgInitializedName*", "declaration", "= NULL",
                                         CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

     AliasSymbol.setFunctionPrototype   ( "HEADER_ALIAS_SYMBOL", "../Grammar/Symbol.code" );
     AliasSymbol.setDataPrototype       ( "SgSymbol*", "alias", "= NULL",
                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // Note that the "use" statment can rename symbols referenced across scopes and this is different from the
  // renaming of symbols (constructs that have symbols) within a single scope (use a SgRenameSymbol for that case).
     AliasSymbol.setDataPrototype       ( "bool", "isRenamed", "= false",
                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AliasSymbol.setDataPrototype       ( "SgName", "new_name", "= \"\"",
                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (7/11/2014): Added to support references to the associated using declarations and using directives 
  // that caused the sgAliasSymbol symbol to be built.  This is critical to support for name qualification
  // that is different before and after a using declaration; but which forces the sgAliasSymbol to be 
  // generated only once within the AST (before uparsing the associated using declaration of using directive
  // that caused the generation of the SgAliasSymbol in the symbol tabel for the associated scope).
  // Note that test2014_90.C is a simple example of why this is important.
     AliasSymbol.setDataPrototype("SgNodePtrList", "causal_nodes", "",
                   NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     RenameSymbol.setFunctionPrototype   ( "HEADER_RENAME_SYMBOL", "../Grammar/Symbol.code" );
     RenameSymbol.setDataPrototype       ( "SgSymbol*", "original_symbol", "= NULL",
                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     RenameSymbol.setDataPrototype       ( "SgName", "new_name", "= \"\"",
                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (5/3/2010): Added symbol table support to the binary analysis within ROSE.  Values that
  // are addresses or references to data will have symbols in a function symbol table.  All other 
  // values are assumed to be literals and will not have associated symbols.
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
     AsmBinaryAddressSymbol.setFunctionPrototype ( "HEADER_ASM_BINARY_ADDRESS_SYMBOL", "../Grammar/Symbol.code" );
     AsmBinaryAddressSymbol.setDataPrototype     ( "SgName", "address_name", "= \"\"",
                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmBinaryAddressSymbol.setDataPrototype     ( "SgAsmInstruction*", "address", "= NULL",
                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmBinaryDataSymbol.setFunctionPrototype ( "HEADER_ASM_BINARY_DATA_SYMBOL",    "../Grammar/Symbol.code" );
     AsmBinaryDataSymbol.setDataPrototype     ( "SgName", "variable_name", "= \"\"",
                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmBinaryDataSymbol.setDataPrototype     ( "SgAsmInstruction*", "address", "= NULL",
                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

  // ***********************************************************************
  // ***********************************************************************
  //                       Source Code Definition
  // ***********************************************************************
  // ***********************************************************************

  // Symbol.setSubTreeFunctionSource ( "SOURCE", "../Grammar/sageCommon.code" );
  // Symbol.excludeFunctionSource    ( "SOURCE", "../Grammar/sageCommon.code" );
     Symbol.setFunctionSource        ( "SOURCE", "../Grammar/Symbol.code" );

     VariableSymbol.setFunctionSource       ( "SOURCE_GET_TYPE", "../Grammar/Symbol.code" );
     TemplateVariableSymbol.setFunctionSource( "SOURCE_GET_TYPE", "../Grammar/Symbol.code" );

#if USE_FORTRAN_IR_NODES
     IntrinsicSymbol.setFunctionSource      ( "SOURCE_GET_TYPE", "../Grammar/Symbol.code" );
  // ModuleSymbol.setFunctionSource         ( "SOURCE_MODULE_SYMBOL", "../Grammar/Symbol.code" );
     ModuleSymbol.setFunctionSource         ( "SOURCE_EMPTY_GET_TYPE", "../Grammar/Symbol.code" );
     InterfaceSymbol.setFunctionSource      ( "SOURCE_EMPTY_GET_TYPE", "../Grammar/Symbol.code" );
     CommonSymbol.setFunctionSource         ( "SOURCE_GET_TYPE", "../Grammar/Symbol.code" );
#endif

  // TypeSymbol.setFunctionSource           ( "SOURCE_GET_TYPE", "../Grammar/Symbol.code" );
     FunctionSymbol.setFunctionSource       ( "SOURCE_GET_TYPE", "../Grammar/Symbol.code" );
     TemplateFunctionSymbol.setFunctionSource( "SOURCE_GET_TYPE", "../Grammar/Symbol.code" );
     ClassSymbol.setFunctionSource          ( "SOURCE_GET_TYPE", "../Grammar/Symbol.code" );
     TemplateClassSymbol.setFunctionSource  ( "SOURCE_GET_TYPE", "../Grammar/Symbol.code" );
     NonrealSymbol.setFunctionSource        ( "SOURCE_GET_TYPE", "../Grammar/Symbol.code" );

  // DQ (12/15/2011): I think we have to support types for templates.
     TemplateSymbol.setFunctionSource     ( "SOURCE_GET_TYPE", "../Grammar/Symbol.code" );       // [DT] 5/10/2000
  // TemplateSymbol.setFunctionSource       ( "SOURCE_EMPTY_GET_TYPE", "../Grammar/Symbol.code" ); // [DT] 5/10/2000

  // TemplateInstantiationSymbol.setFunctionSource( "SOURCE_GET_TYPE", "../Grammar/Symbol.code" ); // [DT] 5/11/2000
     EnumSymbol.setFunctionSource           ( "SOURCE_GET_TYPE", "../Grammar/Symbol.code" );
     EnumFieldSymbol.setFunctionSource      ( "SOURCE_GET_TYPE", "../Grammar/Symbol.code" );

     TypedefSymbol.setFunctionSource        ( "SOURCE_GET_TYPE", "../Grammar/Symbol.code" );

  // DQ (11/4/2014): Added support for templated typedef (C++11 feature).
     TemplateTypedefSymbol.setFunctionSource( "SOURCE_GET_TYPE", "../Grammar/Symbol.code" );

     MemberFunctionSymbol.setFunctionSource ( "SOURCE_GET_TYPE", "../Grammar/Symbol.code" );
     TemplateMemberFunctionSymbol.setFunctionSource ( "SOURCE_GET_TYPE", "../Grammar/Symbol.code" );

  // DQ (12/9/2007): Handle the case of labels as a special case.
  // LabelSymbol.setFunctionSource          ( "SOURCE_EMPTY_GET_TYPE", "../Grammar/Symbol.code" );
  // LabelSymbol.setFunctionSource          ( "SOURCE_SHORT_GET_NAME", "../Grammar/Symbol.code" );
     LabelSymbol.setFunctionSource          ( "SOURCE_LABEL_SYMBOL", "../Grammar/Symbol.code" );

  // DQ (9/9/2011): Added support for JavaLabelStatement (which has a different type of support for labels than C/C++ or Fortran).
     JavaLabelSymbol.setFunctionSource      ( "SOURCE_JAVA_LABEL_SYMBOL", "../Grammar/Symbol.code" );

  // There is really no difference between the long and short versions (just debugging code, I think)
     VariableSymbol.setFunctionSource       ( "SOURCE_LONG_GET_NAME", "../Grammar/Symbol.code" );
     TemplateVariableSymbol.setFunctionSource ( "SOURCE_LONG_GET_NAME", "../Grammar/Symbol.code" );

  // TypeSymbol.setFunctionSource           ( "SOURCE_LONG_GET_NAME",  "../Grammar/Symbol.code" );
     FunctionSymbol.setFunctionSource       ( "SOURCE_SHORT_GET_NAME", "../Grammar/Symbol.code" );
     TemplateFunctionSymbol.setFunctionSource       ( "SOURCE_SHORT_GET_NAME", "../Grammar/Symbol.code" );
     ClassSymbol.setFunctionSource          ( "SOURCE_SHORT_GET_NAME", "../Grammar/Symbol.code" );
     TemplateClassSymbol.setFunctionSource  ( "SOURCE_SHORT_GET_NAME", "../Grammar/Symbol.code" );
     NonrealSymbol.setFunctionSource        ( "SOURCE_SHORT_GET_NAME", "../Grammar/Symbol.code" );

  // DQ (3/11/2004): Force name of templateSymbol to be the template string (at least until we can get 
  // to the point in the processing were we have access to the realy template name within EDG (then it 
  // might be good to reset the name from the whole template text string.
  // TemplateSymbol.setFunctionSource       ( "SOURCE_SHORT_EMPTY_GET_NAME", "../Grammar/Symbol.code" );
     TemplateSymbol.setFunctionSource       ( "SOURCE_SHORT_GET_NAME", "../Grammar/Symbol.code" );
  // TemplateInstantiationSymbol.setFunctionSource( "SOURCE_SHORT_GET_NAME", "../Grammar/Symbol.code" );

     EnumSymbol.setFunctionSource           ( "SOURCE_SHORT_GET_NAME", "../Grammar/Symbol.code" );
     EnumFieldSymbol.setFunctionSource      ( "SOURCE_SHORT_GET_NAME", "../Grammar/Symbol.code" );
     TypedefSymbol.setFunctionSource        ( "SOURCE_SHORT_GET_NAME", "../Grammar/Symbol.code" );

  // DQ (11/4/2014): Added support for templated typedef (C++11 feature).
     TemplateTypedefSymbol.setFunctionSource( "SOURCE_SHORT_GET_NAME", "../Grammar/Symbol.code" );

     MemberFunctionSymbol.setFunctionSource ( "SOURCE_SHORT_GET_NAME", "../Grammar/Symbol.code" );
     TemplateMemberFunctionSymbol.setFunctionSource ( "SOURCE_SHORT_GET_NAME", "../Grammar/Symbol.code" );

     MemberFunctionSymbol.setFunctionSource ( "SOURCE_DECLARATION", "../Grammar/Symbol.code" );
  // TemplateMemberFunctionSymbol.setFunctionSource ( "SOURCE_DECLARATION", "../Grammar/Symbol.code" );

  // DQ (2/29/2004): Source code for template declaration support
  // TemplateInstantiationSymbol.setFunctionSource( "SOURCE_TEMPLATE_INSTANTIATION_DECLARATION", "../Grammar/Symbol.code" );

  // DQ (5/3/2004): Added support for namespaces
     NamespaceSymbol.setFunctionSource      ( "SOURCE_NAMESPACE_SYMBOL", "../Grammar/Symbol.code" );
     NamespaceSymbol.setFunctionSource      ( "SOURCE_EMPTY_GET_TYPE", "../Grammar/Symbol.code" );
  // We need a special version of the get_symbol_basis() function
  // NamespaceSymbol.setFunctionSource      ( "SOURCE_SHORT_GET_NAME", "../Grammar/Symbol.code" );
     
  // PP (06/03/20)+: supporting Ada   
     AdaPackageSymbol.setFunctionSource     ( "SOURCE_ADA_PACKAGE_SYMBOL", "../Grammar/Symbol.code" );     
     AdaPackageSymbol.setFunctionSource     ( "SOURCE_EMPTY_GET_TYPE", "../Grammar/Symbol.code" );

     AdaTaskSymbol.setFunctionSource        ( "SOURCE_ADA_TASK_SYMBOL", "../Grammar/Symbol.code" );     
     AdaTaskSymbol.setFunctionSource        ( "SOURCE_EMPTY_GET_TYPE", "../Grammar/Symbol.code" );

  // DQ (12/23/2005): Removed SgName object and so we now need to build the get_name() member function
     DefaultSymbol.setFunctionSource        ( "SOURCE_SHORT_DEFAULT_GET_NAME", "../Grammar/Symbol.code" );


#if USE_FORTRAN_IR_NODES
  // DQ (3/19/2007): Support for Fortran IR nodes (contributed by Rice)
     ModuleSymbol.setFunctionSource         ( "SOURCE_MODULE_GET_NAME", "../Grammar/Symbol.code" );
     IntrinsicSymbol.setFunctionSource      ( "SOURCE_INTRIN_GET_NAME", "../Grammar/Symbol.code" );
     InterfaceSymbol.setFunctionSource      ( "SOURCE_INTERFACE_GET_NAME", "../Grammar/Symbol.code" );
     CommonSymbol.setFunctionSource         ( "SOURCE_COMMON_GET_NAME", "../Grammar/Symbol.code" );

  // IntrinsicSymbol.setFunctionSource ( "SOURCE_SHORT_EMPTY_GET_NAME", "../Grammar/Symbol.code" );
  // ModuleSymbol.setFunctionSource    ( "SOURCE_SHORT_EMPTY_GET_NAME", "../Grammar/Symbol.code" );
  // InterfaceSymbol.setFunctionSource ( "SOURCE_SHORT_EMPTY_GET_NAME", "../Grammar/Symbol.code" );
  // CommonSymbol.setFunctionSource    ( "SOURCE_SHORT_EMPTY_GET_NAME", "../Grammar/Symbol.code" );

  // IntrinsicSymbol.setFunctionSource ( "SOURCE_EMPTY_GET_TYPE", "../Grammar/Symbol.code" );
  // ModuleSymbol.setFunctionSource    ( "SOURCE_EMPTY_GET_TYPE", "../Grammar/Symbol.code" );
  // InterfaceSymbol.setFunctionSource ( "SOURCE_EMPTY_GET_TYPE", "../Grammar/Symbol.code" );
  // CommonSymbol.setFunctionSource    ( "SOURCE_EMPTY_GET_TYPE", "../Grammar/Symbol.code" );
#endif

     AliasSymbol.setFunctionSource          ( "SOURCE_ALIAS_SYMBOL", "../Grammar/Symbol.code" );

     RenameSymbol.setFunctionSource         ( "SOURCE_RENAME_SYMBOL", "../Grammar/Symbol.code" );

#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
  // DQ (5/3/2010): Added symbol table support to the binary analysis within ROSE.  Values that
  // are addresses or references to data will have symbols in a function symbol table.  All other 
  // values are assumed to be literals and will not have associated symbols.
     AsmBinaryAddressSymbol.setFunctionSource ( "SOURCE_ASM_BINARY_ADDRESS_SYMBOL", "../Grammar/Symbol.code" );
     AsmBinaryDataSymbol.setFunctionSource    ( "SOURCE_ASM_BINARY_DATA_SYMBOL",    "../Grammar/Symbol.code" );
#endif
   }

