
#include "grammar.h"

#include "ROSETTA_macros.h"
#include "terminal.h"
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

     NEW_TERMINAL_MACRO (InitializedName, "InitializedName", "InitializedNameTag" );
     NEW_TERMINAL_MACRO (Pragma, "Pragma", "PragmaTag" );
  // Grammar::Terminal  Attribute( "Attribute", sageGrammar, "Attribute" );
  // Grammar::Terminal  BitAttribute( "BitAttribute", sageGrammar, "BitAttribute" );

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

  // DQ (4/2/2007): Added list as separate IR node to support mixing of lists and data members in IR nodes in ROSETTA.
     NEW_TERMINAL_MACRO (TemplateParameterList, "TemplateParameterList", "TemplateParameterListTag" );
     NEW_TERMINAL_MACRO (TemplateArgumentList, "TemplateArgumentList", "TemplateArgumentListTag" );

  // We don't use these yet, though we might in the future!
  // Grammar::Terminal  ApplyFunction( "ApplyFunction", sageGrammar, "ApplyFunction" );
  // Grammar::Terminal  printFunction( "printFunction", sageGrammar, "printFunction" );

     NEW_NONTERMINAL_MACRO (BitAttribute, FuncDecl_attr | ClassDecl_attr /* | TemplateInstDecl_attr */,"BitAttribute","BitAttributeTag", false);

     NEW_NONTERMINAL_MACRO (Attribute, Pragma | BitAttribute, "Attribute", "AttributeTag", false);

  // DQ (4/25/2004): Must be placed before the modifiers (since it includes one as a data member)
     NEW_TERMINAL_MACRO (BaseClass, "BaseClass", "BaseClassTag" );

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

  // DQ (11/21/2007): This is part of support for the common block statement
     NEW_TERMINAL_MACRO (CommonBlockObject, "CommonBlockObject",     "TEMP_CommonBlockObject" );

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
          Modifier              | Name                      | SymbolTable         | InitializedName      |
          Attribute             | File_Info                 | File                | Project              |
          Options               | Unparse_Info              | BaseClass           | TypedefSeq           |
          TemplateParameter     | TemplateArgument          | Directory           | FileList             |
          DirectoryList         | FunctionParameterTypeList | QualifiedName       | TemplateArgumentList |
          TemplateParameterList | /* RenamePair                | InterfaceBody       |*/
          Graph                 | GraphNode                 | GraphEdge           |

          GraphNodeList         | GraphEdgeList             | TypeTable           |

          NameGroup             | CommonBlockObject         | DimensionObject     | FormatItem           |
          FormatItemList        | DataStatementGroup        | DataStatementObject | 
          DataStatementValue    ,
          "Support", "SupportTag", false);
