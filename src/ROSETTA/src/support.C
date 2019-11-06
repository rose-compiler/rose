// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
#include "rose_config.h"

#include "grammar.h"

#include "ROSETTA_macros.h"
#include "AstNodeClass.h"
//#include "OmpAttribute.h"

// What should be the behavior of the default constructor for Grammar

void
Grammar::setUpSupport ()
   {
  // This function sets up the symbol system for the grammar.  In this case it implements the
  // C++ grammar, but this will be modified to permit all grammars to contain elements of the
  // C++ grammar.  Modified grammars will add and subtract elements from this default C++ grammar.

  // We might want a mechanism to mark these as supporting terminals and non-terminals
  // that are to be used for every grammar.  We might do this through the definition of
  // separate grammar just for support and which other grammars would use or be derived
  // from (more formally, a base class for all grammars).

  // Since these represent generic support we should remove the "Sg" (though
  // we clearly build them from ideas laid out within SAGE++ and SAGE 2.
     NEW_TERMINAL_MACRO (Name, "Name", "NameTag" );
     NEW_TERMINAL_MACRO (SymbolTable, "SymbolTable", "SymbolTableTag" );

     NEW_TERMINAL_MACRO (Pragma, "Pragma", "PragmaTag" );
  // Grammar::AstNodeClass  Attribute( "Attribute", sageGrammar, "Attribute" );
  // Grammar::AstNodeClass  BitAttribute( "BitAttribute", sageGrammar, "BitAttribute" );

#if 0
  // const_volatile specifiers (const, volatile)
  // storage specifiers (auto, static, register, extern, mutable)
  // access specifiers (private, protected, public)
  // function specifiers (inline, virtual, pure-specifier, explicit)
  // UPC access specifiers (unspecified, shared, strict, relaxed)
  // special_function specifiers (constructor, destructor, conversion, operator)
  // declaration specifiers (storage specifiers, type specifiers, function specifiers,
  //           friend, typedef, export, exception-specifier)
  // type specifier (simple-type-specifier (not in SAGE III, char, bool, int, float, etc.),
  //           class specifier, enum specifier, elaborated-type-specifier, cv-qualifier, restrict)
  // elaborated-type-specifier (class, struct, union, enum, typename, typename template)
  // linkage-specification (extern, extern "C", extern "C++")
  // base-class-specifier (virtual, access specifiers)
#endif

  // DQ (4/6/2004): Deprecated support for ModifierNodes will be replaced by two separate classes
  // (TypeModifier, StorageModifier, AccessModifier, ConstVolatileModifier, FunctionModifier, ClassModifier)
  // Original SAGE modfifier support (ModifierNodes)
     NEW_TERMINAL_MACRO (ModifierNodes          ,"ModifierNodes"          , "ModifierNodesTag" );

  // DQ (4/19/2004): New modifiers (C++ grammar calls them specifiers)
  // ConstVolatileModifier, StorageModifier, AccessModifier, FunctionModifier,
  // UPC_AccessModifier, SpecialFunctionModifier, DeclarationModifier, TypeModifier,
  // ElaboratedTypeModifier, LinkageModifier, BaseClassModifier
     NEW_TERMINAL_MACRO (ConstVolatileModifier  ,"ConstVolatileModifier"  , "ConstVolatileModifierTag" );
     NEW_TERMINAL_MACRO (StorageModifier        ,"StorageModifier"        , "StorageModifierTag" );
     NEW_TERMINAL_MACRO (AccessModifier         ,"AccessModifier"         , "AccessModifierTag" );
     NEW_TERMINAL_MACRO (FunctionModifier       ,"FunctionModifier"       , "FunctionModifierTag" );
     NEW_TERMINAL_MACRO (UPC_AccessModifier     ,"UPC_AccessModifier"     , "UPC_AccessModifierTag" );
     NEW_TERMINAL_MACRO (LinkageModifier        ,"LinkageModifier"        , "LinkageModifierTag" );
     NEW_TERMINAL_MACRO (SpecialFunctionModifier,"SpecialFunctionModifier", "SpecialFunctionModifierTag" );
     NEW_TERMINAL_MACRO (TypeModifier           ,"TypeModifier"           , "TypeModifierTag" );
     NEW_TERMINAL_MACRO (ElaboratedTypeModifier ,"ElaboratedTypeModifier" , "ElaboratedTypeModifierTag" );
     NEW_TERMINAL_MACRO (BaseClassModifier      ,"BaseClassModifier"      , "BaseClassModifierTag" );
     NEW_TERMINAL_MACRO (DeclarationModifier    ,"DeclarationModifier"    , "DeclarationModifierTag" );

  // TV (05/03/2010): OpenCL Access Mode Support
     NEW_TERMINAL_MACRO (OpenclAccessModeModifier, "OpenclAccessModeModifier", "OPENCL_ACCESS_MODE" );

  // DQ (7/22/2006): I have decided to not list this as a modifier, since it is not a part of the C or C++ grammar.
  // NEW_TERMINAL_MACRO (AsmOperandModifier     ,"AsmOperandModifier"     , "AsmOperandModifierTag" );

  // DQ (4/7/2004): Added support for modifiers (organized with a base class)
  // DQ (4/19/2004): Added new modifiers (C++ grammar calls them specifiers)
  // DQ (4/21/2004): Order is important here since the objects are included in
  //                 other objects and the size must be known.
     NEW_NONTERMINAL_MACRO (Modifier,
          ModifierNodes           | ConstVolatileModifier  | StorageModifier    |
          AccessModifier          | FunctionModifier       | UPC_AccessModifier |
          SpecialFunctionModifier | ElaboratedTypeModifier | LinkageModifier    |
          BaseClassModifier       | TypeModifier           | DeclarationModifier|
          OpenclAccessModeModifier, "Modifier", "ModifierTag", false);

     NEW_TERMINAL_MACRO (File_Info, "_File_Info", "_File_InfoTag" );

#if 0
     NEW_TERMINAL_MACRO (File, "File", "FileTag" );
#else
  // DQ (9/2/2008): Separate out the handling of source files from binary files.
  // Note that we may at a later point distinguish source file into: ScriptFile,
  // FortranFile, CppFile, CFile, etc. It is not clear if that is useful. For now
  // we seperate out BinaryFile since it should not be a pointer from SgFile and
  // shared with a source file because there are so many ways that a binary file
  // can be related to a source file (and many source files).  The mapping is left
  // to an analysis phase to define and not defined in the structure of the AST.
     NEW_TERMINAL_MACRO (SourceFile, "SourceFile", "SourceFileTag" );
     NEW_TERMINAL_MACRO (BinaryComposite, "BinaryComposite", "BinaryCompositeTag" );
     BinaryComposite.isBoostSerializable(true);
     NEW_TERMINAL_MACRO (UnknownFile, "UnknownFile", "UnknownFileTag" );

  // Mark this as being able to be an IR node for now and later make it false.
     NEW_NONTERMINAL_MACRO (File, SourceFile | BinaryComposite | UnknownFile , "File", "FileTag", false);
#endif
     NEW_TERMINAL_MACRO (FileList, "FileList", "FileListTag" );
     NEW_TERMINAL_MACRO (Directory, "Directory", "DirectoryTag" );
     NEW_TERMINAL_MACRO (DirectoryList, "DirectoryList", "DirectoryListTag" );
     NEW_TERMINAL_MACRO (Project, "Project", "ProjectTag" );
     NEW_TERMINAL_MACRO (Options, "Options", "OptionsTag" );
     NEW_TERMINAL_MACRO (Unparse_Info, "Unparse_Info", "Unparse_InfoTag" );

  // DQ (9/18/2018): Adding suport for IncludeFile tree to the SgSourceFile
     NEW_TERMINAL_MACRO (IncludeFile, "IncludeFile", "IncludeFileTag" );

     NEW_TERMINAL_MACRO (FuncDecl_attr, "FuncDecl_attr", "FuncDecl_attrTag" );
     NEW_TERMINAL_MACRO (ClassDecl_attr, "ClassDecl_attr", "ClassDecl_attrTag" );

  // MS: reorganized lists in IR not containing non-list objects to simplify IR and traversal design
     NEW_TERMINAL_MACRO (TypedefSeq, "TypedefSeq", "T_TYPEDEF_SEQ" );
     NEW_TERMINAL_MACRO (FunctionParameterTypeList, "FunctionParameterTypeList", "T_FUNCTION_PARAMETER_TYPE_LIST" );

  // DQ (3/12/2004): Added support for template parameters and template arguments
  // Parameters are defined in the template declaration (formal parameters attached
  // to the SgTemplateDeclaration). Arguments are the values specified to build a
  // template instantiation (actual parameters attached to the SgTemplateInstantiationDecl).
     NEW_TERMINAL_MACRO (TemplateParameter, "TemplateParameter", "TemplateParameterTag" );
     NEW_TERMINAL_MACRO (TemplateArgument, "TemplateArgument", "TemplateArgumentTag" );

  // DQ (3/10/2018): I think these IR nodes are no longer used, and if so then they could be deleted.
  // DQ (4/2/2007): Added list as separate IR node to support mixing of lists and data members in IR nodes in ROSETTA.
     NEW_TERMINAL_MACRO (TemplateParameterList, "TemplateParameterList", "TemplateParameterListTag" );
     NEW_TERMINAL_MACRO (TemplateArgumentList, "TemplateArgumentList", "TemplateArgumentListTag" );

  // We don't use these yet, though we might in the future!
  // Grammar::AstNodeClass  ApplyFunction( "ApplyFunction", sageGrammar, "ApplyFunction" );
  // Grammar::AstNodeClass  printFunction( "printFunction", sageGrammar, "printFunction" );

     NEW_NONTERMINAL_MACRO (BitAttribute, FuncDecl_attr | ClassDecl_attr /* | TemplateInstDecl_attr */,"BitAttribute","BitAttributeTag", false);

     NEW_NONTERMINAL_MACRO (Attribute, Pragma | BitAttribute, "Attribute", "AttributeTag", false);

  // DQ (4/25/2004): Must be placed before the modifiers (since it includes one as a data member)
     NEW_TERMINAL_MACRO (ExpBaseClass, "ExpBaseClass", "ExpBaseClassTag" );
     NEW_TERMINAL_MACRO (NonrealBaseClass, "NonrealBaseClass", "NonrealBaseClassTag" );
     NEW_NONTERMINAL_MACRO (BaseClass, ExpBaseClass | NonrealBaseClass, "BaseClass", "BaseClassTag", false );

// #define OLD_GRAPH_NODES 0
// #if OLD_GRAPH_NODES


  // Type for graph node:
  // DQ (8/18/2008): This should be removed in the final version; added for backward compatability!
  //   NEW_TERMINAL_MACRO (DirectedGraphNode, "DirectedGraphNode", "DirectedGraphNodeTag" );

  // Types of graph edges:
     NEW_TERMINAL_MACRO (UndirectedGraphEdge, "UndirectedGraphEdge", "UndirectedGraphEdgeTag" );
     NEW_TERMINAL_MACRO (DirectedGraphEdge,   "DirectedGraphEdge",   "DirectedGraphEdgeTag" );

  // Types of graph nodes and edges:
     NEW_TERMINAL_MACRO (GraphNode,  "GraphNode", "GraphNodeTag");
     NEW_NONTERMINAL_MACRO (GraphEdge, DirectedGraphEdge | UndirectedGraphEdge, "GraphEdge", "GraphEdgeTag", false);

  // Types of Bi-directional graphs:
     NEW_TERMINAL_MACRO (StringKeyedBidirectionalGraph,  "StringKeyedBidirectionalGraph",  "StringKeyedBidirectionalGraphTag" );
     NEW_TERMINAL_MACRO (IntKeyedBidirectionalGraph,     "IntKeyedBidirectionalGraph",     "IntKeyedBidirectionalGraphTag" );
     NEW_NONTERMINAL_MACRO (BidirectionalGraph,  StringKeyedBidirectionalGraph | IntKeyedBidirectionalGraph,  "BidirectionalGraph",     "BidirectionalGraphTag" , false);

  // A bi-directional graph is a type of directed graph (also called an Incidence Directed Graph)
     NEW_NONTERMINAL_MACRO (IncidenceDirectedGraph, BidirectionalGraph,    "IncidenceDirectedGraph",     "IncidenceDirectedGraphTag" , false);

  // Types of graphs:
  // DQ (8/18/2008): Added support for compatability with older IR nodes (will be removed later!)
     //NEW_TERMINAL_MACRO (DirectedGraph,     "DirectedGraph",     "DirectedGraphTag" );
     NEW_TERMINAL_MACRO (IncidenceUndirectedGraph,     "IncidenceUndirectedGraph",     "IncidenceUndirectedGraphTag" );
     NEW_NONTERMINAL_MACRO (Graph, IncidenceDirectedGraph | IncidenceUndirectedGraph  ,"Graph", "GraphTag", false);

  // Internal supporting IR nodes for graphs:
  // DQ (8/18/2008): Added support for container so that we don't mix data member and list elements as children.
     NEW_TERMINAL_MACRO (GraphNodeList, "GraphNodeList", "GraphNodeListTag");
     NEW_TERMINAL_MACRO (GraphEdgeList, "GraphEdgeList", "GraphEdgeListTag");


  // DQ (12/19/2005): Support for explicit qualified names in the source code (currently we generate them
  // if they are required, but this causes the generated source to be a bit verbose for some applications).
     NEW_TERMINAL_MACRO (QualifiedName, "QualifiedName", "QualifiedNameTag" );

#if USE_FORTRAN_IR_NODES
  // DQ (11/19/2007): Support for the Fortran namelist statement (SgNamelistStatement)
     NEW_TERMINAL_MACRO (NameGroup,     "NameGroup",     "TEMP_Name_Group" );

  // DQ (12/1/2007): Support for the Fortran dimension statement
     NEW_TERMINAL_MACRO (DimensionObject, "DimensionObject",     "TEMP_DimensionObject" );

  // DQ (11/20/2007): Support for the data statement
     NEW_TERMINAL_MACRO (DataStatementGroup,  "DataStatementGroup",  "TEMP_DataStatementGroup" );
     NEW_TERMINAL_MACRO (DataStatementObject, "DataStatementObject", "TEMP_DataStatementObject" );
     NEW_TERMINAL_MACRO (DataStatementValue,  "DataStatementValue",  "TEMP_DataStatementValue" );

  // DQ (12/18/2007): Support for format statement
     NEW_TERMINAL_MACRO (FormatItem,     "FormatItem",     "TEMP_FormatItem" );

  // DQ (12/23/2007): Added a list version (required for repeat_sepcification support
     NEW_TERMINAL_MACRO (FormatItemList, "FormatItemList", "TEMP_FormatItemList" );

  // DQ (10/6/2008): Moved to SgLocatedNodeSupport.
  // DQ (10/3/2008): Support for the Fortran "USE" statement and its rename list option.
  // NEW_TERMINAL_MACRO (RenamePair,     "RenamePair",     "TEMP_Rename_Pair" );

  // DQ (10/6/2008): Moved to SgLocatedNodeSupport.
  // DQ (10/6/2008): Support for the Fortran "USE" statement and its rename list option.
  // NEW_TERMINAL_MACRO (InterfaceBody,  "InterfaceBody",  "TEMP_Interface_Body" );
#endif

  // DQ (7/22/2010): And now we implement a type table to make sure that each type is only built once and then properly referenced (shared).
  // This will also make it possible to have exact type equivalence be tested using only pointer equality instead of anything more elaborate.
  // This is also where the FunctionTypeTable should be moved (to tidy up ROSE a bit).
     NEW_TERMINAL_MACRO (TypeTable,         "TypeTable",         "TYPE_TABLE" );

  // DQ (6/12/2013): Added list IR nodes to support Java language requirements.
     NEW_TERMINAL_MACRO (JavaImportStatementList,  "JavaImportStatementList", "JavaImportStatementListTag" );
     NEW_TERMINAL_MACRO (JavaClassDeclarationList, "JavaClassDeclarationList", "JavaClassDeclarationListTag" );

  // DQ (9/15/2018): Adding support for header file usage report (for unparsing).
     NEW_TERMINAL_MACRO (HeaderFileReport, "HeaderFileReport", "HeaderFileReportTag" );

#if 0
  // tps (08/08/07): Added the graph, graph nodes and graph edges
     NEW_NONTERMINAL_MACRO (Support,
          Modifier              | Name                      | SymbolTable         | InitializedName      |
          Attribute             | File_Info                 | File                | Project              |
          Options               | Unparse_Info              | BaseClass           | TypedefSeq           |
          TemplateParameter     | TemplateArgument          | Directory           | FileList             |
          DirectoryList         | FunctionParameterTypeList | QualifiedName       | TemplateArgumentList |
          TemplateParameterList | /* RenamePair                | InterfaceBody       |*/
          Graph                 | GraphNode                 | GraphEdge           |
          NameGroup             | CommonBlockObject         | DimensionObject     | FormatItem           |
          FormatItemList        | DataStatementGroup        | DataStatementObject | DataStatementValue,
          "Support", "SupportTag", false);
#endif

// DQ (3/30/2009): This is the moved because "#if !0" is a problem for MSVS.
// Note that OLD_GRAPH_NODES is set to "1" above...
// #if 0 // !OLD_GRAPH_NODES == 0
// /* (now derived from GraphNode) DirectedGraphNode | */ GraphNodeList | GraphEdgeList |
// #if OLD_GRAPH_NODES == 1
     NEW_NONTERMINAL_MACRO (Support,
          Modifier              | Name                      | SymbolTable              | 
          Attribute             | File_Info                 | File                     | Project              |
          Options               | Unparse_Info              | BaseClass                | TypedefSeq           |
          TemplateParameter     | TemplateArgument          | Directory                | FileList             |
          DirectoryList         | FunctionParameterTypeList | QualifiedName            | TemplateArgumentList |
          TemplateParameterList | /* RenamePair                | InterfaceBody       |*/
          Graph                 | GraphNode                 | GraphEdge                |
          GraphNodeList         | GraphEdgeList             | TypeTable                |
          NameGroup             | DimensionObject           | FormatItem               |
          FormatItemList        | DataStatementGroup        | DataStatementObject      | IncludeFile          |
          DataStatementValue    | JavaImportStatementList   | JavaClassDeclarationList | HeaderFileReport,
          "Support", "SupportTag", false);
//#endif


  // ***********************************************************************
  // ***********************************************************************
  //                       Header Code Declaration
  // ***********************************************************************
  // ***********************************************************************

     Support.isBoostSerializable(true);

  // Header declarations for Support
  // Support.setSubTreeFunctionPrototype ( "HEADER", "../Grammar/sageCommon.code");
     Support.setFunctionPrototype        ( "HEADER", "../Grammar/Support.code");

     SymbolTable.setFunctionPrototype         ( "HEADER_SYMBOL_TABLE", "../Grammar/Support.code");
#if 1
  // DQ (5/22/2006): I think we really do need this since this is required state for
  // iteration through all symbols (except that I had expected them to be unique).

  // AJ (10/21/2004): Modified Sage III to use STL based hash table implementation.
  // MK: I moved the following data member declarations from ../Grammar/Support.code to this position:
  // SymbolTable.setDataPrototype("SgSymbolHashBase::iterator","iterator", "= NULL",
  //                      NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     SymbolTable.setDataPrototype("hash_iterator","iterator", "",
                            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif
  // We do not traverse the following data member for the moment!
  // DQ (11/6/2001): changed option to permit generation of access functions
  // The name is used internally within the find member function to hold the string being sought
  // SymbolTable.setDataPrototype("SgName","name", "= NULL",
     SymbolTable.setDataPrototype("SgName","name", "= \"\"",
                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (11/6/2001): changed option to permit generation of access functions
  // The no_name is used internally within the find member function to indicate if the name is being used
     SymbolTable.setDataPrototype("bool","no_name", "= false",
                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // AJ (10/21/2004): Modified Sage III to use STL based hash table implementation.
  // DQ (11/6/2001): changed option to permit generation of access functions
  // SymbolTable.setDataPrototype("SgSymbolHashMultiMap*","table", "= NULL",
  //                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);
     SymbolTable.setDataPrototype(" rose_hash_multimap*","table", "= NULL",
                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);

  // DQ (6/12/2007): Fixed use of std::set<SgNode*> to use SgNodeSet so that ROSETTA's test for
  // pointer to IR node can just look for "*" in the type.  This is a internal detail of ROSETTA.
  // DQ (3/10/2007): Adding set for symbols so that we can support fast tests for existance.
  // SymbolTable.setDataPrototype("SgNodeSetPtr","symbolSet", "= NULL",
  //          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // SymbolTable.setDataPrototype("std::set<SgNode*>","symbolSet", "",
     SymbolTable.setDataPrototype("SgNodeSet","symbolSet", "",
                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/27/2010): data member to force case sensitive or case insensitive semantics (default is case sensitive).
     SymbolTable.setDataPrototype("bool","case_insensitive","= false",
                            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);

  // DQ (7/12/2014): This supports name qualification (see test2014_90.C) and the use of SgAliasSymbols before
  // of after their use in the AST relative to the causal nodes (SgUsingDirectiveStatement, SgUsingDeclarationStatement, 
  // SgBaseClass, etc.).  This is a requirement imposed because we must support the generation of source code
  // from the AST (and correct name qualification as a result).
     SymbolTable.setDataPrototype("static SgNodeSet","aliasSymbolCausalNodeSet", "",
                            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (7/22/2010): Added type table to support stricter uniqueness of types and proper sharing.
     TypeTable.setFunctionPrototype( "HEADER_TYPE_TABLE", "../Grammar/Support.code" );
     TypeTable.setAutomaticGenerationOfConstructor(false);
     TypeTable.setDataPrototype    ( "SgSymbolTable*","type_table","= NULL",
                                                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);

  // DQ (3/29/2014): data member to force search of base classes (for Java).
     SymbolTable.setDataPrototype("static bool","force_search_of_base_classes","= false",
                            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);


     Name.setFunctionPrototype                ( "HEADER_NAME", "../Grammar/Support.code");

#if 0
     Name.setDataPrototype( "SgNodePtrList", "TemplateArgs", "= NULL",
                            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#else
  // DQ (2/29/2004): Commented out
  // Name.setDataPrototype( "SgNodePtrList", "TemplateArgs", "",
  //         CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

     //Name.setDataPrototype( "SgNodePtrList", "TemplateArgs", "= SgEmptyNodePtrList");
     //Name.setDataPrototype( "SgTypePtrList", "TemplateArgs", "= NULL");
     //Name.setDataPrototype( "SgNodePtrList", "TemplateParams", "= SgEmptyNodePtrList");
#if 0
     Name.setDataPrototype( "SgNodePtrList", "TemplateParams", "= NULL",
                            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#else
  // DQ (2/29/2004): Commented out
  // Name.setDataPrototype( "SgNodePtrList", "TemplateParams", "",
  //         CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif
     Name.setDataPrototype("std::string","char","= \"\"",
                            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (2/28/2004): Data member to hold orignal template name (without parameters or arguments)
  // Name.setDataPrototype("string","TemplateName","",
  //         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     Attribute.setFunctionPrototype           ( "HEADER_ATTRIBUTE", "../Grammar/Support.code");
     Pragma.setFunctionPrototype              ( "HEADER_PRAGMA", "../Grammar/Support.code");
     BitAttribute.setFunctionPrototype        ( "HEADER_BIT_ATTRIBUTE", "../Grammar/Support.code");

  // DQ (4/6/2004): Depricated ModifierNodes node and new separate TypeModifier and StorageModifier nodes
  // DQ (4/19/2004): New modifiers (C++ grammar calls them specifiers)
  //    ConstVolatileModifier, StorageModifier, AccessModifier, FunctionModifier,
  //    UPC_AccessModifier, SpecialFunctionModifier, DeclarationModifier, TypeModifier,
  //    ElaboratedTypeModifier, LinkageModifier, BaseClassModifier
     Modifier.setFunctionPrototype                ( "HEADER_MODIFIER"                 , "../Grammar/Support.code");
     ModifierNodes.setFunctionPrototype           ( "HEADER_MODIFIER_NODES"           , "../Grammar/Support.code");
     ConstVolatileModifier.setFunctionPrototype   ( "HEADER_CV_TYPE_MODIFIER"         , "../Grammar/Support.code");
     StorageModifier.setFunctionPrototype         ( "HEADER_STORAGE_MODIFIER"         , "../Grammar/Support.code");
     AccessModifier.setFunctionPrototype          ( "HEADER_ACCESS_MODIFIER"          , "../Grammar/Support.code");
     FunctionModifier.setFunctionPrototype        ( "HEADER_FUNCTION_MODIFIER"        , "../Grammar/Support.code");
     UPC_AccessModifier.setFunctionPrototype      ( "HEADER_UPC_ACCESS_MODIFIER"     , "../Grammar/Support.code");
     SpecialFunctionModifier.setFunctionPrototype ( "HEADER_SPECIAL_FUNCTION_MODIFIER", "../Grammar/Support.code");
     TypeModifier.setFunctionPrototype            ( "HEADER_TYPE_MODIFIER"            , "../Grammar/Support.code");
     DeclarationModifier.setFunctionPrototype     ( "HEADER_DECLARATION_MODIFIER"     , "../Grammar/Support.code");
     ElaboratedTypeModifier.setFunctionPrototype  ( "HEADER_ELABORATED_TYPE_MODIFIER" , "../Grammar/Support.code");
     LinkageModifier.setFunctionPrototype         ( "HEADER_LINKAGE_MODIFIER"         , "../Grammar/Support.code");
     BaseClassModifier.setFunctionPrototype       ( "HEADER_BASECLASS_MODIFIER"       , "../Grammar/Support.code");

     File_Info.setFunctionPrototype           ( "HEADER_FILE_INFORMATION", "../Grammar/Support.code");

  // Skip building a parse function for this AstNodeClass/nonterminal of the Grammar
     if (isRootGrammar() == false)
        {
          Attribute.excludeFunctionPrototype ( "HEADER_PARSER", "../Grammar/Node.code" );
          Attribute.excludeFunctionSource    ( "SOURCE_PARSER", "../Grammar/parserSourceCode.macro" );
        }

     SourceFile.setFunctionPrototype          ( "HEADER_APPLICATION_SOURCE_FILE", "../Grammar/Support.code");
  // SourceFile.setAutomaticGenerationOfConstructor(false);

     BinaryComposite.setFunctionPrototype          ( "HEADER_APPLICATION_BINARY_FILE", "../Grammar/Support.code");

     UnknownFile.setFunctionPrototype          ( "HEADER_APPLICATION_UNKNOWN_FILE", "../Grammar/Support.code");



     File.setFunctionPrototype                ( "HEADER_APPLICATION_FILE", "../Grammar/Support.code");
  // File.setAutomaticGenerationOfConstructor(false);
  // Later we can have this be autogenerated (let's see what we need first)
  // DQ (12/4/2004): Now we automate the generation of the destructors
  // File.setAutomaticGenerationOfDestructor (false);

     FileList.setFunctionPrototype             ( "HEADER_APPLICATION_FILE_LIST", "../Grammar/Support.code");
     Directory.setFunctionPrototype            ( "HEADER_APPLICATION_DIRECTORY", "../Grammar/Support.code");
     DirectoryList.setFunctionPrototype        ( "HEADER_APPLICATION_DIRECTORY_LIST", "../Grammar/Support.code");

  // DQ (6/12/2013): Added to support Java requirements.
     JavaImportStatementList.setFunctionPrototype  ( "HEADER_JAVA_IMPORT_STATEMENT_LIST", "../Grammar/Support.code");
     JavaClassDeclarationList.setFunctionPrototype ( "HEADER_JAVA_CLASS_DECLARATION_LIST", "../Grammar/Support.code");

  // DQ (12/19/2005): Support for explicitly specified qualified names
     QualifiedName.setFunctionPrototype        ( "HEADER_QUALIFIED_NAME", "../Grammar/Support.code");

     File_Info.setAutomaticGenerationOfConstructor(false);
  // DQ (12/4/2004): Now we automate the generation of the destructors
  // File_Info.setAutomaticGenerationOfDestructor (false);

     Project.setFunctionPrototype             ( "HEADER_APPLICATION_PROJECT", "../Grammar/Support.code");
  // DQ (4/7/2001) Build our own constructor and destructor so that we can handle the fileList (an STL list)
     Project.setAutomaticGenerationOfConstructor(false);
  // DQ (12/4/2004): Now we automate the generation of the destructors
  // Project.setAutomaticGenerationOfDestructor (false);

     Options.setFunctionPrototype             ( "HEADER_OPTIONS", "../Grammar/Support.code");
     Unparse_Info.setFunctionPrototype          ( "HEADER_UNPARSE_INFO", "../Grammar/Support.code");

  // MK: I have moved the following data member declarations from ../Grammar/Support.code to this place
  // Space for unparse_type_num tag

  // DQ (11/4/2003): Previous type was "unsigned int" added support
  // for longer unparse_type_num but had to specify type of
  // p_unparse_attribute as "long long int" instead of unparse_type_num
  // since it seems that all enum types are considered in compatable with
  // long long int within bitwise operations (using g++ 2.96).
  // Unparse_Info.setDataPrototype("long long int", "unparse_attribute", "= b_enum_defaultValue",
  //           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Unparse_Info.setDataPrototype("SgBitVector", "unparse_attribute", "",
                                   NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // Space for access_attr_enum tag
     Unparse_Info.setDataPrototype("int", "access_attribute", "= 0",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Unparse_Info.setDataPrototype("int", "nested_expression", "= 0",
                                   NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // For carrying along the variable name when printing out function/array
  // type with the symbol's name
     Unparse_Info.setDataPrototype("std::string", "operator_name", "= \"\"",
                                   NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Unparse_Info.setDataPrototype("SgName", "var_name", "= \"\"",
                                   NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // Keep ptr to the declaration statement
     Unparse_Info.setDataPrototype("SgDeclarationStatement*", "declstatement_ptr", "= NULL",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // Keep current scope's type, set in SgClassDefinition's unparser
     Unparse_Info.setDataPrototype("SgNamedType*", "current_context", "= NULL",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // Keep track of previous array type's index string -- this is because
  // array is built like this  A[2][3]  Array(Array(A,3),2)
  // We do not traverse the following data member for the moment!
     Unparse_Info.setDataPrototype("SgName", "array_index_list", "= \"\"",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (1/12/2004): Put static back, but cleared static list in
  //                 post_construction_initialization() member function.
  // DQ (1/11/2004): Bugfix for autonomous typedefs (can't make this a static
  //                 member since it then has side effects).
  // Support for unparsing references to structure tags within the same structure
  // Example is: struct Xtag { Xtag *xref; } X; such code is common in tree data
  // structures.
  // Unparse_Info.setDataPrototype("list<SgNamedType*>", "structureTagList", "",
  // Unparse_Info.setDataPrototype("SgTypePtrList", "structureTagProcessingList", "",
     Unparse_Info.setDataPrototype("static SgTypePtrList", "structureTagProcessingList", "",
          NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Build our own constructor so that it can contain debugging into to make sure it is not
  // called within the unparser (unparser should only call the copy constructor to preserve
  // the semantics of the inherited attribute mechanism)
     Unparse_Info.setAutomaticGenerationOfConstructor(false);

  // DQ (9/6/2004): Added support for output of name qualification (list of classes or namespace required to resolve types)
  // Unparse_Info.setDataPrototype("SgSymbolPtrList","listOfScopeSymbols","",
  //        NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL);

  // DQ (5/11/2011): This is depricated because this is older support for name qualification.
  // DQ (9/8/2004): Added support for output of name qualification for namespaces, needed a different
  // variable specific for namespaces because "current_context" is a SgNamedType.
     Unparse_Info.setDataPrototype("SgNamespaceDeclarationStatement*","current_namespace","= NULL",
                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (1/31/2006): Support for static bool forceDefaultConstructorToTriggerError;
     Unparse_Info.setDataPrototype("static bool","forceDefaultConstructorToTriggerError","",
                                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

  // DQ (3/18/2006): Support for output of information about formatting within code generation (unparsing).
  // This information is helpful in visualizing where and why linefeeds and indentation are introduced in
  // the code generation phase (unparsing).
     Unparse_Info.setDataPrototype("bool","outputCodeGenerationFormatDelimiters","= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);

  // DQ (5/11/2011): This is depricated because this is older support for name qualification.
  // DQ (10/10/2006): Support for reference to a list that would be used for qualified name generation for any type.
     Unparse_Info.setDataPrototype ( "SgQualifiedNamePtrList", "qualifiedNameList", "= SgQualifiedNamePtrList()",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (10/20/2006): Added support for output of function calls.  We need to know if we are
  // unparsing a function call so that we can add the name qualification properly.  The function
  // call expression (SgFunctionCallExp) is used to query the AST as ot the relative position of
  // any defining or nondefining function declarations that would define the scope of the function.
     Unparse_Info.setDataPrototype("SgFunctionCallExp*","current_function_call","= NULL",
                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (5/11/2011): This is depricated because this is older support for name qualification.
  // DQ (5/22/2007): Added scope information so that we could lookup hidden list to get qualified names correct.
     Unparse_Info.setDataPrototype("SgScopeStatement*","current_scope","= NULL",
                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (5/21/2011): Added information required for new name qualification support. This is the node used to
  // lookup the string computed to support the name qualified name for either the declaration name or type name
  // that requires qualiticcation.
     Unparse_Info.setDataPrototype("SgNode*","reference_node_for_qualification","= NULL",
                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (5/11/2011): Added information required for new name qualification support. We now strore this information
  // where the named constrcuts are referenced and this information is passed through the SgUnparse_Info object.
     Unparse_Info.setDataPrototype("int","name_qualification_length","= 0",
                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (5/11/2011): Added information required for new name qualification support.
     Unparse_Info.setDataPrototype("bool","type_elaboration_required","= false",
                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (5/11/2011): Added information required for new name qualification support.
     Unparse_Info.setDataPrototype("bool","global_qualification_required","= false",
                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // driscoll6 (6/6/2011): Added nesting level information for Python unparsing.
     Unparse_Info.setDataPrototype("int","nestingLevel","= 0",
                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);

  // DQ (29/8/2017): changed the language enum name so that we could use it for both input and output language specifications.
  // DQ (9/15/2012): Added support to specify the language directly (required to unparse SgBoolVal in some cases where 
  // they are used in SgTemplateParameters in a SgTemplateInstantiation which would be constrcuted before having its
  // parent set (thus not allowing the unparseBoolVal() to call TransformationSupport::getFile(expr) and find the 
  // associated SgFile IR node). This is only an issue during AST construction.
  // Unparse_Info.setDataPrototype("SgFile::outputLanguageOption_enum","language","= SgFile::e_default_output_language",
  //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     Unparse_Info.setDataPrototype("SgFile::languageOption_enum","language","= SgFile::e_default_language",
                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);

  // DQ (1/10/2015): We need to save a pointer to the SgSourceFile to support the token based unparsing efficiently.
     Unparse_Info.setDataPrototype("SgSourceFile*","current_source_file","= NULL",
                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (4/28/2017): Added information required for use of generated names in the output of types for template 
  // arguments as used in symbol table lookup.
     Unparse_Info.setDataPrototype("bool","use_generated_name_for_template_arguments","= false",
                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     Unparse_Info.setDataPrototype("bool","user_defined_literal","= false",
                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     BaseClass.setFunctionPrototype           ( "HEADER_BASECLASS", "../Grammar/Support.code");
     ExpBaseClass.setFunctionPrototype        ( "HEADER_EXP_BASE_CLASS", "../Grammar/Support.code");
     NonrealBaseClass.setFunctionPrototype    ( "HEADER_NONREAL_BASE_CLASS", "../Grammar/Support.code");

  // DQ (4/29/2004): Removed in place of new modifier interface
  // BaseClass.setDataPrototype               ( "int"                , "base_specifier", "= 0",
  //             CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // BaseClass.setDataPrototype               ( "SgClassDeclaration*", "base_class", "= NULL",
  //             CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     BaseClass.setDataPrototype               ( "SgClassDeclaration*", "base_class", "= NULL",
                                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, CLONE_PTR);

  // DQ (6/21/2005): This is used in the EDG/Sage interface when the SgBaseClass constructor is called
  // modified to be a boolean type instead of unsigned.
     BaseClass.setDataPrototype               ( "bool", "isDirectBaseClass", "= false",
                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (1/21/2019): Fix this to cause access functions to be automatically generated.
  // DQ (11/7/2007): This should not be shared when a copy is make (see copytest_2007_26.C).
  // DQ (4/25/2004): New interfce for modifiers (forced to make this a pointer to a SgBaseClassModifier
  //                 because it could not be specified before the declaration of BaseClass, limitations
  //                 in ROSETTA).
  // BaseClass.setDataPrototype               ( "SgBaseClassModifier*", "baseClassModifier", "= NULL",
  //              NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);
  // BaseClass.setDataPrototype               ( "SgBaseClassModifier*", "baseClassModifier", "= NULL",
  //             NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);
     BaseClass.setDataPrototype               ( "SgBaseClassModifier*", "baseClassModifier", "= NULL",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);

  // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
  // DQ (5/11/2011): Added support for name qualification.
  // BaseClass.setDataPrototype ( "int", "name_qualification_length", "= 0",
  //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     BaseClass.setDataPrototype ( "int", "name_qualification_length", "= 0",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
  // DQ (5/11/2011): Added information required for new name qualification support.
  // BaseClass.setDataPrototype("bool","type_elaboration_required","= false",
  //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     BaseClass.setDataPrototype("bool","type_elaboration_required","= false",
                                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
  // DQ (5/11/2011): Added information required for new name qualification support.
  // BaseClass.setDataPrototype("bool","global_qualification_required","= false",
  //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     BaseClass.setDataPrototype("bool","global_qualification_required","= false",
                                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     ExpBaseClass.setDataPrototype ( "SgExpression*", "base_class_exp", "= NULL",
                                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     NonrealBaseClass.setDataPrototype ( "SgNonrealDecl*", "base_class_nonreal", "= NULL",
                                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     FuncDecl_attr.setFunctionPrototype ( "HEADER_FUNCTION_DECLARATION_ATTRIBUTE", "../Grammar/Support.code");
     ClassDecl_attr.setFunctionPrototype( "HEADER_CLASS_DECLARATION_ATTRIBUTE", "../Grammar/Support.code");
  // TemplateInstDecl_attr.setFunctionPrototype( "HEADER_TEMPLATE_INSTANTIATION_DECLARATION_ATTRIBUTE", "../Grammar/Support.code");


  // DQ (1/18/2006): renames this to be consistant and to allow the generated functions to map to
  // the virtual SgNode::get_file_info(). This then meens that we need to remove the use of
  // SgPragma::get_fileInfo() where it is used.
  // Added to SAGE 3 (not present in SAGE 2) to support PragmaStatement (unbound pragmas)
  // Pragma.setDataPrototype  ( "Sg_File_Info*", "fileInfo", "= NULL",
  //        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);
  // Pragma.setDataPrototype  ( "Sg_File_Info*", "file_info", "= NULL",
  //        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);

  // DQ (1/11/13): The astCopy and astDelete are inconsistant if we share the pointer (we need to copy using CLONE_PTR 
  // so that we will build a new Sg_File_Info object).
  // Pragma.setDataPrototype  ( "Sg_File_Info*", "startOfConstruct", "= NULL",
  //                            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);
     Pragma.setDataPrototype  ( "Sg_File_Info*", "startOfConstruct", "= NULL",
                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);
  // DQ (5/3/2012): Added endOfConstruct to be uniform and prepare SgPragma API to movement of SgPragma to be derived from SgLocatedNode in the future.
  // Pragma.setDataPrototype  ( "Sg_File_Info*", "endOfConstruct", "= NULL",
  //             CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);
     Pragma.setDataPrototype  ( "Sg_File_Info*", "endOfConstruct", "= NULL",
                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);
     Pragma.setDataPrototype ( "short" , "printed", "= 0",
                 NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/1/2015): Build the access functions, but don't let the set_* access function set the "p_isModified" flag.
  // DQ (1/3/2006): Added attribute via ROSETTA (changed to pointer to AstAttributeMechanism)
  // Modified implementation to only be at specific IR nodes.
  // Pragma.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
  //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     Pragma.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
            NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);
     Pragma.setFunctionPrototype( "HEADER_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
     Pragma.setFunctionSource   ( "SOURCE_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");


  // DQ (5/6/2005): Build support for directories in addition to files (for support of large projects)
  // DQ (1/20/2010): I would like to avoid links and permissions etc., I hope that such details will 
  // not be required in the future.
     Directory.setDataPrototype ( "std::string", "name", "= \"\"",
                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Directory.setDataPrototype ( "SgFileList*", "fileList", "= NULL",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // Note that this must be of type: "SgDirectoryList*" since it a pointer to an IR node and not a STL list.
     Directory.setDataPrototype ( "SgDirectoryList*", "directoryList", "= NULL",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (11/1/2015): Build the access functions, but don't let the set_* access function set the "p_isModified" flag.
  // DQ (1/3/2006): Added attribute via ROSETTA (changed to pointer to AstAttributeMechanism)
  // Modified implementation to only be at specific IR nodes.
  // Directory.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
  //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     Directory.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
            NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);
     Directory.setFunctionPrototype      ( "HEADER_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
     Directory.setFunctionSource         ( "SOURCE_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");

#if 0
  // DQ (6/12/2013): Added support for Java requirements.
     JavaImportStatementList.setDataPrototype ( "SgJavaImportStatementPtrList", "java_import_list", "",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     JavaClassDeclarationList.setDataPrototype ( "SgClassDeclarationPtrList", "java_class_list", "",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#else
  // DQ (11/20/2013): Modified these data members to be traversed by the AST traversal mechanism.
  // DQ (6/12/2013): Added support for Java requirements.
     JavaImportStatementList.setDataPrototype ( "SgJavaImportStatementPtrList", "java_import_list", "",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     JavaClassDeclarationList.setDataPrototype ( "SgClassDeclarationPtrList", "java_class_list", "",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

     FileList.setDataPrototype          ( "SgFilePtrList", "listOfFiles", "",
  //             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  //             NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
                 NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (11/1/2015): Build the access functions, but don't let the set_* access function set the "p_isModified" flag.
  // DQ (1/23/2010): Not clear if we should put attributes here, but for now it is OK.
  // This is presently need to avoid an error in the roseHPCToolkitTests, but there may be 
  // a better way to avoid that error.
  // FileList.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
  //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     FileList.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
                               NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);
     FileList.setFunctionPrototype      ( "HEADER_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
     FileList.setFunctionSource         ( "SOURCE_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");

  // DQ (5/6/2005): ROSETTA generated the wrong code for this case until it was fixed 5/8/2005 in Cxx_GrammarTreeTraversalSuccessorContainer.C
     DirectoryList.setDataPrototype     ( "SgDirectoryPtrList", "listOfDirectories", "",
  //             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
                 NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (12/19/2005): Support for explicitly specified qualified names (scope is a shared reference to a SgScopeStatement)
     QualifiedName.setDataPrototype     ( "SgScopeStatement*", "scope", "= NULL",
                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


  // ******************************************************************
  // **************************** File IR Node ************************
  // ******************************************************************

  // Header declarations for Node
  // File.setPredeclarationString     ("HEADER_APPLICATION_FILE_PREDECLARATION" , "../Grammar/Support.code");

  // DQ (1/18/2006): Rename this to be consistant and to allow the generated functions to map to
  // the virtual SgNode::get_file_info(). This then meens that we need to remove the use of
  // SgFile::get_fileInfo() where it is used.
  // DQ (8/17/2005): Added to make it simple to both hold the filename of the current file AND
  // support the map mechanism required to permit string comparisions of filenames to be done
  // using integer (key) comparision.  This removes a huge number of string comparisons from the
  // processing of IR nodes (e.g. in the unparser, and in the traverseInFile() AST traversal mechanism).
  // Use of this veraible will be added incrementally in the next releases.  We could have alternativel
  // just stored the map key integer value, but that would expose the details of the implementation.
  // As it is we have a full Sg_File_Info object and the line number and column number, and perhaps the
  // IR node classification flags as well, are largely meaningless.  A slightly better factorization
  // might be possible in the future.
  // RPM (12/2/2008): Data members that appear to control ROSE behavior rather than represent some
  //                  property of the parsed source code have accessors generated via
  //                  BUILD_FLAG_ACCESS_FUNCTIONS rather than BUILD_ACCESS_FUNCTIONS. The only
  //                  difference between the two is that the FLAG version doesn't call
  //                  set_isModified(true) in the ROSETTA-generated get_* methods.
  // File.setDataPrototype         ( "Sg_File_Info*" , "fileInfo", "= NULL",
  //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);

  // DQ (5/3/2007): Renamed to be consistant with SgLoctedNode use, but added get_file_Info()
  // access function to support previous interface for a while.
  // File.setDataPrototype         ( "Sg_File_Info*" , "file_info", "= NULL",
  //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);

  // DQ (1/11/13): The astCopy and astDelete are inconsistant if we share the pointer (we need to copy using CLONE_PTR 
  // so that we will build a new Sg_File_Info object).
  // File.setDataPrototype         ( "Sg_File_Info*" , "startOfConstruct", "= NULL",
  //                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);
     File.setDataPrototype         ( "Sg_File_Info*" , "startOfConstruct", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);


  // DQ (9/2/2008): We want to move this to be in the SgSourceFile
     SourceFile.setDataPrototype   ( "SgGlobal*", "globalScope", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (10/1/2008): Added support for lists of SgModuleStatement where they are
  // not a part of the current translation unit.  A place for the information in
  // the *.mod files to be put.  This is the data member for the list.
     SourceFile.setDataPrototype   ( "SgModuleStatementPtrList", "module_list", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/20/2010): Added SgTokenPtrList to support a token list at the SourceFile level.
  // In the future we may have more than one to support pre and post processing using CPP or Fortran equivalent).
     SourceFile.setDataPrototype   ( "SgTokenPtrList", "token_list", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (9/15/2011): A home for declarations that have no defined scope in the program (see test2011_80.C)
  // Some declarations (e.g. "struct b* x;" cause a type, and thus a declaration and symbol, to be constructed 
  // with no clear indication as to which scope into which it should be defined. This has become clear as part
  // of debugging the name qualification support.  This scope permits use to have a location (holding scope) 
  // to support this issue in C and C++.  Another example is the a templated class nested in a templated class.
  // Here we also don't have a location for the declaration and the symbol that is generated for this case.
     SourceFile.setDataPrototype   ( "SgGlobal*", "temp_holding_scope", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#if 0
  // DQ (9/12/2009): Adding support for new name qualification (not ready yet).
  // DQ (9/11/2009): Added support for mapping id numbers to statement pointers.
  // This is part of support for a new implementation of name qualification.
     SourceFile.setDataPrototype   ( "SgStatementPtrList", "statementNumberContainer", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#if 0
  // DQ (6/12/2013): Added Java support for reference to Java package, imports and type declarations.
     SourceFile.setDataPrototype   ( "SgJavaPackageStatement *", "package", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, NO_COPY_DATA);
     SourceFile.setDataPrototype   ( "SgJavaImportStatementList*", "import_list", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, NO_COPY_DATA);
     SourceFile.setDataPrototype   ( "SgJavaClassDeclarationList*", "class_list", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, NO_COPY_DATA);
#else
  // DQ (11/19/2013): Modified these data members to be traversed by the AST traversal mechanism.
  // DQ (6/12/2013): Added Java support for reference to Java package, imports and type declarations.
     SourceFile.setDataPrototype   ( "SgJavaPackageStatement *", "package", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, DEF_DELETE, NO_COPY_DATA);
     SourceFile.setDataPrototype   ( "SgJavaImportStatementList*", "import_list", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, DEF_DELETE, NO_COPY_DATA);
     SourceFile.setDataPrototype   ( "SgJavaClassDeclarationList*", "class_list", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, DEF_DELETE, NO_COPY_DATA);
#endif

#if 0
     SourceFile.setDataPrototype   ( "SgJavaImportStatementPtrList", "import_list", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     SourceFile.setDataPrototype   ( "SgClassDeclarationPtrList", "class_list", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
#endif

  // DQ (8/7/2018): Mark files explicitly as header files (support for unparse headers and unparse tokens).
     SourceFile.setDataPrototype   ( "bool", "isHeaderFile", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     SourceFile.setDataPrototype   ( "bool", "isHeaderFileIncludedMoreThanOnce", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (9/15/2018): Adding support for accumulating data to support a report on header file handling (for unparsing of header files).
     SourceFile.setDataPrototype   ( "SgHeaderFileReport*", "headerFileReport", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);
#if 0
  // DQ (11/15/2018): This is redundant with the list in the SgIncludeFile, and now by design every source file has an associated_include_file if it has any include files.
  // DQ (9/18/2018): Adding support for building the include file tree for each source file.
     SourceFile.setDataPrototype   ( "SgIncludeFilePtrList", "include_file_list", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
#endif
  // DQ (11/8/2018): These are added include directories that will be used in the generation of the backend compiler 
  // command line (after all other include directives have been output). This option supports the unparsing of header 
  // files which requires source files to be positioned a locations relative to an application root file position, 
  // and header fils to be included using additional include paths. This is required to handled header files using 
  // "../" prefixes, and different include paths are required depending upon if they are transformed.
     SourceFile.setDataPrototype("SgStringList","extraIncludeDirectorySpecifierList", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/10/2018): Define a link to the SgIncludeFile that is associated with this SgSourceFile (valid pointer only if isHeaderFile == true).
     SourceFile.setDataPrototype   ( "SgIncludeFile*", "associated_include_file", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);

  // DQ (11/6/2018): Added to support unparsing of headers and source files for whole applications having 
  // multiple levels of directory structure.
  // SourceFile.setDataPrototype("std::string", "applicationRootDirectory", "= \"\"",
  //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // SourceFile.setDataPrototype("bool", "usingApplicationRootDirectory", "= false",
  //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     UnknownFile.setDataPrototype   ( "SgGlobal*", "globalScope", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (9/18/2018): Adding support for building the include file tree for each source file.
     IncludeFile.setFunctionPrototype          ( "HEADER_INCLUDE_FILE", "../Grammar/Support.code");

  // DQ (9/18/2018): Added source file which is initialized when unparsing headers.
  // IncludeFile.setDataPrototype ( "SgName", "filename", "",
  //                                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     IncludeFile.setDataPrototype ( "SgName", "filename", "= \"\"",
                                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (9/18/2018): Added source file which is initialized when unparsing headers.
     IncludeFile.setDataPrototype ( "SgSourceFile*", "source_file", " = NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

  // DQ (11/15/2018): Allow traversals of the include tree.
  // IncludeFile.setDataPrototype ( "SgIncludeFilePtrList", "include_file_list", "",
  //                                 NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     IncludeFile.setDataPrototype ( "SgIncludeFilePtrList", "include_file_list", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

  // DQ (9/18/2018): Added source file which is initialized when unparsing headers.
     IncludeFile.setDataPrototype ( "unsigned int", "first_source_sequence_number", " = 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     IncludeFile.setDataPrototype ( "unsigned int", "last_source_sequence_number", " = 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

     IncludeFile.setDataPrototype   ( "bool", "isIncludedMoreThanOnce", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (10/28/2018): Where a header file is used more than once we introduce the simplifying concept of
  // a primary use.  The first use is the primary use, and this provides for an initial implementation.
  // Note that multiple uses of the same header file may be transformed differently, so rewriting the 
  // header file might not be well defined (unless we output all versions).
     IncludeFile.setDataPrototype   ( "bool", "isPrimaryUse", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (10/28/2018): Add the hash for the file (to provide error checking to JIT mechanisms).
     IncludeFile.setDataPrototype   ( "std::string", "file_hash", "= \"\"",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/5/2018): Added more information that we have available.
     IncludeFile.setDataPrototype ( "SgName", "name_used_in_include_directive", "= \"\"",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/9/2018): Added source file link which is initialized when attaching CPP directives and comments.
     IncludeFile.setDataPrototype ( "SgSourceFile*", "source_file_of_translation_unit", " = NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     IncludeFile.setDataPrototype ( "SgSourceFile*", "including_source_file", " = NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

  // DQ (11/9/2018): Added to support link to parent of any nested include file (null indicates that the include file is the parent of a SgSourceFile.
     IncludeFile.setDataPrototype ( "SgIncludeFile*", "parent_include_file", " = NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

   // DQ (11/12/2018): Added support to detect system vs. non system includes and preincluded files.
   // System include files are specified using <> syntax, non-system include files are specified using "" syntax, 
   // and preinclude include files are specified using a preinclude option on the commandline.
     IncludeFile.setDataPrototype   ( "bool", "isSystemInclude", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     IncludeFile.setDataPrototype   ( "bool", "isPreinclude", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/16/2018): Mark the SgIncludeFile for header files that will drive the explicit inclusion of an include path at compile time.
     IncludeFile.setDataPrototype   ( "bool", "requires_explict_path_for_unparsed_headers", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


  // DQ (11/12/2018): Mark this include file as not being able to support tranformations under token-based unparsing.
     IncludeFile.setDataPrototype   ( "bool", "can_be_supported_using_token_based_unparsing", "= true",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/5/2018): Added more information that we have available.
     IncludeFile.setDataPrototype ( "SgName", "directory_prefix", "= \"\"",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     IncludeFile.setDataPrototype ( "SgName", "name_without_path", "= \"\"",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/16/2018): Save the applicationRootDirectory.
     IncludeFile.setDataPrototype ( "SgName", "applicationRootDirectory", "= \"\"",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/15/2018): will_be_unparsed
     IncludeFile.setDataPrototype   ( "bool", "will_be_unparsed", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/15/2018): Allow us to mark the ROSE generated include file that holds macros and declarations for builtin functions.
     IncludeFile.setDataPrototype   ( "bool", "isRoseSystemInclude", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/21/2018): Allow us to mark the ROSE generated include files from system directories.
     IncludeFile.setDataPrototype   ( "bool", "from_system_include_dir", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/21/2018): Allow us to mark the ROSE generated include files from system directories.
     IncludeFile.setDataPrototype   ( "bool", "preinclude_macros_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/21/2018): Allow us to mark the ROSE generated include files from system directories.
     IncludeFile.setDataPrototype   ( "bool", "isApplicationFile", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (9/18/2018): We can likely eliminate this IR node now that we store the include file tree directly
  // (though this one is computed from the EDG/ROSE translation instead of from the CPP include directives).
  // DQ (9/15/2018): Adding support for report on header file handling (for unparsing).
     HeaderFileReport.setFunctionPrototype ( "HEADER_HEADER_FILE_REPORT", "../Grammar/Support.code");

     HeaderFileReport.setDataPrototype ( "SgSourceFile*", "source_file", " = NULL",
                                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

#if 1
  // DQ (9/15/2018): Comment this out while we get the rest of the new IR nodes implementation into place.
     HeaderFileReport.setDataPrototype ( "SgSourceFilePtrList", "include_file_list", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
#endif



  // DQ (10/16/2005): Added to support C++ style argument handling in SgFile
  // File.setDataPrototype("std::list<std::string>","originalCommandLineArgumentList", "",
     File.setDataPrototype("SgStringList","originalCommandLineArgumentList", "",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

#if !ROSE_MICROSOFT_OS
// DQ (4/25/2009): Must fix code in sageInterface/sageBuilder.C before we can use the proper BUILD_LIST_ACCESS_FUNCTIONS macro above.
#warning "This should be using the BUILD_LIST_ACCESS_FUNCTIONS"
#endif
     File.isBoostSerializable(true);

  // Modified ROSE to hold variables into the File object
  // DQ (8/10/2004): modified to be an int instead of a bool
     File.setDataPrototype         ( "int", "verbose", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (12/8/2007): Added support to control warnings in front-end (specifically for Fortran support
  // to control use of warnings mode in syntax checking pass using gfortran prior to calling OFP).
     File.setDataPrototype         ( "bool", "output_warnings", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     File.setDataPrototype         ( "bool", "C_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "Cxx_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     File.setDataPrototype         ( "bool", "Fortran_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "CoArrayFortran_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     File.setDataPrototype         ( "int", "upc_threads", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (10/11/2010): Added initial Java support.
     File.setDataPrototype         ( "bool", "Java_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // X10 support
     File.setDataPrototype         ( "bool", "X10_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // PHP support
     File.setDataPrototype         ( "bool", "PHP_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Python support
     File.setDataPrototype         ( "bool", "Python_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // TV (05/17/2010) Cuda support
     File.setDataPrototype         ( "bool", "Cuda_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // TV (05/17/2010) OpenCL support
     File.setDataPrototype         ( "bool", "OpenCL_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (8/25/2017): Added more language support.
  // Csharp support
     File.setDataPrototype         ( "bool", "Csharp_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (8/25/2017): Added more language support.
  // Ada support
     File.setDataPrototype         ( "bool", "Ada_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (8/25/2017): Added more language support.
  // Jovial support
     File.setDataPrototype         ( "bool", "Jovial_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (8/25/2017): Added more language support.
  // Cobol support
     File.setDataPrototype         ( "bool", "Cobol_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (9/7/2018): By default for C/C++ I now think this should be false (and it is set this way for source files.
  // DQ (5/18/2008): Added flag to specify that CPP preprocessing is required (default true for C and C++, and
  // Fortran with *.F?? extension an explicitly set to false for fortran with *.f?? extension and binaries).
     File.setDataPrototype         ( "bool", "requires_C_preprocessor", "= true",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#if 1
  // DQ (2/5/2009): I think we need to make each file as binary or not and also record the setting on the command line.
  // DQ (2/4/2009): Moved this to the SgProject since it applies to the command line and all files.
  // DQ (1/9/2008): This permits a file to be marked explicitly as a binary file and avoids
  // confusion when processing object files within linking (where no source file is present
  // and the object file could be interpreted as being provided for binary analysis).
     File.setDataPrototype         ( "bool", "binary_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

  // DQ (8/19/2007): Added more options specific to Fortran support
  // File.setDataPrototype         ( "bool", "fixedFormat", "= false",
  //             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // File.setDataPrototype         ( "bool", "freeFormat", "= false",
  //             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "SgFile::outputFormatOption_enum", "inputFormat", "= SgFile::e_unknown_output_format",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "SgFile::outputFormatOption_enum", "outputFormat", "= SgFile::e_unknown_output_format",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "SgFile::outputFormatOption_enum", "backendCompileFormat", "= SgFile::e_unknown_output_format",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "fortran_implicit_none", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // Liao, 10/28/2008: Support for OpenMP 3.0 model for C/C++ pragma, will use it for Fortran later on, changed fortran_openmp to openmp
     File.setDataPrototype         ( "bool", "openmp", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // Liao, 5/31/2009: Only invoke the OpenMP C/C++ and Fortran directive parsers and generate OmpAttributes
     File.setDataPrototype         ( "bool", "openmp_parse_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // Only generate SgOmp* nodes from OmpAttributes in AST, no further actions
     File.setDataPrototype         ( "bool", "openmp_ast_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // Lowering OpenMP directives to code with explicit runtime calls
     File.setDataPrototype         ( "bool", "openmp_lowering", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "cray_pointer_support", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Liao, 1/30/2014: Support for FAIL-SAFE resilience pragma
     File.setDataPrototype         ( "bool", "failsafe", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (12/11/2007): Adds support for parser to output the parser rules to be called. For Fortran
  // support in ROSE this corresponds to the "--dump" option in the Open Fortran Parser (OFP).
  // There is no corresponding action with EDG for the C and C++ support in ROSE, so for C/C++
  // this option has no effect.
     File.setDataPrototype         ( "bool", "output_parser_actions", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "exit_after_parser", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "skip_syntax_check", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // TV (09/24/2018): Adding the ability to skip the parser when testing the backend with ROSE command line processing
     File.setDataPrototype         ( "bool", "skip_parser", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (4/7/2010): This permits less agressive syntax checking, but still some syntax checking.
  // Some F90 code cannot be passed through the gfortran syntax check using "-std=f95" so we have
  // to relax this to "-std=gnu" or skip the option altogether.
     File.setDataPrototype         ( "bool", "relax_syntax_check", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Operational options
  // File.setDataPrototype         ( "bool", "skip_rose", "= false",
  //             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/1/2011): Added support to just run the EDG front-end without translation of the EDG AST to the ROSE AST.
  // This is helpful in evaluating the EDG frontend within the move to EDG 4.3 and later versions (update for C++ robustness).
     File.setDataPrototype         ( "bool", "skip_translation_from_edg_ast_to_rose_ast", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     File.setDataPrototype         ( "bool", "skip_transformation", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "skip_unparse", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "skipfinalCompileStep", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Code generation options:
     File.setDataPrototype         ( "bool", "unparse_includes", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (12/4/2007): Added command-line support for unparsing #line directives (previously removed feature)
  // The use of #line directives permits the debugger to reference the original source code instead of
  // ROSE generated source code.
     File.setDataPrototype         ( "bool", "unparse_line_directives", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (4/14/2013): Added options to permit selection of either overloaded operator names or use of operator syntax 
  // for function calls in the unparsed code.  The default is to reproduce the same use as in the input code.
  // These options permit agressive levels of testing of both extremes (and overrides the default behavior to
  // reproduce the function call use as specified in the input code.
     File.setDataPrototype         ( "bool", "unparse_function_calls_using_operator_syntax", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "unparse_function_calls_using_operator_names", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (8/30/2008): Added support for tailoring the output of unparsed disassembled instructions
     File.setDataPrototype         ( "bool", "unparse_instruction_addresses", "= true",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "unparse_raw_memory_contents", "= true",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "unparse_binary_file_format", "= true",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (8/27/2007): Added support for simple translation using separately specified language unparser.
  // this is supported for testing alternative language code generation, not for a faithful translation
  // between languages (which would require translation of the AST to support langauge constructs not
  // shared between C/C++ and Fortran (for example).  This is also how the Promela support should be
  // provided, though this work only handles C (not C++, or Fortran) and only a subset of C, plus
  // numerous translations are required (See Christian Iwainsky's thesis).
  // File.setDataPrototype         ( "SgFile::outputLanguageOption_enum", "outputLanguage", "= SgFile::e_default_output_language",
  //                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "SgFile::languageOption_enum", "outputLanguage", "= SgFile::e_default_language",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (29/8/2017): Specification of input language (we will want to get this to match all of the get_Cxx_only flags and 
  // likely reimplement those functions to use this single enum). This is general work to support more languages.
     File.setDataPrototype         ( "SgFile::languageOption_enum", "inputLanguage", "= SgFile::e_default_language",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Internal data
  // File.setDataPrototype         ( "int"    , "numberOfSourceFileNames", "= -1",
  //             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // File.setDataPrototype         ( "char**" , "sourceFileNamesWithPath", "= NULL",
  //             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // File.setDataPrototype         ( "char**" , "sourceFileNamesWithoutPath", "= NULL",
  //             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "std::string" , "sourceFileNameWithPath", "= \"\"",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "std::string" , "sourceFileNameWithoutPath", "= \"\"",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // File.setDataPrototype         ( "bool", "skip_buildHigherLevelGrammars", "= false",
  //             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // File.setDataPrototype         ( "char*"  , "unparse_output_filename", "= NULL",
  //             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);
     File.setDataPrototype         ( "std::string"  , "unparse_output_filename", "= \"\"",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (4/17/2015): Adding support to save the original specification of the object file name from the command line (as specified using the "-o" option).
  // This is required for multiple file support which is using the "-o" option to specify the executable name when linking.  We have to split up the 
  // command so that we can call ROSE seperately with each file to generate a new source file and then call the backend compiler to generate the object 
  // file, and then call the linker seperately to using the object files to generate the named executable.
     File.setDataPrototype         ( "std::string" , "objectFileNameWithPath", "= \"\"",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "std::string" , "objectFileNameWithoutPath", "= \"\"",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (2/2/2003): Added to support -E and -H options (calling the backend directly)
     File.setDataPrototype         ( "bool", "useBackendOnly", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (2/12/2004): Added to support -c on compiler command line
     File.setDataPrototype("bool","compileOnly", "= false",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (2/13/2004): Added to support to save Edg command line
     File.setDataPrototype("std::string","savedFrontendCommandLine", "= \"\"",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (6/21/2005): Backend specific template option.  This might at a later date be abstracted out as a
  // separate class with  backend independent interface to hide the detils of backend specific (if this is
  // possible).  for th moment we include these options directly since g++ is the development platform for
  // ROSE.  Options supported in ROSE from g++ include:
  // -fno-implicit-templates        (suppress output of any but explicitly instantiated templates)
  // -fno-implicit-inline-templates (suppress output of any but explicitly instantiated inline templates)
  // -frepo                         (Enable automatic template instantiation)
     File.setDataPrototype("bool","no_implicit_templates", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype("bool","no_implicit_inline_templates", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // File.setDataPrototype("bool","repo", "= false",
  //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Turn this off to avoid error in copy function (strdup(char**) error)
  //   File.setAutomaticGenerationOfCopyFunction(false);

  // DQ (4/20/2006): Permit skipping all comments and CPP directives (provides faster processing)
  // But this can not be used if generating code to be compiled by the backend.
     File.setDataPrototype("bool","skip_commentsAndDirectives", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (4/19/2006): Added to permit optional collection of all comments from header files.
     File.setDataPrototype("bool","collectAllCommentsAndDirectives", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (3/24/2019): Adding an option to support addition of CPP directives to the AST.  Currently only restricted 
  // to CPP directives, and may be extended to comments later. Default is false to preserve original ROSE AST behavior.
     File.setDataPrototype( "bool", "translateCommentsAndDirectivesIntoAST", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // negara1 (07/08/2011): Added to permit optional header files unparsing.
     File.setDataPrototype("bool","unparseHeaderFiles", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (4/7/2001) Added support for multiple files (save the preprocessor
  //               directives and comments into the SgFile)
  //               the ROSEAttributesListContainer represents all preprocessor
  //               declarations and comments from each source file (and its
  //               corresponding include files) (thus a list of lists of lists
  //               of cpp directives and comments!!!)
     File.setDataPrototype         ( "ROSEAttributesListContainerPtr" , "preprocessorDirectivesAndCommentsList", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);

  // DQ (5/22/2005): Added support to record the file index to support multiple file handling
  // File.setDataPrototype("int","fileNameIndex", "= 0",
  //             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/1/2015): Build the access functions, but don't let the set_* access function set the "p_isModified" flag.
  // DQ (1/3/2006): Added attribute via ROSETTA (changed to pointer to AstAttributeMechanism)
  // Modified implementation to only be at specific IR nodes.
  // File.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
  //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     File.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
            NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);
     File.setFunctionPrototype      ( "HEADER_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
     File.setFunctionSource         ( "SOURCE_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");

  // Testing options
     File.setDataPrototype         ( "bool", "KCC_frontend", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "new_frontend", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

#if 0
  // DQ (10/26/2009): After discussion with Peter, this data member should be removed.
  // MS: DOT option flag (depricate this if it is not needed any more)
     File.setDataPrototype         ( "bool", "travTraceToDOT", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

  // Older options (will be depricated)
     File.setDataPrototype         ( "bool", "disable_edg_backend", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "disable_sage_backend", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "int"    , "testingLevel", "= -1",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "preinit_il", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "enable_cp_backend", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#if 0
  // DQ (10/26/2009): After discussion with Peter, this data member should be removed.
     File.setDataPrototype         ( "bool", "outputGrammarTreeFiles", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "outputGrammarTreeFilesForHeaderFiles", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (10/15/2001): added to make output of PDF version of EDG AST optional
  // (since they are very large and most often only the SAGE III version is wanted)
     File.setDataPrototype         ( "bool", "outputGrammarTreeFilesForEDG", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

  // DQ (8/8/2006): Permit marking of the generated source code with "#define ROSE_GENERATED_CODE"
  // option requested by Yarden at IBM.
     File.setDataPrototype         ( "bool", "markGeneratedFiles", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (9/19/2006): Allow testing of ROSE using input that is expected to fail (return passed if test failed)
     File.setDataPrototype         ( "bool", "negative_test", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (9/24/2006): Permit optional strict language handling (enforce ANSI.ISO standards)
     File.setDataPrototype         ( "bool", "strict_language_handling", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     // AS (9/29/2008): Added support for wave on command line

     File.setDataPrototype         ( "bool", "wave", "= false",
         NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);




  // DQ (10/31/2006): Support for embedded color codes in the generated code from ROSE.
  // Different values trigger embedded codes for different types of internal data
  // (missing information, compiler generated code, etc.).
     File.setDataPrototype         ( "int", "embedColorCodesInGeneratedCode", "= 0",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (10/31/2006): Support for output of source position information for visualization
  // of source position information (debugging support). different values report source
  // position information about different sorts of languge constructs, statements,
  // expressions, types, etc.
     File.setDataPrototype         ( "int", "generateSourcePositionCodes", "= 0",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/29/2006): Support for use of __cplusplus macro for C mode using source file with C++ filename extension.
  // File.setDataPrototype         ( "bool", "requires_cplusplus_macro", "= false",
  //             CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "sourceFileUsesCppFileExtension", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (8/11/2007): Support for Fortran and its flavors
     File.setDataPrototype         ( "bool", "sourceFileUsesFortranFileExtension", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "sourceFileUsesFortran77FileExtension", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "sourceFileUsesFortran90FileExtension", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "sourceFileUsesFortran95FileExtension", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "sourceFileUsesFortran2003FileExtension", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (1/25/2016): Added initial support in command line handling for Fortran 2008 files.
     File.setDataPrototype         ( "bool", "sourceFileUsesFortran2008FileExtension", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     File.setDataPrototype         ( "bool", "sourceFileUsesCoArrayFortranFileExtension", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "sourceFileUsesPHPFileExtension", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "sourceFileUsesPythonFileExtension", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "sourceFileUsesJavaFileExtension", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "sourceFileUsesBinaryFileExtension", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "sourceFileTypeIsUnknown", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "sourceFileUsesX10FileExtension", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (28/8/2017): Adding language support.
     File.setDataPrototype         ( "bool", "sourceFileUsesCsharpFileExtension", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "sourceFileUsesAdaFileExtension", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "sourceFileUsesJovialFileExtension", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "sourceFileUsesCobolFileExtension", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (9/26/2011): Added support to detect dangling pointers in ROSE translators.
  // This is not an expensive test, but it fails for isolated parts of ROSE currently 
  // so it should be made optional at this early stage (before it is made a default 
  // test in the AST consistency tests).
     File.setDataPrototype         ( "int", "detect_dangling_pointers", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (6/7/2013): Added support for use of experimental fortran front-end.
     File.setDataPrototype("bool", "experimental_fortran_frontend", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Rasmussen (8/30/2010): Added experimental support for using the Flang parser for Fortran
     File.setDataPrototype("bool", "experimental_flang_frontend", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Rasmussen (3/12/2018): Added support for CUDA Fortran within the experimental fortran frontend.
     File.setDataPrototype("bool", "experimental_cuda_fortran_frontend", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (1/23/2016): Added support for OFP parsing and pretty printing of generated Aterm
  // (this is part of the internal testing of the new (experimental) Fortran support).
     File.setDataPrototype("bool", "experimental_fortran_frontend_OFP_test", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // To be consistent with the use of binaryFile we will implement get_binaryFile() and set_binaryFile()
  // functions so that we can support the more common (previous) interface where there was only a single
  // SgAsmFile pointers called "binaryFile".
     BinaryComposite.setDataPrototype("SgAsmGenericFileList*", "genericFileList", "= NULL",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     BinaryComposite.setDataPrototype("SgAsmInterpretationList*","interpretations","= NULL",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (11/5/2008): This should maybe be added to the SgAsmGenericFile instead of the SgBinaryFile, if so
  // we will move it.  For now we can't add it to SgAsmGenericFile becuase we could not traverse both a
  // list and a data member in the definition of an AST traversal.
  // BinaryFile.setDataPrototype("SgAsmDwarfCompilationUnit*","dwarf_info","= NULL",
  //             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (8/16/2008): parse binary executable file format only (some uses of ROSE may only do analysis of
  // the binary executable file format and not the instructions).  This is also useful for testing.
     File.setDataPrototype         ( "bool", "read_executable_file_format_only", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/9/2008): parse binary executable file format only (but skip the representation of
  // symbols since there can be thousands of them and they make the dot graphs impossible to manage).
     File.setDataPrototype         ( "bool", "visualize_executable_file_format_skip_symbols", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/9/2008): parse binary executable file format only (but skip the representation of
  // symbols since there can be thousands of them and they make the dot graphs impossible to manage).
     File.setDataPrototype         ( "bool", "visualize_dwarf_only", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (8/26/2008): Adds support for only disassembling the instructions, skips use of information
  // gathered from the data structures built from the binary executable file format (symbols,
  // section permisions, etc.).
     File.setDataPrototype         ( "bool", "read_instructions_only", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (5/23/2015): This must be static because ASM statments can exist in GNU statement expressions 
  // within typeof operators which then causes the ASM statement to not be traversed as part of the 
  // AST (beccause it is hidden in a type (and types are not traversed).  The unparsing of the ASM 
  // statement checks this flag (skip_unparse_asm_commands) since unparsing of ASM is architecture
  // dependent and a special problem for the portability of the ROSE regression tests (e.g. on older
  // versions of MAC OS which were non-x86). The solution is to make this a static boolean flag so
  // that we need not find the SgFile object via a traversal upwards in the AST through the parent 
  // pointers.
  // DQ (1/10/2009): The C language ASM statements are providing significant trouble, they are
  // frequently machine specific and we are compiling then on architectures for which they were
  // not designed.  This option allows then to be read, constructed in the AST to support analysis
  // but not unparsed in the code given to the backend compiler, since this can fail. (See
  // test2007_20.C from Linux Kernel for an example).
  // File.setDataPrototype         ( "bool", "skip_unparse_asm_commands", "= false",
  //             NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "static bool", "skip_unparse_asm_commands", "= false",
                 NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (2/3/2009): For a library archive, these are the name of the object files it contains.
  // This information is obtained via "ar -vox <archive>", and saving and reading the list.
     File.setDataPrototype("SgStringList","libraryArchiveObjectFileNameList", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (2/3/2009): added boolean data member to record if this is a library archive.
     File.setDataPrototype         ( "bool", "isLibraryArchive", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (2/5/2009): added boolean data member to record if this is an object file being processed for binary analysis.
     File.setDataPrototype         ( "bool", "isObjectFile", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/20/2010): Unparse using tokens were possible instead of from the AST.  This is possible
  // where transformations have not been done (i.e. where the AST has not be modified).  Where this
  // is possible the generated code is fundamentally more portable and can be diff'ed against to 
  // generate patches (useful on large scale projects to design new tools).
     File.setDataPrototype ("bool", "unparse_tokens", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (1/30/2014): Support testing of unparsing using tokens with synthetic marking of 
  // statements to be transformations.  This is used the test the transition steps between
  // unparsing from the AST vs. the token stream.
     File.setDataPrototype ("int", "unparse_tokens_testing", "= 0",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/12/2014): Added to support testing of the unparsing using token streams. This option
  // forces the output of two files representing the unparsing of each statement using the tken stream
  // mapping to the AST.  The token_leading_* file uses the mapping and the leading whitespace 
  // mapping between statements, where as the token_trailing_* file uses the mapping and the trailing 
  // whitespace mapping between statements.  Both files should be identical, and the same as the 
  // input file.  This option was previously internally activated when the verbose level was non-zero,
  // but this was not a good long-term approach.
     File.setDataPrototype ("bool", "unparse_using_leading_and_trailing_token_mappings", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Liao (12/15/2016): Unparse template from its AST.
  // By default, the original string stored by EDG is used to output template AST
     File.setDataPrototype ("bool", "unparse_template_ast", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

#if 1
  // DQ (2/17/2013): Added support to skip AST consistancy testing AstTests::runAllTests(SgProject*)
  // This testing is useful but interferes with performance testing using HPCToolKit.
  // Note that the AstTests::runAllTests() function must be called explicitly in the user's translator.
     File.setDataPrototype("bool", "skipAstConsistancyTests", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

  // DQ (4/28/2014): This might be improved it it were moved to the translator directly.  The result
  // would be the demonstration of a more general mechansim requireing no modification to ROSE directly.
  // DQ (4/28/2014): Added support for shared keyword as extension to C (implemented as embedded DSL 
  // within UPC base language, this serves as an example of what is required to support a simple DSL 
  // in ROSE).  In general we would need a more flexible mechanism than adding a flag to ROSE.
  // File.setDataPrototype ("bool", "shared_memory_dsl", "= false",
  //             NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (4/17/2015): Adding multifile handling support for commandline generation.
     File.setDataPrototype ("bool", "multifile_support", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (5/24/2015): Record if optimization is specified on the command line (later maybe also save what level).
  // This is required to set the __OPTIMIZED__ macro (to follow the GNU API).  See test2015_153.c.
     File.setDataPrototype("bool", "optimization", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (12/11/2015): Use the token stream to improve source position information.
  // Some source position information is unavailable in EDG, e.g. the end of most 
  // secondary declarations, for loop initialization statements, etc.  This is part
  // of a currently experimental mechanism to improve the source positon information 
  // in the AST using local searches of the token stream as a part of the token stream 
  // mapping to the AST (as used in the token-based unparsing).
     File.setDataPrototype ("bool", "use_token_stream_to_improve_source_position_info", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (12/23/2015): Adding optional support for variables to be in the same variable declaration 
  // (optionally eding a long standing ROSE specific normalization that we would like to move away 
  // from longer term). Note that this is already addressed in Fortran (not an option to have 
  // variable declarations normalized), but not it has not be even an option in C/C++ until now.
  // File.setDataPrototype ("bool", "allow_multiple_names_in_variable_declarations", "= false",
  //             NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype ("bool", "suppress_variable_declaration_normalization", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // TV (04/11/2018): Whether or not to generate a graphviz representation of EDG internal representation
     File.setDataPrototype("bool", "edg_il_to_graphviz", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // TV (10/01/2018): ROSE-1424
     File.setDataPrototype("bool", "no_optimize_flag_for_frontend", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // TV (10/08/2018): ROSE-1392
     File.setDataPrototype("bool", "unparse_edg_normalized_method_ROSE_1392", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // ******************************************************************************
  // ******************************************************************************
  //                             ROSE Graph IR Nodes
  // ******************************************************************************
  // ******************************************************************************


// DQ (4/30/2009): Redesigned the Graph IR node support in ROSE.

// Todo: Turn off the suport backward compatability
// Todo: Remove the AstAttributeMechanism* from SgGraphNode and SgGraphEdge
//       in favor of the same API, but implemented using a map of maps at
//       the SgGraph indexed by integer values.

// DQ (5/2/2009): This is put into rose_paths.h
// the warning macro is causing problems in Windows
#if !_MSC_VER
#ifndef ROSE_USING_GRAPH_IR_NODES_FOR_BACKWARD_COMPATABILITY
#warning "ROSE_USING_GRAPH_IR_NODES_FOR_BACKWARD_COMPATABILITY not set"
#endif
#endif
  // ******************************************************************************
  //                                SgGraphNode
  // ******************************************************************************

  // A node contains an index, name, and a pointer to an associated IR node in the
  // AST (approximately 8 words; 4 from SgNode).

     GraphNode.setFunctionPrototype ( "HEADER_GRAPHNODE", "../Grammar/Support.code");

  // An alternative might be to keep a hash_map<int,string> in the SgGraph (to keep the size small).
     GraphNode.setDataPrototype("std::string","name","= \"\"",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // This provides a link between graph nodes and AST nodes.
  // DQ (4/25/2009): The variable name should not also be the name of a type (allowed in C++ but bad style).
     GraphNode.setDataPrototype("SgNode*","SgNode","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (4/29/2009): Added storage of index as explicit value to enumerate the nodes.
     GraphNode.setDataPrototype("int","index","= -1",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // Internal static counter used to initialize the index value in each node.
     GraphNode.setDataPrototype("static int","index_counter","",
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/1/2015): Build the access functions, but don't let the set_* access function set the "p_isModified" flag.
  // This could be defined using a map of node attributes in the SgGraph class.
  // GraphNode.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     GraphNode.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);

     GraphNode.setFunctionPrototype ( "HEADER_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
     GraphNode.setFunctionSource    ( "SOURCE_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");


  // tps : TODO : This will be eliminated because it is redundant with the AstAttributeMechanism.
     GraphNode.setDataPrototype("std::map<int, std::string>","properties", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


  // ******************************************************************************
  //                                SgGraphEdge
  // ******************************************************************************

  // An edge contains an index, a name, and two SgGraphNode pointers (approximately
  // 9 words; 4 from SgNode).

     GraphEdge.setFunctionPrototype ( "HEADER_GRAPHEDGE", "../Grammar/Support.code");
     GraphEdge.setDataPrototype("SgGraphNode*","node_A","= NULL",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     GraphEdge.setDataPrototype("SgGraphNode*","node_B","= NULL",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     GraphEdge.setDataPrototype("std::string","name","= \"\"",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (4/29/2009): Added storage of index as explicit value to enumerate the edges.
     GraphEdge.setDataPrototype("int","index","= -1",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Internal static counter used to initialize the index value in each node.
     GraphEdge.setDataPrototype("static int","index_counter","",
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/1/2015): Build the access functions, but don't let the set_* access function set the "p_isModified" flag.
  // This could be defined using a map of edge attributes in the SgGraph class.
  // GraphEdge.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     GraphEdge.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);
     GraphEdge.setFunctionPrototype ( "HEADER_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
     GraphEdge.setFunctionSource    ( "SOURCE_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");

  // DQ (8/18/2008): NOTE: "SgIntegerStringMapPtrList" does not cause ROSETTA to generate the
  // correct code, where as "std::map<int, std::string>" appears to work better.
  // GraphEdge.setDataPrototype("std::string","type","= \"\"",
  //                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // GraphEdge.setDataPrototype("int","graph_id", "= 0",
  //                       CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // GraphEdge.setDataPrototype("std::map<int, std::string>","properties", "",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // GraphEdge.setDataPrototype("SgIntegerStringMapPtrList","properties", "",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


     // tps : todo : remove me
     GraphEdge.setDataPrototype("std::map<int, std::string>","properties", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


  // ******************************************************************************
  //                      Edge Types derived from SgGraphEdge
  // ******************************************************************************

     DirectedGraphEdge.setFunctionPrototype   ( "HEADER_DIRECTED_GRAPH_EDGE"  , "../Grammar/Support.code");
     UndirectedGraphEdge.setFunctionPrototype ( "HEADER_UNDIRECTED_GRAPH_EDGE", "../Grammar/Support.code");



  // These IR nodes might be eliminated in favor of just a function to generate the same level 
  // of support using a combination of two maps in SgGraph.

#ifdef ROSE_USING_GRAPH_IR_NODES_FOR_BACKWARD_COMPATABILITY
     GraphNodeList.setFunctionPrototype ( "HEADER_GRAPH_NODE_LIST", "../Grammar/Support.code");
     GraphNodeList.setDataPrototype("rose_graph_hash_multimap","nodes","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     GraphEdgeList.setFunctionPrototype ( "HEADER_GRAPH_EDGE_LIST", "../Grammar/Support.code");
     GraphEdgeList.setDataPrototype("rose_graph_node_edge_hash_multimap","edges","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif


  // ******************************************************************************
  //                                  SgGraph
  // ******************************************************************************

     Graph.setPredeclarationString ("HEADER_GRAPH_PREDECLARATION"  , "../Grammar/Support.code");
     Graph.setPostdeclarationString ("HEADER_GRAPH_POSTDECLARATION", "../Grammar/Support.code");

     Graph.setFunctionPrototype ( "HEADER_GRAPH", "../Grammar/Support.code");
     Graph.setDataPrototype("std::string","name","= \"\"",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     Graph.setDataPrototype("rose_graph_integer_node_hash_map","node_index_to_node_map","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Graph.setDataPrototype("rose_graph_integer_edge_hash_map","edge_index_to_edge_map","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Graph.setDataPrototype("rose_graph_integerpair_edge_hash_multimap","node_index_pair_to_edge_multimap","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Graph.setDataPrototype("rose_graph_string_integer_hash_multimap","string_to_node_index_multimap","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Graph.setDataPrototype("rose_graph_string_integer_hash_multimap","string_to_edge_index_multimap","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (5/3/2009): Added support for multimap access to edges using a specific map (index --> SgGraphEdge*).
     Graph.setDataPrototype("rose_graph_integer_edge_hash_multimap","node_index_to_edge_multimap","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (4/29/2009): Added storage of index as explicit value to enumerate the graph objects.
  // Graph.setDataPrototype("int","graph_id", "= -1",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Graph.setDataPrototype("int","index","= -1",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // Internal static counter used to initialize the index value in each node.
     Graph.setDataPrototype("static int","index_counter","",
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // These are Boost specific data members used to return results from Boost Graph Library algorithms.
  // These will likely be removed since they can and should be put into the API for the graph algorithms.
  // DQ (4/29/2009): Added boost specific data structures required by many boost graph algorithms.
     Graph.setDataPrototype("SgBoostEdgeList","boost_edges","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Graph.setDataPrototype("SgBoostEdgeWeightList","boost_edge_weights","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/1/2015): Build the access functions, but don't let the set_* access function set the "p_isModified" flag.
  // Attribute support for SgGraph
  // Graph.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     Graph.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);

#if 0
  // DQ (5/1/2009): Use this in the future instead of the AstAttributeMechanism* on the nodes and edge classes.
  // Use a map of integer index values to AstAttributeMechanism* so that we can avoid putting an std::map
  // onto each SgGraphNode and SgGraphEdge (this reduces the size of the SgGraphNode and SgGraphEdge class;
  // making it more space efficent for large graphs).
     Graph.setDataPrototype("std::map<int,AstAttributeMechanism*>","nodeAttributeMechanism","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     Graph.setDataPrototype("std::map<int,AstAttributeMechanism*>","edgeAttributeMechanism","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
#endif

     // tps : todo : remove me
     Graph.setDataPrototype("std::map<int, std::string>","properties", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     Graph.setFunctionPrototype ( "HEADER_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
     Graph.setFunctionSource    ( "SOURCE_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");


  // ******************************************************************************
  //                          Graphs derived from SgGraph
  // ******************************************************************************

     IncidenceUndirectedGraph.setFunctionPrototype ( "HEADER_INCIDENCE_UNDIRECTED_GRAPH", "../Grammar/Support.code");
     IncidenceUndirectedGraph.setFunctionSource    ( "SOURCE_INCIDENCE_UNDIRECTED_GRAPH", "../Grammar/Support.code");

     IncidenceDirectedGraph.setFunctionPrototype ( "HEADER_INCIDENCE_DIRECTED_GRAPH", "../Grammar/Support.code");
     IncidenceDirectedGraph.setFunctionSource    ( "SOURCE_INCIDENCE_DIRECTED_GRAPH", "../Grammar/Support.code");


  // This is the more uniform integer to SgGraphEdge version of rose_graph_node_edge_hash_multimap.
     IncidenceDirectedGraph.setDataPrototype("rose_graph_integer_edge_hash_multimap","node_index_to_edge_multimap_edgesOut","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


  // This is the more uniform integer to SgGraphEdge version of rose_graph_node_edge_hash_multimap.
  // The derived class "BidirectionalGraph" might use this map differently (with different semantics).
     IncidenceDirectedGraph.setDataPrototype("rose_graph_integer_edge_hash_multimap","node_index_to_edge_multimap_edgesIn","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


  // DQ (5/3/2009): Not clear if this should add a new hash_multimap or if it just chages the
  // semantics of the hash_multimaps in the dirived class (IncidenceDirectedGraph). This class
  // should have a different API that make is specific to bidirectional incidence graph use.
     BidirectionalGraph.setFunctionPrototype ( "HEADER_BIDIRECTIONAL_GRAPH", "../Grammar/Support.code");

#if 0
  // DQ (5/3/2009): Base class IncidenceDirectedGraph should use rose_graph_integer_edge_hash_multimap
  // but perhaps in a different way (with different semantics).
  // DQ (4/30/2009): Removed these in favor of the hash_multimap using the SgGraphEdge class.
     BidirectionalGraph.setDataPrototype("rose_directed_graph_hash_multimap","edgesIn","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif


#if 0
#if 1
  // DQ (4/25/2009): For now leave this as a map<int,string> but we want it to be a map<int,SgGraphNode*> later...
     IntKeyedBidirectionalGraph.setDataPrototype("std::map<int, std::string>","edgeMap","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#else
     IntKeyedBidirectionalGraph.setDataPrototype("std::map<int, SgGraphNode*>","edgesMap","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif
#endif





  // ******************************************************************
  // ************************* Project IR Node ************************
  // ******************************************************************

#if ROSE_USING_OLD_PROJECT_FILE_LIST_SUPPORT
  // Old functionality
  // DQ (4/7/2001) Added support for multiple files (changed SgFile* to SgFilePtrListPtr*)
  // Project.setDataPrototype ( "SgFile*"  , "file", "= NULL" );
     Project.setDataPrototype ( "SgFilePtrList"  , "fileList", "= SgFilePtrList()" ,
                           CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#else
  // DQ (1/20/2010): Change to provide uniformity in ROSE for how SgFile is handled (just like any other IR node).
  // This avoids was was previously references that we returned and allows the ROSETTA rules to be
  // followed that avoid having more than one list or mixing lists with data members.
  // SgFileList is an IR node and this is a data member pointer of that type.
     Project.setDataPrototype ( "SgFileList*"  , "fileList_ptr", "= NULL" ,
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#if 0
  // DQ (10/16/2005): Removed because they are not needed except as a local variable in the
  // command line processing function.
     // MK: store the command line data associated with the file (or is it the argv for the whole project?)
     Project.setDataPrototype("int","numberOfCommandLineArguments", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // MK: store the command line data associated with the file (or is it the argv for the whole project?)
     Project.setDataPrototype("char**","commandLineArgumentList", "= NULL",
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Project.setDataPrototype("int","defaultNumberOfCommandLineArguments", "= 0",
  //        NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // Project.setDataPrototype("char**","defaultCommandLineArgumentList", "= NULL",
  //        NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

  // Project.setDataPrototype("std::list<std::string>","originalCommandLineArgumentList", "",
     Project.setDataPrototype("SgStringList","originalCommandLineArgumentList", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

#if !ROSE_MICROSOFT_OS
// DQ (4/25/2009): Must fix code in sageInterface/sageBuilder.C before we can use the proper BUILD_LIST_ACCESS_FUNCTIONS macro above.
#warning "This should be using the BUILD_LIST_ACCESS_FUNCTIONS"
#endif

     Project.setDataPrototype("int","frontendErrorCode", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype("int","javacErrorCode", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype("int","ecjErrorCode", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype("int","midendErrorCode", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (11/1/2015): Build the access functions, but don't let the set_* access function set the "p_isModified" flag.
  // Project.setDataPrototype("int","backendErrorCode", "= 0",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype("int","backendErrorCode", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     File.setDataPrototype("int","frontendErrorCode", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype("int","javacErrorCode", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype("int","ecjErrorCode", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  //Liao 4/26/2017, enable exception/signal handling of midend processing of each file   
     File.setDataPrototype("int","midendErrorCode", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype("int","unparserErrorCode", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/1/2015): Build the access functions, but don't let the set_* access function set the "p_isModified" flag.
  // File.setDataPrototype("int","backendCompilerErrorCode", "= 0",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype("int","backendCompilerErrorCode", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     File.setDataPrototype("bool","unparsedFileFailedCompilation", "= false",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // TOO1 (03/05/2013): Ignore errors (similar to GNU Make's --keep-going), and
  //                    simply compile the original input code as a failover.
     Project.setDataPrototype("bool", "keep_going", "= false",
                              NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE); 

  // TOO1 (03/20/2014): Dangerous rope for Pontetec, -rose:unparser:clobber_input_file
     Project.setDataPrototype      ( "bool", "unparser__clobber_input_file", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


     Project.setDataPrototype("std::string","outputFileName", "= \"\"",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#if 0
     Project.setDataPrototype("std::list<std::string>","sourceFileNameList", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype("std::list<std::string>","objectFileNameList", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype("std::list<std::string>","libraryFileList", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype("std::list<std::string>","librarySpecifierList", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype("std::list<std::string>","libraryDirectorySpecifierList", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype("std::list<std::string>","includeDirectorySpecifierList", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#else
     Project.setDataPrototype("SgStringList","sourceFileNameList", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype("SgStringList","objectFileNameList", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype("SgStringList","libraryFileList", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype("SgStringList","librarySpecifierList", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype("SgStringList","libraryDirectorySpecifierList", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (10/26/2009): After discussion with Peter, this data member should be moved to the SgFile (or SgSourceFile) object.
     Project.setDataPrototype("SgStringList","includeDirectorySpecifierList", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (10/18/2010): Added support to extract "-D" options so that for Fortran we could process them separately
  // by triggering the use of the C preprocessor and passing them to CPP explicitly.
     Project.setDataPrototype("SgStringList","macroSpecifierList", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (10/26/2009): After discussion with Peter, this data member should be moved to the SgFile (or SgSourceFile) object.
  // DQ (1/13/2009): Added support for GNU -include (for pre inclusion of files) and -isystem
  // (for preinclusion of directories to be searched) options.
     Project.setDataPrototype("SgStringList","preincludeFileList", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype("SgStringList","preincludeDirectoryList", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

  // DQ (2/12/2004): Added to support -c on compiler command line
     Project.setDataPrototype("bool","compileOnly", "= false",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // AS (9/29/2008): Added support for wave on command line
     Project.setDataPrototype("bool","wave", "= false",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


  // Project.setDataPrototype("bool","linkOnly", "= false",
  //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (2/21/2004): Added to support prelinker on compiler command line
     Project.setDataPrototype("bool","prelink", "= false",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (5/20/2005): Added to support template instantiation control from command line
     Project.setDataPrototype("SgProject::template_instantiation_enum","template_instantiation_mode", "= SgProject::e_default",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (7/7/2005): Added to support AST merging (specified using several parameters).
  // A specified file records the working directory and the commandline for later execution.
     Project.setDataPrototype("bool","astMerge", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype("std::string","astMergeCommandFile", "= \"\"",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Milind Chabbi (9/9/2013): Added a commandline option to use a file to generate persistent id for files
  // used in different compilation units.
     Project.setDataPrototype("std::string","projectSpecificDatabaseFile", "= \"\"",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (10/16/2005): Added support to detect use of "-E" on compiler's command line.
  // Special processing is done when "-E" is used with multiple file names on the command line.
     Project.setDataPrototype         ( "bool", "C_PreprocessorOnly", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

#if 1
  // DQ (11/1/2015): Build the access functions, but don't let the set_* access function set the "p_isModified" flag.
  // DQ (1/3/2006): Added attribute via ROSETTA (changed to pointer to AstAttributeMechanism)
  // Modified implementation to only be at specific IR nodes.
  // Project.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
  //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     Project.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
            NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);
     Project.setFunctionPrototype      ( "HEADER_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
     Project.setFunctionSource         ( "SOURCE_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
#endif

  // DQ (8/29/2006): Support for CSV data file reporting performance of compilation.
  // This file accumulates information (if specified) and permits plots of performance
  // for different parts of the compilation internally within ROSE.
     Project.setDataPrototype("std::string","compilationPerformanceFile", "= \"\"",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (1/16/2008): Added include/exclude path lists for use internally by translators.
  // For example in Compass this is the basis of a mechanism to exclude processing of
  // header files from specific directorys (where messages about the properties of the
  // code there are not meaningful since they cannot be changed by the user). These
  // are ment to be used by ROSE translators and as a result have unspecificed semantics
  // in ROSE, it is just a common requirement of many uses of ROSE.
  // Project.setDataPrototype("bool","lookForIncludePaths", "= false",
  //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // Project.setDataPrototype("bool","lookForExcludePaths", "= false",
  //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype("SgStringList","includePathList", "",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype("SgStringList","excludePathList", "",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype("SgStringList","includeFileList", "",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype("SgStringList","excludeFileList", "",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#if 1
  // DQ (2/4/2009): Moved this to the SgProject since it applies to the command line and all files.
  // DQ (1/9/2008): This permits a file to be marked explicitly as a binary file and avoids
  // confusion when processing object files within linking (where no source file is present
  // and the object file could be interpreted as being provided for binary analysis).
     Project.setDataPrototype ( "bool", "binary_only", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

  // DQ (5/1/2009): Requested feature by Andreas (controls use of SQLite database for analysis).
  // This permits multiple files to be handled separately in that program analysis which supports it.
     Project.setDataPrototype("std::string","dataBaseFilename", "= \"\"",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

#if 1
  // DQ (1/20/2010): Added list of directories to the SgProject to represent large scale projects
  // (as can be useful for code generation).
  // Note that this is marked as NO_TRAVERSAL because it will otherwise interfere with "SgFilePtrList fileList"
  // This can be changed later, but it could effect a number of interfaces internally, so don't change it yet.
     Project.setDataPrototype ("SgDirectoryList*", "directoryList", "= NULL",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

  // DQ (4/7/2010): Support for C, C++, and Fortran. The SgProject needs this state so that
  // it can save information from the command line when there are no files such as when it 
  // is used for linking (where there is just a list of object files).
     Project.setDataPrototype ( "bool", "C_only", "= false",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype ( "bool", "Cxx_only", "= false",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (7/21/2012): Added support for C11, C++0x and C++11 from the command line.
     Project.setDataPrototype ( "bool", "C11_only", "= false",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype ( "bool", "Cxx0x_only", "= false",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype ( "bool", "Cxx11_only", "= false",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (4/20/2014): Adding C14 and C++14 support.
     Project.setDataPrototype ( "bool", "C14_only", "= false",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype ( "bool", "Cxx14_only", "= false",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     Project.setDataPrototype ( "bool", "Fortran_only", "= false",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     Project.setDataPrototype ("std::list<std::string>", "Fortran_ofp_jvm_options", "",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (10/11/2010): Added initial Java support.
     Project.setDataPrototype ( "bool", "Java_only", "= false",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     Project.setDataPrototype ( "bool", "X10_only", "= false",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Liao 6/29/2012: support using rose translator to act like a linker wrapper if -rose:openmp:lowering is specified
  // Add a flag to indicate if this is a linking involved in the lowered OpenMP input program.
  // The openmp_lowering flag at SgFile level won't exist when there are no input source files at the linking stage
  //
  // Simply using OpenMP_Only is not sufficient for OpenMP since OpenMP input files can be handled in three different ways in ROSE:
  // 1) parsing only 2) generating dedicated AST 3) actual OpenMP lowering
  // Only the one with lowering will need special linking support to connect to libxomp.a and pthreads.
     Project.setDataPrototype ( "bool", "openmp_linking", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     Project.setDataPrototype ("std::list<std::string>", "Java_ecj_jvm_options", "",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     Project.setDataPrototype ("bool", "Java_batch_mode", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     Project.setDataPrototype ("std::list<std::string>", "Java_classpath", "",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype ("std::list<std::string>", "Java_sourcepath", "",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype("std::string", "Java_destdir", "= Rose::getWorkingDirectory()",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype("std::string", "Java_source_destdir", "= Rose::getWorkingDirectory()",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype ("std::string", "Java_s", "",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype ("std::string", "Java_source", "",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype ("std::string", "Java_target", "",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype ("std::string", "Java_encoding", "",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype ("std::string", "Java_g", "",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype ("bool", "Java_nowarn", "",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype ("bool", "Java_verbose", "",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype ("bool", "Java_deprecation", "",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype ("std::list<std::string>", "Java_bootclasspath", "",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (10/3/2010): Adding support for having CPP directives explicitly in the AST (as IR nodes instead of handled similar to comments).
     Project.setDataPrototype ( "bool", "addCppDirectivesToAST", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // negara1 (06/23/2011): This field tracks for each included file its including preprocessing infos.
     Project.setDataPrototype("std::map<std::string, std::set<PreprocessingInfo*> >", "includingPreprocessingInfosMap", "",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // negara1 (07/29/2011): The following two fields track include paths that compiler searches for quoted and bracketed includes correspondingly.
     Project.setDataPrototype("std::list<std::string>", "quotedIncludesSearchPaths", "",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     Project.setDataPrototype("std::list<std::string>", "bracketedIncludesSearchPaths", "",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // negara1 (08/17/2011): Added to permit specifying an optional root folder for header files unparsing.
     Project.setDataPrototype("std::string", "unparseHeaderFilesRootFolder", "= \"\"",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (9/18/2011): Added support for specification of frontend constant folding (only supported in C/C++ using EDG).
     Project.setDataPrototype("bool", "frontendConstantFolding", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

#if 0
  // DQ (2/17/2013): Added support to skip AST consistancy testing AstTests::runAllTests(SgProject*)
  // This testing is useful but interferes with performance testing using HPCToolKit.
  // Note that the AstTests::runAllTests() function must be called explicitly in the user's translator.
     Project.setDataPrototype("bool", "skipAstConsistancyTests", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

  // DQ (6/12/2013): Added Java support for a global scope spanning files. Might be 
  // useful in the future for multiple file handling in other languages.
     Project.setDataPrototype("SgGlobal*", "globalScopeAcrossFiles", "= NULL",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);

  // DQ (9/14/2013): Adding option to copy the location of the input file as the position for the generated output file.
  // This is now demonstrated to be important in the case of ffmpeg-1.2 for the file "file.c" where it is specified as
  // "libavutil/file.c" on the command line and we by default put it into the current directory (top level directory 
  // in the directory structure).  But it is a subtle and difficult to reproduce error that the generated file will
  // not compile properly from the top level directory (even when the "-I<absolute path>/libavutil" is specified).
  // We need an option to put the generated file back into the original directory where the input source files is
  // located, so that when the generated rose_*.c file is compiled (with the backend compiler, e.g. gcc) it can use
  // the identical rules for resolving head files as it would have for the original input file (had it been compiled
  // using the backend compiler instead).
  // DQ (9/16/2013): Changed name from "build_generated_file_in_same_directory_as_input_file" to "unparse_in_same_directory_as_input_file".
     Project.setDataPrototype("bool", "unparse_in_same_directory_as_input_file", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (1/19/2014): This option "-S" is required for some build systems (e.g. valgrind).
     Project.setDataPrototype("bool", "stop_after_compilation_do_not_assemble_file", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (1/20/2014): This option "-u" or "-undefined" is required for some build systems (e.g. valgrind).
     Project.setDataPrototype("std::string", "gnuOptionForUndefinedSymbol", "= \"\"",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (1/20/2014): This option "-m32" over-rides the 64-bit mode on 64-bit environments and is required for some build systems (e.g. valgrind).
     Project.setDataPrototype("bool", "mode_32_bit", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (3/19/2014): This option -rose:noclobber causes the output of source code to an existing file to be an error.
     Project.setDataPrototype("bool", "noclobber_output_file", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (3/19/2014): This option -rose:noclobber causes the output of source code to an existing file to be an error if it results in a different file.
  // Output of an identifal file is not an error.
     Project.setDataPrototype("bool", "noclobber_if_different_output_file", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (1/31/2014): Added optimization for a few wireshark files. This post-processing is 
  // not required for C, but is only usefule for C++ (name qualification).  Still it is 
  // only a performance problem on a handfull of files in wireshark application. So we
  // need to provide an option to supress.  This is currently being investigated further.
  // not that this option is inconsistant with OpenMP usage, but not a problem for any
  // other C code (the C++ aspect of this could also be fixed). This is related to a 
  // problem when the original expression trees were a part of the AST traversal, but 
  // this issue was fixed in 2012 (or so). There is as a result, less need for this
  // post-processing phase, except that it appears to be relied upon by the OpenMP and 
  // C++ support (both of which can be fixed).
     Project.setDataPrototype("bool", "suppressConstantFoldingPostProcessing", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (9/8/2016): Adding support to optionally unparse template declarations from the AST 
  // (instead of from a saved string available from the EDG frontend). This option is false
  // by default until we verifiy the we can support unparsing templates from the AST.  When
  // true, this option permits transforamtions on the AST representing template declarations 
  // to be output in the generated source code from ROSE.  Until thenm user defined transformations 
  // of AST representing the template declarations can be done, howevr only the original (as 
  // normalized by EDG) string representation of the template will be unparsed by ROSE.
     Project.setDataPrototype("bool", "unparseTemplateDeclarationsFromAST", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Pei-Hung (8/6/2014): This option -rose:appendPID appends PID into the temporary output name to avoid issues in parallel compilation. 
     Project.setDataPrototype("bool", "appendPID", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (9/15/2018): Added support for a report on the internal use of header file unparsing.
  // This is to support the header file unparsing when used with and without the token based unparsing.
     Project.setDataPrototype("bool", "reportOnHeaderFileUnparsing", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/6/2018): Added to support unparsing of headers and source files for whole applications having 
  // multiple levels of directory structure.
     Project.setDataPrototype("std::string", "applicationRootDirectory", "= \"\"",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype("bool", "usingApplicationRootDirectory", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     Project.setDataPrototype("bool", "ast_merge", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype("std::string", "astfile_out", "",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Project.setDataPrototype("std::list<std::string>", "astfiles_in", "",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


     Attribute.setDataPrototype    ( "std::string"  , "name", "= \"\"",
                                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
   //  Attribute.setAutomaticGenerationOfCopyFunction(false);
  // Attribute.setDataPrototype    ( "char*", "name"    , "= \"\"" );

     BitAttribute.setDataPrototype ( "unsigned long int"  , "bitflag", "= 0",
                                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (4/6/2004): Depricated ModifierNodes node and new separate TypeModifier and StorageModifier nodes
  // MK: I moved the following data member declarations from ../Grammar/Support.code to this position:
  // ModifierNodes.setDataPrototype("SgModifierTypePtrVector", "nodes", "= NULL",
     ModifierNodes.setDataPrototype("SgModifierTypePtrVector", "nodes", "",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     ModifierNodes.setDataPrototype("SgModifierNodes*", "next", "= NULL",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     ModifierNodes.setAutomaticGenerationOfConstructor(false);
  // DQ (12/4/2004): Now we automate the generation of the destructors
  // ModifierNodes.setAutomaticGenerationOfDestructor (false);

#if 0
Specifiers that can have multiple values (implemented with a protected vector<bool> or similar):
     function specifiers
     special_function specifiers  (e.g. constructor and conversion)
     declaration specifiers
     type specifier
     elaborated-type-specifier (typename and class ?)
     base-class-specifier

Specifiers that can have only one value (implemented with a protected enum variable internally):
     storage specifiers
     const_volatile
     access specifiers
     UPC access specifiers
     linkage-specification
#endif

  // DQ (4/6/2004): New nodes (use "modifiers" if multiple values can be set,
  //                use "modifier" if only one value can be set).
  // DQ (4/19/2004): New modifiers (C++ grammar calls them specifiers)
  //    ConstVolatileModifier, StorageModifier, AccessModifier, FunctionModifier,
  //    UPC_AccessModifier, SpecialFunctionModifier, DeclarationModifier, TypeModifier,
  //    ElaboratedTypeModifier, LinkageModifier, BaseClassModifier
  // DQ (4/19/2004): Commented out override of construction of constructors
  // xxxModifier.setAutomaticGenerationOfConstructor(false);
  // xxxModifier.setAutomaticGenerationOfDestructor (false);
     ConstVolatileModifier.setDataPrototype("SgConstVolatileModifier::cv_modifier_enum", "modifier", "= SgConstVolatileModifier::e_unknown",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     StorageModifier.setDataPrototype("SgStorageModifier::storage_modifier_enum", "modifier","= SgStorageModifier::e_unknown",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (12/1/2007): Added support for gnu extension "__thread" (will be available in EDG version > 3.3)
  // But added to support use by Gouchun Shi (UIUC).  Code generation support also added in unparser.
     StorageModifier.setDataPrototype("bool", "thread_local_storage","= false",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AccessModifier.setDataPrototype("SgAccessModifier::access_modifier_enum", "modifier", "= SgAccessModifier::e_unknown",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Note that more than one value can be set, so this implements a bit vector of flags to be used.
     FunctionModifier.setDataPrototype("SgBitVector", "modifierVector", "",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (1/3/2009): Added support for GNU constructor priority (added paramter "N" as in "void f __attribute__((constructor (N)));")
     FunctionModifier.setDataPrototype("unsigned long int", "gnu_attribute_constructor_destructor_priority", "= 0",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     FunctionModifier.setDataPrototype("std::string", "gnu_attribute_named_weak_reference", "=\"\"",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     FunctionModifier.setDataPrototype("std::string", "gnu_attribute_named_alias", "=\"\"",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // TV (05/03/2010): 
     FunctionModifier.setDataPrototype("SgType *", "opencl_vec_type", "= NULL",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
                                    
     FunctionModifier.setDataPrototype("SgFunctionModifier::opencl_work_group_size_t", "opencl_work_group_size", "",//"= {1, 1, 1}",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
                                    
     SpecialFunctionModifier.setDataPrototype("SgBitVector","modifierVector", "",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     UPC_AccessModifier.setDataPrototype("SgUPC_AccessModifier::upc_access_modifier_enum", "modifier", "= SgUPC_AccessModifier::e_unknown",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UPC_AccessModifier.setDataPrototype("bool", "isShared","= false",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // Note that layout == 0 means allocate all to one thread (what EDG calls "INDEFINITE") (also same as "[]"),
  //           layout == -1 means no layout was specified (what EDG calls "NONE"),
  //           layout == -2 means layout == "*" (what EDG calls "BLOCK")
  // any non-zero positive value is the block size,
  // the default is block size == 1 (cyclic distribution).
     UPC_AccessModifier.setDataPrototype("long", "layout","= -1",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     DeclarationModifier.setDataPrototype("SgBitVector", "modifierVector", "",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     DeclarationModifier.setDataPrototype("SgTypeModifier", "typeModifier", ".reset()",
                                    NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     DeclarationModifier.setDataPrototype("SgAccessModifier", "accessModifier", ".reset()",
                                    NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     DeclarationModifier.setDataPrototype("SgStorageModifier", "storageModifier", ".reset()",
                                    NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (8/18/2014): Note that this is no longer GNU specific and is used to hold the section name for microsoft 
  // specific attributes (e.g. allocate("segment_name") and code_seg("segment_name")). Changing the name of this
  // data member would make this more clear.
     DeclarationModifier.setDataPrototype("std::string", "gnu_attribute_section_name", "=\"\"",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     DeclarationModifier.setDataPrototype("SgDeclarationModifier::gnu_declaration_visability_enum", "gnu_attribute_visability","= SgDeclarationModifier::e_unknown_visibility",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     DeclarationModifier.setDataPrototype("std::string", "microsoft_uuid_string", "=\"\"",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     DeclarationModifier.setDataPrototype("std::string", "microsoft_property_get_function_name", "=\"\"",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     DeclarationModifier.setDataPrototype("std::string", "microsoft_property_put_function_name", "=\"\"",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     TypeModifier.setDataPrototype("SgBitVector", "modifierVector", "",
                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     TypeModifier.setDataPrototype("SgUPC_AccessModifier", "upcModifier", ".reset()",
                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     TypeModifier.setDataPrototype("SgConstVolatileModifier", "constVolatileModifier", ".reset()",
                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     TypeModifier.setDataPrototype("SgElaboratedTypeModifier", "elaboratedTypeModifier", ".reset()",
                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (12/4/2007): GNU extension machine mode.
  // There are a lot of these and type codes can be used to specify them.
     TypeModifier.setDataPrototype("SgTypeModifier::gnu_extension_machine_mode_enum", "gnu_extension_machine_mode", "= SgTypeModifier::e_gnu_extension_machine_mode_unspecified",
                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (3/1/2013): To support letting zero being an acceptable value, we want to make the default value -1.
  // To do this we need for this value to be signed, since the range of values is so small it can be a short value.
  // DQ (1/3/2009): Added support for alignment specfication using gnu attributes (zero is used as the default to imply no alignment specification).
  // TypeModifier.setDataPrototype("unsigned long int", "gnu_attribute_alignment", "= -1",
  //            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // TypeModifier.setDataPrototype("short", "gnu_attribute_alignment", "= -1",
  //            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     TypeModifier.setDataPrototype("int", "gnu_attribute_alignment", "= -1",
                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (1/3/2009): This is used for funtion types only. I reserve values less than zero (-1 implies that
  // this was not set, default value). Note that the standard might require this to be unsigned, but I
  // would like to avoid the EDG tick of shifting the value by one to reserve zero to be the default.
     TypeModifier.setDataPrototype("long", "gnu_attribute_sentinel", "= -1",
                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     TypeModifier.setDataPrototype("unsigned", "address_space_value", "= 0",
                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     TypeModifier.setDataPrototype("unsigned", "vector_size", "= 1",
                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     ElaboratedTypeModifier.setDataPrototype("SgElaboratedTypeModifier::elaborated_type_modifier_enum", "modifier",
                "= SgElaboratedTypeModifier::e_unknown",
                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     LinkageModifier.setDataPrototype("SgLinkageModifier::linkage_modifier_enum", "modifier",
                "= SgLinkageModifier::e_unknown",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     BaseClassModifier.setDataPrototype("SgBaseClassModifier::baseclass_modifier_enum", "modifier",
                "= SgBaseClassModifier::e_unknown",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     BaseClassModifier.setDataPrototype("SgAccessModifier", "accessModifier", "",
                                    NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


  // MK: I moved the following data member declarations from ../Grammar/Support.code to this position:
  // File_Info.setDataPrototype("char*","filename","= NULL",
  //        NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);
  // File_Info.setDataPrototype("int","file_id","= NULL_FILE_ID",
  //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File_Info.setDataPrototype("int","file_id","= NULL_FILE_ID",
                                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File_Info.setDataPrototype("int","line","= 0",
                                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File_Info.setDataPrototype("int","col","= 0",
                                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // File_Info.setDataPrototype("bool","isPartOfTransformation","= false",
  //        NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // We need a way to classify statements as being from a transformation, or a if from a
  // transformation either representing the transformation, declarations required by the
  // transformation, or intializations associated with a transformation. Indidual bits
  // within the integer are used to classify File_Info objects. Bits 0-15 are presently
  // reserved.
     File_Info.setDataPrototype("unsigned int","classificationBitField","= 0",
                                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (12/18/2012): Adding physical source position (existing file_id and line data is for logical file position).
  // Note that the column number is the same for both logical and physical positions.
     File_Info.setDataPrototype("int","physical_file_id","= NULL_FILE_ID",
                                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File_Info.setDataPrototype("int","physical_line","= 0",
                                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     File_Info.setDataPrototype("unsigned int","source_sequence_number","= 0",
                                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (2/27/2019): I want to add line number support, and use this to test if we can support 
  // CPP directives and comments added to alll shared IR nodes from all files.  If this works
  // then we can consider a better design that would use a different data structure.
  // DQ (2/26/2019): We need to use BUILD_LIST_ACCESS_FUNCTIONS else we return a copy of
  // the list and only modify the copy instead of the list stored in the IR node.
  // MK (8/2/05) : This set contains a list of file ids. During unparsing, if we encounter
  //               a node with this Sg_File_Info object, we only want to unparse this file
  //               if the file we are currently unparsing is in this list.
  //               NOTE: this set should be empty unless the node is marked as shared
  //! This set contains a list of all file ids for which the accompanying node should be unparsed
  // File_Info.setDataPrototype("std::set<int>","fileIDsToUnparse","",
  // File_Info.setDataPrototype("SgFileIdList","fileIDsToUnparse","",
  //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // File_Info.setDataPrototype("SgFileIdList","fileIDsToUnparse","",
  //        NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // File_Info.setDataPrototype("SgFileIdList","fileIDsToUnparse","",
  //        NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File_Info.setDataPrototype("SgFileIdList","fileIDsToUnparse","",
            NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File_Info.setDataPrototype("SgFileLineNumberList","fileLineNumbersToUnparse","",
            NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // I can't see where this is being used or that it needs to be here (these are used in unparser!)
  // File_Info.setDataPrototype("int","referenceCount","= 0",
  //                            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // File_Info.setDataPrototype("static int","cur_line","= 0",
  //                            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // File_Info.setDataPrototype("static std::string","cur_file","= \"\"",
  //                            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File_Info.setPostdeclarationString   ("HEADER_UNPARSE_POSTDECLARATION", "../Grammar/Support.code");

  // DQ (2/23/2010): Added static access function for static data members (ROSETTA generates only not statuc access functions).
  // File_Info.setDataPrototype("static std::map<int, std::string>","fileidtoname_map","",
  //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // File_Info.setDataPrototype("static std::map<std::string, int>","nametofileid_map","",
  //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File_Info.setDataPrototype("static std::map<int, std::string>","fileidtoname_map","",
                                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File_Info.setDataPrototype("static std::map<std::string, int>","nametofileid_map","",
                                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // File_Info.setDataPrototype("static int","max_file_id","",
  //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Note that using this feature can make the compilation for the header
  // file sensitive to the ordering of the classes within the output file
     Name.setPostdeclarationString ("HEADER_NAME_POSTDECLARATION", "../Grammar/Support.code");
     Name.setAutomaticGenerationOfConstructor(false);
  // DQ (12/4/2004): Now we automate the generation of the destructors
  // Name.setAutomaticGenerationOfDestructor (false);

     SymbolTable.setPredeclarationString  ("HEADER_SYMBOL_TABLE_PREDECLARATION" , "../Grammar/Support.code");
     SymbolTable.setPostdeclarationString ("HEADER_SYMBOL_TABLE_POSTDECLARATION", "../Grammar/Support.code");
     SymbolTable.setAutomaticGenerationOfConstructor(false);
  // DQ (12/4/2004): Now we automate the generation of the destructors
  // SymbolTable.setAutomaticGenerationOfDestructor (false);

     FuncDecl_attr.setPostdeclarationString  ("HEADER_FUNCTION_DECLARATION_ATTRIBUTE_POSTDECLARATION", "../Grammar/Support.code");
     FuncDecl_attr.setAutomaticGenerationOfConstructor(false);
     ClassDecl_attr.setPostdeclarationString ("HEADER_CLASS_DECLARATION_ATTRIBUTE_POSTDECLARATION", "../Grammar/Support.code");
     ClassDecl_attr.setAutomaticGenerationOfConstructor(false);
  // TemplateInstDecl_attr.setPostdeclarationString ("HEADER_TEMPLATE_INSTANTIATION_DECLARATION_ATTRIBUTE_POSTDECLARATION",
  //                                             "../Grammar/Support.code");
  // TemplateInstDecl_attr.setAutomaticGenerationOfConstructor(false);

  // MK: I moved the following data member declarations from ../Grammar/Support.code to this position:
     Options.setDataPrototype("std::ostream*","default_output","= NULL",
                              NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);
     Options.setDataPrototype("std::ostream*","debug_output","= NULL",
                              NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);
     Options.setDataPrototype("std::ostream*","error_output","= NULL",
                              NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);
     Options.setDataPrototype("std::ostream*","logging_output","= NULL",
                              NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);
     Options.setDataPrototype("int","debug_level","= 0",
                              NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Options.setDataPrototype("int","logging_level","= 0",
                              NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (3/12/2004): Added to support templates
     TemplateParameter.setFunctionPrototype ( "HEADER_TEMPLATE_PARAMETER", "../Grammar/Support.code");
     TemplateParameter.setDataPrototype     ( "SgTemplateParameter::template_parameter_enum"  , "parameterType", "= parameter_undefined",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     TemplateParameter.setDataPrototype     ( "SgType*", "type", "= NULL",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     TemplateParameter.setDataPrototype     ( "SgType*", "defaultTypeParameter", "= NULL",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     TemplateParameter.setDataPrototype     ( "SgExpression*", "expression", "= NULL",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     TemplateParameter.setDataPrototype     ( "SgExpression*", "defaultExpressionParameter", "= NULL",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     TemplateParameter.setDataPrototype     ( "SgDeclarationStatement*", "templateDeclaration", "= NULL",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     TemplateParameter.setDataPrototype     ( "SgDeclarationStatement*", "defaultTemplateDeclarationParameter", "= NULL",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (11/21/2011): template parameters can be "int U = 42" in which case "U" needs to be an initialized name (see test2011_157.C).
     TemplateParameter.setDataPrototype     ( "SgInitializedName*", "initializedName", "= NULL",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);


     TemplateArgument.setFunctionPrototype ( "HEADER_TEMPLATE_ARGUMENT", "../Grammar/Support.code");
     TemplateArgument.setDataPrototype     ( "SgTemplateArgument::template_argument_enum"   , "argumentType", "= argument_undefined",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     TemplateArgument.setDataPrototype     ( "bool"   , "isArrayBoundUnknownType", "= false",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     TemplateArgument.setDataPrototype     ( "SgType*", "type", "= NULL",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/27/2016): Adding an optional (and more suitable reference to a type alias that can be unparsed).
  // This type was evaluated to no reference internal private types that are marked as private access.  Due 
  // to a normalization by EDG, we can sometimes have teamplate arguments referencing types that would result 
  // in an error with newer compilers (e.g. GNU 6.1) if they were unparsed in the generated code.  This pointer
  // is available as an alternative type that can be unparsed (used by the unparser, and the name qualification).
  // This type does not have any priviate access typedefs contained within it.
     TemplateArgument.setDataPrototype     ( "SgType*", "unparsable_type_alias", "= NULL",
                                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Could be an array bound (integer) or some unknown type
     TemplateArgument.setDataPrototype     ( "SgExpression*", "expression", "= NULL",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (12/22/2011): The new design has a SgTemplateClassDeclaration derived from a SgClassDeclaration (and
  // the same for SgTemplateFunctionDeclaration, etc.) So we have to change this to support the new design.
  // Now we have to use a common base class which would be the SgDeclarationStatement.
  // DQ: Case of a template specialization (and partial specialization) not handled
  // here (not clear on how to do this)
  // TemplateArgument.setDataPrototype     ( "SgTemplateInstantiationDecl*", "templateInstantiation", "= NULL",
  //              CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     TemplateArgument.setDataPrototype     ( "SgDeclarationStatement*", "templateDeclaration", "= NULL",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (8/10/2013): template arguments can be a SgInitializedName or an expression when it is a non-type (see test2013_303.C).
  // I think we want to eventually make this a constructor argument, but not at first.
     TemplateArgument.setDataPrototype     ( "SgInitializedName*", "initializedName", "= NULL",
                                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (2/27/2005): Support for recognition of default template arguments
  // (required to fix bug demonstrated in test2005_12.C)
     TemplateArgument.setDataPrototype     ( "bool", "explicitlySpecified", "= true",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
  // DQ (5/14/2011): Added support for name qualification.
  // TemplateArgument.setDataPrototype ( "int", "name_qualification_length", "= 0",
  //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     TemplateArgument.setDataPrototype ( "int", "name_qualification_length", "= 0",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
  // DQ (5/14/2011): Added information required for new name qualification support.
  // TemplateArgument.setDataPrototype("bool","type_elaboration_required","= false",
  //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     TemplateArgument.setDataPrototype("bool","type_elaboration_required","= false",
                                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
  // DQ (5/14/2011): Added information required for new name qualification support.
  // TemplateArgument.setDataPrototype("bool","global_qualification_required","= false",
  //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     TemplateArgument.setDataPrototype("bool","global_qualification_required","= false",
                                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#if 1
  // DQ (5/4/2013): These variables are needed to reproduce the way that we handle types 
  // in function parameters and allow the same technique to be used for template arguments.
  // This needs to be handled using a template function where we will refactor the relevent 
  // code that can be shared between the function parameter handling in function
  // unparseFunctionParameterDeclaration() and the template argument handling in 
  // function unparseTemplateArgument().

  // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
  // DQ (12/20/2006): Record if global name qualification is required on the type.
  // See test2003_01.C for an example of where this is required. Note that for a
  // variable declaration (SgVariableDeclaration) this information is recorded directly
  // on the SgVariableDeclaration node.  This use on the InitializedName is reserved for
  // function parameters, and I am not sure if it is useful anywhere else.
  // TemplateArgument.setDataPrototype("bool", "requiresGlobalNameQualificationOnType", "= false",
  //             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     TemplateArgument.setDataPrototype("bool", "requiresGlobalNameQualificationOnType", "= false",
                 NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
  // DQ (5/12/2011): Added support for name qualification on the type referenced by the InitializedName
  // (not the SgInitializedName itself since it might be referenced from several places, I think).
  // TemplateArgument.setDataPrototype ( "int", "name_qualification_length_for_type", "= 0",
  //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     TemplateArgument.setDataPrototype ( "int", "name_qualification_length_for_type", "= 0",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
  // DQ (5/12/2011): Added information required for new name qualification support.
  // TemplateArgument.setDataPrototype("bool","type_elaboration_required_for_type","= false",
  //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     TemplateArgument.setDataPrototype("bool","type_elaboration_required_for_type","= false",
                                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
  // DQ (5/12/2011): Added information required for new name qualification support.
  // TemplateArgument.setDataPrototype("bool","global_qualification_required_for_type","= false",
  //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     TemplateArgument.setDataPrototype("bool","global_qualification_required_for_type","= false",
                                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif
  // TV (10/05/2018): create a double-link chain of instances of a template argument instead of sharing (ROSE-1431)
     TemplateArgument.setDataPrototype("SgTemplateArgument*","previous_instance","= NULL",
                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     TemplateArgument.setDataPrototype("SgTemplateArgument*","next_instance","= NULL",
                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (2/10/2019): Need to be able to specify function parameters that are a part of C++11 parameter pack associated with variadic templates. 
  // TemplateArgument.setDataPrototype     ( "bool", "is_parameter_pack", "= false",
  //           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     TemplateArgument.setDataPrototype     ( "bool", "is_pack_element", "= false",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (3/10/2018): I think these IR nodes are not longer used.  If so then we could remove them.
  // DQ (4/2/2007): Added list as separate IR node to support mixing of lists and data members in IR nodes in ROSETTA.
     TemplateArgumentList.setFunctionPrototype ( "HEADER_TEMPLATE_ARGUMENT_LIST", "../Grammar/Support.code");
     TemplateArgumentList.setDataPrototype ( "SgTemplateArgumentPtrList", "args",  "",
                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     TemplateParameterList.setFunctionPrototype ( "HEADER_TEMPLATE_PARAMETER_LIST", "../Grammar/Support.code");
     TemplateParameterList.setDataPrototype ( "SgTemplateParameterPtrList", "args",  "",
                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // This function exists everywhere (at each node of the grammar)!
  // Node.setSubTreeFunctionPrototype ( "HEADER_IS_CLASSNAME", "../Grammar/Node.code");
  // Node.excludeFunctionPrototype( "HEADER_IS_CLASSNAME", "../Grammar/Node.code");
  // Node.setDataPrototype    ( "SgAttribute*","attribute","= NULL");
  // Node.excludeDataPrototype( "SgAttribute*","attribute","= NULL");

  // LocatedNode.setFunctionPrototype ( "HEADER", "../Grammar/LocatedNode.code");
  // LocatedNode.setDataPrototype     ( "Sg_File_Info*", "file_info", "= NULL");

     NameGroup.setFunctionPrototype ( "HEADER_NAME_GROUP", "../Grammar/Support.code");
     NameGroup.setDataPrototype     ( "std::string", "group_name", "=\"\"",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (11/30/2007): Later make this a SgExpression* so that we can use it for the associate
  // list and so that it more accurately reflects the namelist semantics.
  // NameGroup.setDataPrototype     ( "SgStringList", "name_list", "",
  //              NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     NameGroup.setDataPrototype     ( "SgStringList", "name_list", "",
                  NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     DimensionObject.setFunctionPrototype ( "HEADER_DIMENSION_OBJECT", "../Grammar/Support.code");
     DimensionObject.setDataPrototype     ( "SgInitializedName*", "array", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     DimensionObject.setDataPrototype     ( "SgExprListExp*", "shape", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     DataStatementGroup.setFunctionPrototype ( "HEADER_DATA_STATEMENT_GROUP", "../Grammar/Support.code");
     DataStatementGroup.setDataPrototype     ( "SgDataStatementObjectPtrList", "object_list", "",
                  NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     DataStatementGroup.setDataPrototype     ( "SgDataStatementValuePtrList", "value_list", "",
                  NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     DataStatementObject.setFunctionPrototype ( "HEADER_DATA_STATEMENT_OBJECT", "../Grammar/Support.code");
     DataStatementObject.setDataPrototype     ( "SgExprListExp*", "variableReference_list", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     DataStatementValue.setFunctionPrototype ( "HEADER_DATA_STATEMENT_VALUE", "../Grammar/Support.code");
     DataStatementValue.setDataPrototype     ( "SgDataStatementValue::data_statement_value_enum", "data_initialization_format", "= SgDataStatementValue::e_unknown",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // These can even be multi-dimensional implied do loops (yuk!)
     DataStatementValue.setDataPrototype     ( "SgExprListExp*", "initializer_list", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     DataStatementValue.setDataPrototype     ( "SgExpression*", "repeat_expression", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     DataStatementValue.setDataPrototype     ( "SgExpression*", "constant_expression", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Support for Fortran format statement.  I have provided a separate object because
  // I think the grammar for this construct is sufficently complex that we might want a
  // more sophisticated implementation in the IR later.  for now I just capture the
  // information as a string literal in an expression.
     FormatItem.setFunctionPrototype ( "HEADER_FORMAT_ITEM", "../Grammar/Support.code");
     FormatItem.setDataPrototype     ( "int", "repeat_specification", "= -1",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     FormatItem.setDataPrototype     ( "SgExpression*", "data", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     FormatItem.setDataPrototype     ( "SgFormatItemList*", "format_item_list", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (12/23/2007): Added list of SgFormatItem objects
     FormatItemList.setFunctionPrototype ( "HEADER_FORMAT_ITEM_LIST", "../Grammar/Support.code");
     FormatItemList.setDataPrototype     ( "SgFormatItemPtrList", "format_item_list", "",
                  NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

#if 0
  // DQ (10/6/2008): Moved to SgLocatedNodeSupport.
     RenamePair.setFunctionPrototype ( "HEADER_RENAME_PAIR", "../Grammar/Support.code");
     RenamePair.setDataPrototype     ( "SgName", "local_name", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     RenamePair.setDataPrototype     ( "SgName", "use_name", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     RenamePair.setDataPrototype     ( "Sg_File_Info*", "startOfConstruct", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);

  // DQ (10/6/2008): Moved to SgLocatedNodeSupport.
  // DQ (10/6/2008): Added support for interface bodies so that we could capture the information
  // used to specify function declaration ro function names in interface statements.
     InterfaceBody.setFunctionPrototype ( "HEADER_INTERFACE_BODY", "../Grammar/Support.code");

  // Record whether the function declaration or the function name was used in the interface body (F90 permits either one).
     InterfaceBody.setDataPrototype     ( "SgName", "function_name", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // We can't traverse this since it may be the same as a declaration in a contains statement.
  // However, if we can properly support the defining vs. non defining declaration then maybe
  // we can.  Work on this later.
     InterfaceBody.setDataPrototype     ( "SgFunctionDeclaration*", "functionDeclaration", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     InterfaceBody.setDataPrototype     ( "bool", "use_function_name", "= false",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     InterfaceBody.setDataPrototype     ( "Sg_File_Info*", "startOfConstruct", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);
     InterfaceBody.setDataPrototype     ( "Sg_File_Info*", "endOfConstruct", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);
#endif

  // TV (05/03/2010): OpenCL Access Mode Support
  
    OpenclAccessModeModifier.setFunctionPrototype ( "HEADER_OPENCL_ACCESS_MODE_MODIFIER", "../Grammar/Support.code" );
    
    OpenclAccessModeModifier.setDataPrototype("SgOpenclAccessModeModifier::access_mode_modifier_enum", "modifier","= SgOpenclAccessModeModifier::e_unknown",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // ***********************************************************************
  // ***********************************************************************
  //                       Source Code Definition
  // ***********************************************************************
  // ***********************************************************************

  // Source code for Node
  // Node.setSubTreeFunctionSource ( "SOURCE", "../Grammar/Common.code" );
  // Node.setFunctionSource( "SOURCE", "../Grammar/Node.code");
  // Node.setFunctionSource( "SOURCE_ROOT_NODE_ERROR_FUNCTION", "../Grammar/Node.code");
  // Node.setSubTreeFunctionSource( "SOURCE_ERROR_FUNCTION", "../Grammar/Node.code");
  // Node.excludeFunctionSource( "SOURCE_ERROR_FUNCTION", "../Grammar/Node.code");
  // Node.editSubstitute("CONSTRUCTOR_BODY"," ");
  // Source code for LocatedNode
  // LocatedNode.setFunctionSource( "SOURCE", "../Grammar/LocatedNode.code");
  // LocatedNode.editSubstitute("CONSTRUCTOR_BODY"," ");

     Support.setFunctionSource         ( "SOURCE", "../Grammar/Support.code");

     SymbolTable.setFunctionSource     ( "SOURCE_SYMBOL_TABLE", "../Grammar/Support.code");

  // DQ (7/22/2010): 
     TypeTable.setFunctionSource    ( "SOURCE_TYPE_TABLE", "../Grammar/Support.code" );

     Name.setFunctionSource            ( "SOURCE_NAME", "../Grammar/Support.code");
     Attribute.setFunctionSource       ( "SOURCE_ATTRIBUTE", "../Grammar/Support.code");
     Pragma.setFunctionSource          ( "SOURCE_PRAGMA", "../Grammar/Support.code");
     BitAttribute.setFunctionSource    ( "SOURCE_BIT_ATTRIBUTE", "../Grammar/Support.code");

  // DQ (4/6/2004): Depricated ModifierNodes node and new separate TypeModifier and StorageModifier nodes
  // DQ (4/19/2004): New modifiers (C++ grammar calls them specifiers)
  //    ConstVolatileModifier, StorageModifier, AccessModifier, FunctionModifier,
  //    UPC_AccessModifier, SpecialFunctionModifier, DeclarationModifier, TypeModifier,
  //    ElaboratedTypeModifier, LinkageModifier, BaseClassModifier
     Modifier.setFunctionSource                ( "SOURCE_MODIFIER"                 , "../Grammar/Support.code");
     ModifierNodes.setFunctionSource           ( "SOURCE_MODIFIER_NODES"           , "../Grammar/Support.code");
     ConstVolatileModifier.setFunctionSource   ( "SOURCE_CV_TYPE_MODIFIER"         , "../Grammar/Support.code");
     StorageModifier.setFunctionSource         ( "SOURCE_STORAGE_MODIFIER"         , "../Grammar/Support.code");
     AccessModifier.setFunctionSource          ( "SOURCE_ACCESS_MODIFIER"          , "../Grammar/Support.code");
     FunctionModifier.setFunctionSource        ( "SOURCE_FUNCTION_MODIFIER"        , "../Grammar/Support.code");
     UPC_AccessModifier.setFunctionSource      ( "SOURCE_UPS_ACCESS_MODIFIER"      , "../Grammar/Support.code");
     SpecialFunctionModifier.setFunctionSource ( "SOURCE_SPECIAL_FUNCTION_MODIFIER", "../Grammar/Support.code");
     DeclarationModifier.setFunctionSource     ( "SOURCE_DECLARATION_MODIFIER"     , "../Grammar/Support.code");
     TypeModifier.setFunctionSource            ( "SOURCE_TYPE_MODIFIER"            , "../Grammar/Support.code");
     ElaboratedTypeModifier.setFunctionSource  ( "SOURCE_ELABORATED_TYPE_MODIFIER" , "../Grammar/Support.code");
     LinkageModifier.setFunctionSource         ( "SOURCE_LINKAGE_MODIFIER"         , "../Grammar/Support.code");
     BaseClassModifier.setFunctionSource       ( "SOURCE_BASECLASS_MODIFIER"       , "../Grammar/Support.code");

  // Place declarations of friend output operators after the BaseClassModifier
  // Modifier.setPostdeclarationString   ("SOURCE_MODIFIER_POSTDECLARATION", "../Grammar/Support.code");

     File_Info.setFunctionSource       ( "SOURCE_FILE_INFORMATION", "../Grammar/Support.code");

     Directory.setFunctionSource       ( "SOURCE_APPLICATION_DIRECTORY", "../Grammar/Support.code");
     DirectoryList.setFunctionSource   ( "SOURCE_APPLICATION_DIRECTORY_LIST", "../Grammar/Support.code");
     File.setFunctionSource            ( "SOURCE_APPLICATION_FILE", "../Grammar/Support.code");
     SourceFile.setFunctionSource      ( "SOURCE_APPLICATION_SOURCE_FILE", "../Grammar/Support.code");
     BinaryComposite.setFunctionSource ( "SOURCE_APPLICATION_BINARY_FILE", "../Grammar/Support.code");
     FileList.setFunctionSource        ( "SOURCE_APPLICATION_FILE_LIST", "../Grammar/Support.code");
     UnknownFile.setFunctionSource     ( "SOURCE_APPLICATION_UNKNOWN_FILE", "../Grammar/Support.code");

  // DQ (9/18/2-18): Adding support for the include file tree into the SgSourceFile.
     IncludeFile.setFunctionSource      ( "SOURCE_INCLUDE_FILE", "../Grammar/Support.code");

     JavaImportStatementList.setFunctionSource  ( "SOURCE_JAVA_IMPORT_STATEMENT_LIST", "../Grammar/Support.code");
     JavaClassDeclarationList.setFunctionSource ( "SOURCE_JAVA_CLASS_DECLARATION_LIST", "../Grammar/Support.code");

     HeaderFileReport.setFunctionSource      ( "SOURCE_HEADER_FILE_REPORT", "../Grammar/Support.code");

     Project.setFunctionSource         ( "SOURCE_APPLICATION_PROJECT", "../Grammar/Support.code");
     Options.setFunctionSource         ( "SOURCE_OPTIONS", "../Grammar/Support.code");
     Unparse_Info.setFunctionSource    ( "SOURCE_UNPARSE_INFO", "../Grammar/Support.code");

     BaseClass.setFunctionSource        ( "SOURCE_BASECLASS", "../Grammar/Support.code");
     ExpBaseClass.setFunctionSource     ( "SOURCE_EXP_BASE_CLASS", "../Grammar/Support.code");
     NonrealBaseClass.setFunctionSource ( "SOURCE_NONREAL_BASE_CLASS", "../Grammar/Support.code");

  // DQ (12/19/2005): Support for explicitly specified qualified names
     QualifiedName.setFunctionSource   ( "SOURCE_QUALIFIED_NAME", "../Grammar/Support.code");

     FuncDecl_attr.setFunctionSource   ( "SOURCE_FUNCTION_DECLARATION_ATTRIBUTE", "../Grammar/Support.code");
     ClassDecl_attr.setFunctionSource  ( "SOURCE_CLASS_DECLARATION_ATTRIBUTE", "../Grammar/Support.code");
  // TemplateInstDecl_attr.setFunctionSource  ( "SOURCE_TEMPLATE_INSTANTIATION_DECLARATION_ATTRIBUTE", "../Grammar/Support.code");

     TypedefSeq.setFunctionPrototype ( "HEADER_TYPEDEF_SEQ", "../Grammar/Support.code" );
     TypedefSeq.setFunctionSource    ( "SOURCE_TYPEDEF_SEQ", "../Grammar/Support.code" );

  // DQ (9/1/2004): Since we call this constructor we should build it
  // TypedefSeq.setAutomaticGenerationOfConstructor(false);
     TypedefSeq.setAutomaticGenerationOfConstructor(true);

  // DQ (12/4/2004): Now we automate the generation of the destructors
  // TypedefSeq.setAutomaticGenerationOfDestructor (true);
     TypedefSeq.setDataPrototype("SgTypePtrList","typedefs","",
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     FunctionParameterTypeList.setFunctionPrototype ( "HEADER_FUNCTION_PARAMETER_TYPE_LIST", "../Grammar/Support.code" );
     FunctionParameterTypeList.setFunctionSource    ( "SOURCE_FUNCTION_PARAMETER_TYPE_LIST", "../Grammar/Support.code" );
     FunctionParameterTypeList.setAutomaticGenerationOfConstructor(false);
  // DQ (12/4/2004): Now we automate the generation of the destructors
  // FunctionParameterTypeList.setAutomaticGenerationOfDestructor (true);
     FunctionParameterTypeList.setDataPrototype("SgTypePtrList","arguments","",
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

#if 0
  // DQ (12/21/2005): Global qualification and qualified name handling are now represented explicitly
  // in the AST. Here we have a list of qualified names a global qualification is represented by
  // a SgQualifiedName that contains a pointer to the SgGlobal scope. This is so variables such as
  // "::X::Y::Z::variable" can be represented properly.
     FunctionParameterTypeList.setDataPrototype ( "SgQualifiedNamePtrList", "returnTypeQualifiedNameList", "",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

  // DQ (3/12/2004): Added for template support
     TemplateParameter.setFunctionSource ( "SOURCE_TEMPLATE_PARAMETER", "../Grammar/Support.code");
     TemplateArgument.setFunctionSource  ( "SOURCE_TEMPLATE_ARGUMENT" , "../Grammar/Support.code");

  // DQ (4/2/2007): Added list as separate IR node to support mixing of lists and data members in IR nodes in ROSETTA.
     TemplateParameterList.setFunctionSource ( "SOURCE_TEMPLATE_PARAMETER_LIST" , "../Grammar/Support.code");
     TemplateArgumentList.setFunctionSource  ( "SOURCE_TEMPLATE_ARGUMENT_LIST" , "../Grammar/Support.code");

  // tps (08/13/2007): Added specific code to graph entities
     Graph.setFunctionSource ( "SOURCE_GRAPH" , "../Grammar/Support.code");
     GraphNode.setFunctionSource  ( "SOURCE_GRAPHNODE" , "../Grammar/Support.code");
     GraphEdge.setFunctionSource  ( "SOURCE_GRAPHEDGE" , "../Grammar/Support.code");



     DirectedGraphEdge.setFunctionSource ( "SOURCE_DIRECTED_GRAPH_EDGE", "../Grammar/Support.code");


  // DQ (11/19/2007): Added support for Fortran namelist statement
     NameGroup.setFunctionSource         ( "SOURCE_NAME_GROUP", "../Grammar/Support.code");

     DimensionObject.setFunctionSource ( "SOURCE_DIMENSION_OBJECT", "../Grammar/Support.code");

  // DQ (11/20/2007): Added support for Fortran data statement
     DataStatementGroup.setFunctionSource  ( "SOURCE_DATA_STATEMENT_GROUP",  "../Grammar/Support.code");
     DataStatementObject.setFunctionSource ( "SOURCE_DATA_STATEMENT_OBJECT", "../Grammar/Support.code");
     DataStatementValue.setFunctionSource  ( "SOURCE_DATA_STATEMENT_VALUE",  "../Grammar/Support.code");


     FormatItem.setFunctionSource ( "SOURCE_FORMAT_ITEM", "../Grammar/Support.code");
     FormatItemList.setFunctionSource ( "SOURCE_FORMAT_ITEM_LIST", "../Grammar/Support.code");

#if 0
  // DQ (10/6/2008): Moved to SgLocatedNodeSupport.
     RenamePair.setFunctionSource ( "SOURCE_RENAME_PAIR", "../Grammar/Support.code");

  // DQ (10/6/2008): Moved to SgLocatedNodeSupport.
     InterfaceBody.setFunctionSource ( "SOURCE_INTERFACE_BODY", "../Grammar/Support.code");
#endif

  // TV (05/03/2010): OpenCL Access Mode Support

    OpenclAccessModeModifier.setFunctionSource ( "SOURCE_OPENCL_ACCESS_MODE_MODIFIER", "../Grammar/Support.code" );

   }




