//#endif


  // ***********************************************************************
  // ***********************************************************************
  //                       Header Code Declaration
  // ***********************************************************************
  // ***********************************************************************

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

  // DQ (6/12/2007): Fixed use of std::set<SgNode*> to use SgNodeSet so taht ROSETTA's test for
  // pointer to IR node can just look for "*" in the type.  This is a internal detail of ROSETTA.
  // DQ (3/10/2007): Adding set for symbols so that we can support fast tests for existance.
  // SymbolTable.setDataPrototype("SgNodeSetPtr","symbolSet", "= NULL",
  //          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // SymbolTable.setDataPrototype("std::set<SgNode*>","symbolSet", "",
     SymbolTable.setDataPrototype("SgNodeSet","symbolSet", "",
              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (7/22/2010): Added type table to support stricter uniqueness of types and proper sharing.
     TypeTable.setFunctionPrototype( "HEADER_TYPE_TABLE", "../Grammar/Support.code" );
     TypeTable.setAutomaticGenerationOfConstructor(false);
     TypeTable.setDataPrototype    ( "SgSymbolTable*","type_table","= NULL",
					     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);

  // InitializedName.setFunctionPrototype     ( "HEADER_INITIALIZED_NAME_DATA", "../Grammar/Support.code");

     InitializedName.setFunctionPrototype     ( "HEADER_INITIALIZED_NAME", "../Grammar/Support.code");

  // DQ (1/18/2006): renames this to be consistant and to allow the generated functions to map to
  // the virtual SgNode::get_file_info(). This then meens that we need to remove the use of
  // SgInitializedName::get_fileInfo() where it is used.
  // DQ (8/2/2004): Added fileInfo object to SgInitializedName object (to help with debugging and
  // to generally provide source position information on those IR nodes which are traversed).
  // InitializedName.setDataPrototype ( "Sg_File_Info*", "fileInfo", "= NULL",
  //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);
  // InitializedName.setDataPrototype ( "Sg_File_Info*", "file_info", "= NULL",
  //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);
     InitializedName.setDataPrototype ( "Sg_File_Info*", "startOfConstruct", "= NULL",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);

  // DQ (12/9/2004): Modified to make the access functions for this data member be automatically
  // generated.  As part of this change Alin's set_name() member function was moved to
  // ROSE/src/frontend/SageIII/sageSupport.[hC] since it has very specific semantics that does not
  // apply to all cases of where the set_name() access function should be called within a SgInitializedName
  // object (since SgInitializedName objects are used for both VariableDeclaration, preinitialization lists, etc.)
  // InitializedName.setDataPrototype("SgName","name", "= NULL",
  //      NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#if 1
  // DQ (10/9/2007): Use the ROSETTA generated version to test failure

     InitializedName.setDataPrototype("SgName","name", "= NULL",
          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#else
     InitializedName.setDataPrototype("SgName","name", "= \"\"",
          NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

  // FMZ (4/7/2009): Added for Cray pointer declaration
#if 0
#if USE_FORTRAN_IR_NODES
     InitializedName.setDataPrototype("bool","isCrayPointer", "= false",
          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     InitializedName.setDataPrototype("SgInitializedName*","crayPointee", "= NULL",
          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif 
#endif

  // DQ (7/20/2004):  think this is the root of the problems in cycles when we traverse types!
     InitializedName.setDataPrototype("SgType*","typeptr", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);
  // QY:11/2/04 remove itemptr
  //   InitializedName.setDataPrototype("SgInitializedName*","itemptr", "= NULL",
  //                                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL);
  // DQ (7/20/2004): The initptr in this SgInitializer object is NULL (set by EDG/SAGE connection)
  //                 to fix previous cycle in these objects (previously fixed in ASTFixes.C.
     InitializedName.setDataPrototype("SgInitializer*","initptr", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (7/20/2004): I think this is a hold over from the old implementation of SageII and that
  // it could be removed at some point.
     InitializedName.setDataPrototype("SgInitializedName*","prev_decl_item", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     InitializedName.setDataPrototype("bool","is_initializer", "= false",
                                      NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     InitializedName.setDataPrototype("SgDeclarationStatement*","declptr", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (3/4/2007): We want to force the copy mechanism to skip building a new SgStorageModifier
  // when making a copy (use NO_COPY_DATA to do this).  The p_storageModifier is handled internally
  // in SageIII.
  // DQ (4/28/2004): Use new modifier classes instead of older interface
  // InitializedName.setDataPrototype("int","storage_class_attributes", "= e_unknown_storage_class",
  //              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL);
  // InitializedName.setDataPrototype("SgStorageModifier*","storageModifier", "= NULL",
  //              NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);
     InitializedName.setDataPrototype("SgStorageModifier*","storageModifier", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, NO_COPY_DATA);
#ifdef BUILD_X_VERSION_TERMINALS
     InitializedName.setDataPrototype("SgX_DeclarationStatement*","X_declptr", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif
  // DQ (11/15/2004): class declarations for nested classes can appear outside the scope of the class to which
  // they belong, thus the parent information is not sufficent to define the relationship of nested classes
  // (and typedefs within the classes, as well, which is the current bug in Kull).  So we need an additional
  // data member to explicitly represent the scope of a class (consistant with the design of the member
  // function declaration).
     InitializedName.setDataPrototype ( "SgScopeStatement*", "scope", "= NULL",
                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (12/11/2004): Extra information when used as an entry within a preinitialization list!
  // There is perhaps a better design possible (adding an new IR node and deriving it from the InitializedName).
  // But that is a bit more complex than  think is justified at present while I experiment with this fix.
  // I need to save information about if an entry in the preinitialization list is associated with the
  // initialization of a base class or a data member.  This information (if a base class) is used to know
  // what names in the preinitialization list need to be reset (when they are template names (to replace
  // names such as "A___L9" with "A<int>").  See testcode2004_160.C.
     InitializedName.setDataPrototype("SgInitializedName::preinitialization_enum","preinitialization", "= e_unknown_preinitialization",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#if 0
  // DQ (12/21/2005): Added to support reference to global functions when locally
  // scopes ones are available (see test2005_179.C).
  // InitializedName.setDataPrototype ( "bool", "global_qualified_name", "= false",
  //           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     InitializedName.setDataPrototype ( "SgQualifiedNamePtrList", "qualifiedNameList", "",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif
#if 1
  // DQ (1/3/2006): Added attribute via ROSETTA (changed to pointer to AstAttributeMechanism)
  // Modified implementation to only be at specific IR nodes.
     InitializedName.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

  // FMZ (2/18/2009)
     InitializedName.setDataPrototype("bool","isCoArray","= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

     InitializedName.setFunctionPrototype      ( "HEADER_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
     InitializedName.setFunctionSource         ( "SOURCE_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
#endif

  // DQ (7/25/2006): Support for asm register names (required for asm statement support common in some standard Linux header files).
     InitializedName.setDataPrototype ( "SgInitializedName::asm_register_name_enum", "register_name_code", "= SgInitializedName::e_invalid_register",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (8/09/2006): Support for asm register names when defined via strings (more general than the EDG mapping to the GNU supported register names)
  // This requirement comes from an Elsa test case: "int foo asm ("myfoo") = 2;" where the register name is unknown and so held as a string.
     InitializedName.setDataPrototype ( "std::string", "register_name_string", "= \"\"",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (12/20/2006): Record if global name qualification is required on the type.
  // See test2003_01.C for an example of where this is required. Note that for a
  // variable declaration (SgVariableDeclaration) this information is recorded directly
  // on the SgVariableDeclaration node.  This use on the InitializedName is reserved for
  // function parameters, and I am not sure if it is useful anyhwere else.
     InitializedName.setDataPrototype("bool", "requiresGlobalNameQualificationOnType", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/20/2007): FORTRAN SUPPORT -- The shape of this variable (if it is an array) was not specified in the
  // declaration.  This is typically the result of a subsequent "allocatable statement" after the declaration.
  // The allocatable statement specifies the shape of an array and that it is appropriate for use with pointers (I think).
     InitializedName.setDataPrototype("bool", "shapeDeferred", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/20/2007): FORTRAN SUPPORT -- The initializer may be defined directly in the variable declaration or
  // in a separate "data statement".  This flag records if the initializer was defered (e.g. to a data statement).
     InitializedName.setDataPrototype("bool", "initializationDeferred", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (1/3/2009): Added support for gnu variable attributes.  Note that these can be specified on a per variable
  // basis and because a variable declaration can contain many variables, the attributs must live with the
  // SgInitializedName (in the future we might define aSgVariableModifier and refactor this code there).
  // Note that more than one value can be set, so this implements a bit vector of flags to be used.
     InitializedName.setDataPrototype("SgBitVector", "gnu_attribute_modifierVector", "",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (1/3/2009): Added support for GNU constructor priority (added paramter "N" as in "void f __attribute__((constructor (N)));")
     InitializedName.setDataPrototype("unsigned long int", "gnu_attribute_initialization_priority", "= 0",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (1/3/2009): Added support for GNU attributes
     InitializedName.setDataPrototype("std::string", "gnu_attribute_named_weak_reference", "=\"\"",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (1/3/2009): Added support for GNU attributes
     InitializedName.setDataPrototype("std::string", "gnu_attribute_named_alias", "=\"\"",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (1/3/2009): Added support for GNU attributes
     InitializedName.setDataPrototype("std::string", "gnu_attribute_cleanup_function", "=\"\"",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (1/3/2009): Added support for GNU attributes
     InitializedName.setDataPrototype("std::string", "gnu_attribute_section_name", "=\"\"",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (1/3/2009): Added support for alignment specfication using gnu attributes (zero is used as the default to imply no alignment specification).
     InitializedName.setDataPrototype("unsigned long int", "gnu_attribute_alignment", "= 0",
                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (1/3/2009): Added support for GNU attributes (reuse the enum declaration at the SgDeclarationModifier IR node).
     InitializedName.setDataPrototype("SgDeclarationModifier::gnu_declaration_visability_enum", "gnu_attribute_visability","= SgDeclarationModifier::e_unknown_visibility",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (9/11/2010): Added support for fortran "protected" marking of variables.
     InitializedName.setDataPrototype("bool", "protected_declaration", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


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

  // Skip building a parse function for this terminal/nonterminal of the Grammar
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

  // DQ (10/10/2006): Support for reference to a list that would be used for qualified name generation for any type.
     Unparse_Info.setDataPrototype ( "SgQualifiedNamePtrList", "qualifiedNameList", "= SgQualifiedNamePtrList()",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (10/20/2006): Added support for output of function calls.  We need to know if we are
  // unparsing a function call so that we can add the name qualification properly.  The function
  // call expression (SgFunctionCallExp) is used to query the AST as ot the relative position of
  // any defining or nondefining function declarations that would define the scope of the function.
     Unparse_Info.setDataPrototype("SgFunctionCallExp*","current_function_call","= NULL",
                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (5/22/2007): Added scope information so that we could lookup hidden list to get qualified names correct.
     Unparse_Info.setDataPrototype("SgScopeStatement*","current_scope","= NULL",
                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     BaseClass.setFunctionPrototype           ( "HEADER_BASECLASS", "../Grammar/Support.code");

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

  // DQ (11/7/2007): This should not be shared when a copy is make (see copytest_2007_26.C).
  // DQ (4/25/2004): New interfce for modifiers (forced to make this a pointer to a SgBaseClassModifier
  //                 because it could not be specified before the declaration of BaseClass, limitations
  //                 in ROSETTA).
  // BaseClass.setDataPrototype               ( "SgBaseClassModifier*", "baseClassModifier", "= NULL",
  //              NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);
     BaseClass.setDataPrototype               ( "SgBaseClassModifier*", "baseClassModifier", "= NULL",
                 NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);

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
     Pragma.setDataPrototype  ( "Sg_File_Info*", "startOfConstruct", "= NULL",
                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);
     Pragma.setDataPrototype ( "short" , "printed", "= 0",
                 NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (1/3/2006): Added attribute via ROSETTA (changed to pointer to AstAttributeMechanism)
  // Modified implementation to only be at specific IR nodes.
     Pragma.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
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

  // DQ (1/3/2006): Added attribute via ROSETTA (changed to pointer to AstAttributeMechanism)
  // Modified implementation to only be at specific IR nodes.
     Directory.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     Directory.setFunctionPrototype      ( "HEADER_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
     Directory.setFunctionSource         ( "SOURCE_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");


     FileList.setDataPrototype          ( "SgFilePtrList", "listOfFiles", "",
  //             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  //             NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
                 NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (1/23/2010): Not clear if we should put attributes here, but for now it is OK.
  // This is presently need to avoid an error in the roseHPCToolkitTests, but there may be 
  // a better way to avoid that error.
     FileList.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
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
     File.setDataPrototype         ( "Sg_File_Info*" , "startOfConstruct", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);


  // DQ (9/2/2008): We want to move this to be in the SgSourceFile
#if 0
     File.setDataPrototype         ( "SgGlobal*", "root", "= NULL",
                                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#else
     SourceFile.setDataPrototype   ( "SgGlobal*", "globalScope", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (10/1/2008): Added support for lists of SgModuleStatement where they are
  // not a part of the current translation unit.  A place for the information in
  // the *.mod files to be put.  This is the data member for the list.
     SourceFile.setDataPrototype   ( "SgModuleStatementPtrList", "module_list", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

#if 0
  // DQ (9/12/2009): Adding support for new name qualification (not ready yet).
  // DQ (9/11/2009): Added support for mapping id numbers to statement pointers.
  // This is part of support for a new implementation of name qualification.
     SourceFile.setDataPrototype   ( "SgStatementPtrList", "statementNumberContainer", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

     UnknownFile.setDataPrototype   ( "SgGlobal*", "globalScope", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

#endif

  // DQ (10/16/2005): Added to support C++ style argument handling in SgFile
  // File.setDataPrototype("std::list<std::string>","originalCommandLineArgumentList", "",
     File.setDataPrototype("SgStringList","originalCommandLineArgumentList", "",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

#if !ROSE_MICROSOFT_OS
// DQ (4/25/2009): Must fix code in sageInterface/sageBuilder.C before we can use the proper BUILD_LIST_ACCESS_FUNCTIONS macro above.
#warning "This should be using the BUILD_LIST_ACCESS_FUNCTIONS"
#endif

  // Modified ROSE to hold variables into the File object
  // DQ (8/10/2004): modified to be an int instead of a bool
     File.setDataPrototype         ( "int", "verbose", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (12/8/2007): Added support to control warnings in front-end (specifically for Fortran support
  // to control use of warnings mode in syntax checking pass using gfortran prior to calling OFP).
     File.setDataPrototype         ( "bool", "output_warnings", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Controls language mode (should support strict variants as well)
     File.setDataPrototype         ( "bool", "C_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // Liao (6/6/2008): Support for UPC model of C , 6/19/2008: add support for static threads compilation
     File.setDataPrototype         ( "bool", "UPC_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (9/19/2010): Added support to control use of C++ internal support when using UPC (which defines initial work for UPC++)
     File.setDataPrototype         ( "bool", "UPCxx_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "int", "upc_threads", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     File.setDataPrototype         ( "bool", "C99_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "Cxx_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (8/11/2007): Support for Fortran (Fortran, f77, f90, f95, f03)
     File.setDataPrototype         ( "bool", "Fortran_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "F77_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "F90_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "F95_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "F2003_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (1/23/2009): Co-Array Fortran (CAF) support
     File.setDataPrototype         ( "bool", "CoArrayFortran_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // PHP support
     File.setDataPrototype         ( "bool", "PHP_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // TV (05/17/2010) Cuda support
     File.setDataPrototype         ( "bool", "Cuda_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // TV (05/17/2010) OpenCL support
     File.setDataPrototype         ( "bool", "OpenCL_only", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

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

  // DQ (4/7/2010): This permits less agressive syntax checking, but still some syntax checking.
  // Some F90 code cannot be passed through the gfortran syntax check using "-std=f95" so we have
  // to relax this to "-std=gnu" or skip the option altogether.
     File.setDataPrototype         ( "bool", "relax_syntax_check", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Operational options
  // File.setDataPrototype         ( "bool", "skip_rose", "= false",
  //             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
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
     File.setDataPrototype         ( "SgFile::outputLanguageOption_enum", "outputLanguage", "= SgFile::e_default_output_language",
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

  // DQ (2/2/2003): Added to support -E and -H options (calling the backend directly)
     File.setDataPrototype         ( "bool", "useBackendOnly", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (2/12/2004): Added to support -c on compiler command line
     File.setDataPrototype("bool","compileOnly", "= false",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (2/13/2004): Added to support to save Edg command line
     File.setDataPrototype("std::string","savedEdgCommandLine", "= \"\"",
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

  // DQ (1/3/2006): Added attribute via ROSETTA (changed to pointer to AstAttributeMechanism)
  // Modified implementation to only be at specific IR nodes.
     File.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
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
     File.setDataPrototype         ( "bool", "sourceFileUsesCoArrayFortranFileExtension", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "sourceFileUsesPHPFileExtension", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "sourceFileUsesBinaryFileExtension", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     File.setDataPrototype         ( "bool", "sourceFileTypeIsUnknown", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // To be consistant with the use of binaryFile we will implement get_binaryFile() and set_binaryFile()
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

  // DQ (1/10/2009): The C language ASM statements are providing significant trouble, they are
  // frequently machine specific and we are compiling then on architectures for which they were
  // not designed.  This option allows then to be read, constructed in the AST to support analysis
  // but not unparsed in the code given to the backend compiler, since this can fail. (See
  // test2007_20.C from Linux Kernel for an example).
     File.setDataPrototype         ( "bool", "skip_unparse_asm_commands", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

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

  // RPM (12/29/2009): Switch to control how aggressive the disassembler is. It takes a list of words based loosely
  // on the constants in the Disassembler::SearchHeuristic enum.
     File.setDataPrototype("unsigned", "disassemblerSearchHeuristics", "= Disassembler::SEARCH_DEFAULT",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // RPM (1/5/2010): Switch to control how the Partitioner looks for functions. It takes a list of words based loosely
  // on the constants in the SgAsmFunctionDeclaration::FunctionReason enum.
     File.setDataPrototype("unsigned", "partitionerSearchHeuristics", "= SgAsmFunctionDeclaration::FUNC_DEFAULT",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // RPM (6/9/2010): Switch to specify the IPD file for the Partitioner.
     File.setDataPrototype("std::string", "partitionerConfigurationFileName", "",
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

  // This could be defined using a map of node attributes in the SgGraph class.
     GraphNode.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

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

  // This could be defined using a map of edge attributes in the SgGraph class.
     GraphEdge.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
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

  // Attribute support for SgGraph
     Graph.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);


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
     Project.setDataPrototype("int","backendErrorCode", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

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

  // DQ (10/16/2005): Added support to detect use of "-E" on compiler's command line.
  // Special processing is done when "-E" is used with multiple file names on the command line.
     Project.setDataPrototype         ( "bool", "C_PreprocessorOnly", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

#if 1
  // DQ (1/3/2006): Added attribute via ROSETTA (changed to pointer to AstAttributeMechanism)
  // Modified implementation to only be at specific IR nodes.
     Project.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
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
     Project.setDataPrototype ( "bool", "Fortran_only", "= false",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (10/3/2010): Adding support for having CPP directives explicitly in the AST (as IR nodes instead of handled similar to comments).
     Project.setDataPrototype ( "bool", "addCppDirectivesToAST", "= false",
            NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

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
     DeclarationModifier.setDataPrototype("std::string", "gnu_attribute_section_name", "=\"\"",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     DeclarationModifier.setDataPrototype("SgDeclarationModifier::gnu_declaration_visability_enum", "gnu_attribute_visability","= SgDeclarationModifier::e_unknown_visibility",
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

  // DQ (1/3/2009): Added support for alignment specfication using gnu attributes (zero is used as the default to imply no alignment specification).
     TypeModifier.setDataPrototype("unsigned long int", "gnu_attribute_alignment", "= 0",
                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (1/3/2009): This is used for funtion types only. I reserve values less than zero (-1 implies that
  // this was not set, default value). Note that the standard might require this to be unsigned, but I
  // would like to avoid the EDG tick of shifting the value by one to reserve zero to be the default.
     TypeModifier.setDataPrototype("long", "gnu_attribute_sentinel", "= -1",
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

  // MK (8/2/05) : This set contains a list of file ids. During unparsing, if we encounter
  //               a node with this Sg_File_Info object, we only want to unparse this file
  //               if the file we are currently unparsing is in this list.
  //               NOTE: this set should be empty unless the node is marked as shared
  //! This set contains a list of all file ids for which the accompanying node should be unparsed
  // File_Info.setDataPrototype("std::set<int>","fileIDsToUnparse","",
     File_Info.setDataPrototype("SgFileIdList","fileIDsToUnparse","",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

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
     TemplateParameter.setDataPrototype     ( "SgTemplateDeclaration*", "templateDeclaration", "= NULL",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     TemplateParameter.setDataPrototype     ( "SgTemplateDeclaration*", "defaultTemplateDeclarationParameter", "= NULL",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     TemplateArgument.setFunctionPrototype ( "HEADER_TEMPLATE_ARGUMENT", "../Grammar/Support.code");
     TemplateArgument.setDataPrototype     ( "SgTemplateArgument::template_argument_enum"   , "argumentType", "= argument_undefined",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     TemplateArgument.setDataPrototype     ( "bool"   , "isArrayBoundUnknownType", "= false",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     TemplateArgument.setDataPrototype     ( "SgType*", "type", "= NULL",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Could be an array bound (integer) or some unkown type
     TemplateArgument.setDataPrototype     ( "SgExpression*", "expression", "= NULL",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ: Case of a template specialization (and partial specialization) not handled
  // here (not clear on how to do this)
  // TemplateArgument.setDataPrototype     ( "SgTemplateInstantiationDecl*", "templateInstantiation", "= NULL",
  //              CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     TemplateArgument.setDataPrototype     ( "SgTemplateDeclaration*", "templateDeclaration", "= NULL",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (2/27/2005): Support for recognition of default template arguments
  // (required to fix bug demonstrated in test2005_12.C)
     TemplateArgument.setDataPrototype     ( "bool", "explicitlySpecified", "= true",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

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

     CommonBlockObject.setFunctionPrototype ( "HEADER_COMMON_BLOCK_OBJECT", "../Grammar/Support.code");
     CommonBlockObject.setDataPrototype     ( "std::string", "block_name", "=\"\"",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     CommonBlockObject.setDataPrototype     ( "SgExprListExp*", "variable_reference_list", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
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

     InitializedName.setFunctionSource ( "SOURCE_INITIALIZED_NAME", "../Grammar/Support.code");
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

     Project.setFunctionSource         ( "SOURCE_APPLICATION_PROJECT", "../Grammar/Support.code");
     Options.setFunctionSource         ( "SOURCE_OPTIONS", "../Grammar/Support.code");
     Unparse_Info.setFunctionSource    ( "SOURCE_UNPARSE_INFO", "../Grammar/Support.code");
     BaseClass.setFunctionSource       ( "SOURCE_BASECLASS", "../Grammar/Support.code");

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
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);

     FunctionParameterTypeList.setFunctionPrototype ( "HEADER_FUNCTION_PARAMETER_TYPE_LIST", "../Grammar/Support.code" );
     FunctionParameterTypeList.setFunctionSource    ( "SOURCE_FUNCTION_PARAMETER_TYPE_LIST", "../Grammar/Support.code" );
     FunctionParameterTypeList.setAutomaticGenerationOfConstructor(false);
  // DQ (12/4/2004): Now we automate the generation of the destructors
  // FunctionParameterTypeList.setAutomaticGenerationOfDestructor (true);
     FunctionParameterTypeList.setDataPrototype("SgTypePtrList","arguments","",
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);

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

  // DQ (11/21/2007): support for common block statements
     CommonBlockObject.setFunctionSource ( "SOURCE_COMMON_BLOCK_OBJECT", "../Grammar/Support.code");

     DimensionObject.setFunctionSource ( "SOURCE_DIMENSION_OBJECT", "../Grammar/Support.code");

  // DQ (11/20/2007): Added support for Fortran data statement
     DataStatementGroup.setFunctionSource  ( "SOURCE_DATA_STATEMENT_GROUP",  "../Grammar/Support.code");
     DataStatementObject.setFunctionSource ( "SOURCE_DATA_STATEMENT_OBJECT", "../Grammar/Support.code");
     DataStatementValue.setFunctionSource  ( "SOURCE_DATA_STATEMENT_VALUE",  "../Grammar/Support.code");


  // Some functions we want to only be defined for higher level grammars (not the root C++ grammar)
     ROSE_ASSERT(InitializedName.associatedGrammar != NULL);
  // ROSE_ASSERT(InitializedName.associatedGrammar->getParentGrammar() != NULL);
  // ROSE_ASSERT(InitializedName.associatedGrammar->getParentGrammar()->getGrammarName() != NULL);
     printf ("### InitializedName.associatedGrammar->getParentGrammar()->getGrammarName() = %s \n",
          (InitializedName.associatedGrammar->getParentGrammar() == NULL) ?
               "ROOT GRAMMAR" :
               InitializedName.associatedGrammar->getParentGrammar()->getGrammarName().c_str());
#if 0
     if (InitializedName.associatedGrammar->isRootGrammar() == false)
        {
          printf ("ADDING SPECIAL X VERSION SOURCE CODE!!! \n");
          InitializedName.setFunctionSource ( "SOURCE_X_INITIALIZED_NAME", "../Grammar/Support.code");
        }
       else
        {
          printf ("NOT ADDING SPECIAL X VERSION SOURCE CODE!!! \n");
        }
#endif

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




















