
#include "ROSETTA_macros.h"
#include "grammar.h"
#include "terminal.h"

// What should be the behavior of the default constructor for Grammar

void
Grammar::setUpNodes ()
   {
  // This function sets up the type system for the grammar.  In this case it implements the
  // C++ grammar, but this will be modified to permit all grammars to contain elements of the
  // C++ grammar.  Modified grammars will add and subtract elements from this default C++ grammar.

  // print out all the available nonterminals (error checking/debugging)
  // terminalList.display("Called from Grammar::setUpNodes()");
  // nonTerminalList.display("Called from Grammar::setUpNodes()");

  // printf ("Exiting after test in Grammar::setUpNodes()! \n");

     Terminal & Expression = *lookupTerminal(terminalList, "Expression");
     Terminal & Statement  = *lookupTerminal(terminalList, "Statement");
  // Liao 11/1/2010, move SgInitializedName to SgLocatedNode
     NEW_TERMINAL_MACRO (InitializedName, "InitializedName", "InitializedNameTag" );

#if USE_OMP_IR_NODES  // Liao, 5/30/2009 add nodes for OpenMP Clauses, 
 // they have source position info and should be traversed
     // add all terminals first, then bottom-up traverse class hierarchy to define non-terminals
     /*
         Class hierarchy
           SgOmpClause  {define all enum types here}
             // simplest clause
           * SgOmpOrderedClause
           * SgOmpNowaitClause
           * SgOmpUntiedClause
             // with some value
           * SgOmpDefaultClause
             // with kind, chunksize
           * SgOmpScheduleClause
              //with expression
           * SgOmpExpressionClause
           ** SgOmpCollapseClause
           ** SgOmpIfClause
           ** SgOmpNumThreadsClause
             // with variable list
           * SgOmpVariablesClause
           ** SgOmpCopyprivateClause
           ** SgOmpPrivateClause
           ** SgOmpFirstprivateClause
           ** SgOmpSharedClause
           ** SgOmpCopyInClause
           ** SgOmpLastprivateClause
              // reduction, op : list
           *** SgOmpReductionClause
        */
     NEW_TERMINAL_MACRO (OmpOrderedClause, "OmpOrderedClause", "OmpOrderedClauseTag" );
     NEW_TERMINAL_MACRO (OmpNowaitClause, "OmpNowaitClause", "OmpNowaitClauseTag" );
     NEW_TERMINAL_MACRO (OmpUntiedClause, "OmpUntiedClause", "OmpUntiedClauseTag" );
     NEW_TERMINAL_MACRO (OmpDefaultClause, "OmpDefaultClause", "OmpDefaultClauseTag" );

     NEW_TERMINAL_MACRO (OmpCollapseClause, "OmpCollapseClause", "OmpCollapseClauseTag" );
     NEW_TERMINAL_MACRO (OmpIfClause, "OmpIfClause", "OmpIfClauseTag" );
     NEW_TERMINAL_MACRO (OmpNumThreadsClause, "OmpNumThreadsClause", "OmpNumThreadsClauseTag" );
     NEW_NONTERMINAL_MACRO (OmpExpressionClause, OmpCollapseClause | OmpIfClause | OmpNumThreadsClause
         ,"OmpExpressionClause", "OmpExpressionClauseTag",false );

     NEW_TERMINAL_MACRO (OmpCopyprivateClause, "OmpCopyprivateClause", "OmpCopyprivateClauseTag" );
     NEW_TERMINAL_MACRO (OmpPrivateClause, "OmpPrivateClause", "OmpPrivateClauseTag" );
     NEW_TERMINAL_MACRO (OmpFirstprivateClause, "OmpFirstprivateClause", "OmpFirstprivateClauseTag" );
     NEW_TERMINAL_MACRO (OmpSharedClause, "OmpSharedClause", "OmpSharedClauseTag" );
     NEW_TERMINAL_MACRO (OmpCopyinClause, "OmpCopyinClause", "OmpCopyinClauseTag" );
     NEW_TERMINAL_MACRO (OmpLastprivateClause, "OmpLastprivateClause", "OmpLastprivateClauseTag" );
     NEW_TERMINAL_MACRO (OmpReductionClause, "OmpReductionClause", "OmpReductionClauseTag" );

     NEW_NONTERMINAL_MACRO (OmpVariablesClause, OmpCopyprivateClause| OmpPrivateClause |OmpFirstprivateClause|
         OmpSharedClause |OmpCopyinClause| OmpLastprivateClause| OmpReductionClause,
         "OmpVariablesClause", "OmpVariablesClauseTag", false);

     NEW_TERMINAL_MACRO (OmpScheduleClause, "OmpScheduleClause", "OmpScheduleClauseTag" );

     NEW_NONTERMINAL_MACRO (OmpClause, OmpOrderedClause | OmpNowaitClause | OmpUntiedClause |
         OmpDefaultClause | OmpExpressionClause | OmpVariablesClause | OmpScheduleClause ,
         "OmpClause", "OmpClauseTag", false);
#endif
     
  // DQ (10/3/2008): Support for the Fortran "USE" statement and its rename list option.
     NEW_TERMINAL_MACRO (RenamePair,     "RenamePair",     "TEMP_Rename_Pair" );

  // DQ (10/6/2008): Support for the Fortran "USE" statement and its rename list option.
     NEW_TERMINAL_MACRO (InterfaceBody,  "InterfaceBody",  "TEMP_Interface_Body" );


  // DQ (10/6/2008): Migrate some of the SgSupport derived IR nodes, that truely have a position in the 
  // source code, to SgLocatedNode.  Start with some of the newer IR nodes which are traversed and thus 
  // are forced to have an interface for the source position interface information (already present in 
  // the SgLocatedNode base class).  Eventually a number of the IR nodes currently derived from SgSupport
  // should be moved to be here (e.g. SgTemplateArgument, SgTemplateParameter, and 
  // a number of the new Fortran specific IRnodes, etc.).
     NEW_NONTERMINAL_MACRO (LocatedNodeSupport, InitializedName | InterfaceBody | RenamePair | OmpClause , "LocatedNodeSupport", "LocatedNodeSupportTag", false );

  // DQ (3/24/2007): Added support for tokens in the IR (to support threading of the token stream 
  // onto the AST as part of an alternative, and exact, form of code generation within ROSE.
  // NEW_NONTERMINAL_MACRO (LocatedNode, Expression | Statement, "LocatedNode", "LocatedNodeTag" );
     NEW_TERMINAL_MACRO (Token, "Token", "TOKEN" );
   // Liao 11/2/2010, LocatedNodeSupport is promoted to the first location since SgInitializedName's internal type is used in some Statement  
     NEW_NONTERMINAL_MACRO (LocatedNode, LocatedNodeSupport| Statement | Expression | Token, "LocatedNode", "LocatedNodeTag", false );

     Terminal & Type    = *lookupTerminal(terminalList, "Type");
     Terminal & Symbol  = *lookupTerminal(terminalList, "Symbol");
     Terminal & Support = *lookupTerminal(terminalList, "Support");

  // DQ (3/14/2007): Added IR support for binaries
     Terminal & AsmNode = *lookupTerminal(terminalList, "AsmNode");

  // printf ("nonTerminalList.size() = %zu \n",nonTerminalList.size());

  // DQ (3/14/2007): Added IR support for binaries
  // NEW_NONTERMINAL_MACRO (Node, Type | Symbol | LocatedNode | Support, "Node", "NodeTag" );
     NEW_NONTERMINAL_MACRO (Node, Support | Type | LocatedNode | Symbol | AsmNode, "Node", "NodeTag", false );
  // NEW_NONTERMINAL_MACRO (Node, Type | Symbol | LocatedNode | Support, "Node", "NodeTag" );

  // ***********************************************************************
  // ***********************************************************************
  //                       Header Code Declaration
  // ***********************************************************************
  // ***********************************************************************

  // Header declarations for Node
     Node.setPredeclarationString     ("HEADER_NODE_PREDECLARATION" , "../Grammar/Node.code");

  // DQ (3/25/2006): Put it back since we can't control the ordering of generated 
  // functions sufficently to have this be a means to document ROSE.
  // DQ (3/24/2006): Move to before common code to better organize documentation
  // Node.setFunctionPrototype        ( "HEADER", "../Grammar/Node.code");

  // MK: the following two function calls could be wrapped into a single one:
     Node.setFunctionPrototype        ( "HEADER", "../Grammar/Common.code");
     Node.setSubTreeFunctionPrototype ( "HEADER", "../Grammar/Common.code");

  // DQ (3/25/2006): Put it back since we can't control the ordering of generated 
  // functions sufficently to have this be a means to document ROSE.
  // DQ (3/24/2006): Move to before common code
     Node.setFunctionPrototype        ( "HEADER", "../Grammar/Node.code");

  // This function exists everywhere (at each node of the grammar)!
     Node.setSubTreeFunctionPrototype    ( "HEADER_IS_CLASSNAME", "../Grammar/Node.code");

  // This function exists everywhere (at each node of the grammar)!
  // Node.setSubTreeFunctionPrototype    ( "HEADER_PARSER", "../Grammar/Node.code");
  // Can't use the leafNodeList until we have built the tree!!!
  // leafNodeList.setSubTreeFunctionPrototype    ( "HEADER_PARSER", "../Grammar/Node.code");
  // leafNodeList.setFunctionPrototype    ( "HEADER_PARSER", "../Grammar/Node.code");

  // Build it everywhere for now (though it is likely only required on the leaves)
     Node.setSubTreeFunctionPrototype    ( "HEADER_PARSER", "../Grammar/Node.code");

#if 0
  // DQ (8/18/2004): This is not used so remove it! 
  // Node.excludeFunctionPrototype( "HEADER_IS_CLASSNAME", "../Grammar/Node.code");
     Node.setDataPrototype               ( "SgAttribute*","singleAttribute","= NULL",
       					   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);
  // MK: Node.excludeSubTreeDataPrototype( "SgAttribute*","attribute","= NULL");

  // MS 12/18/01: attribute support
     Node.setDataPrototype               ( "SgAttributePtrList","uattributes","",
					   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#if 0
  // DQ (8/18/2004): This is not used so remove it! 
  // (Consider using a smart pointer to provide a better implementation)
  // Added (2/24/2001) to start support for reference counting
     Node.setDataPrototype               ( "int","referenceCount","= 1",
					   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE,
                                           NO_COPY_DATA);
     Node.excludeSubTreeDataPrototype( "int","referenceCount","= 1");
#endif

  // MK: I moved the following data member declaration from ../Grammar/Node.code to this position,
  // we rely on the access functions to be defined in the .code files, maybe this should be changed;
     Node.setDataPrototype("SgNode*","parent","= NULL",
			   NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

  // DQ (7/23/2005): Let these be automatically generated by ROSETTA!
  // Opps, these can't be generated by ROSETTA, since it would result 
  // in the recursive call to set_isModified (endless recursion).
  // QY: we need a boolean flag for tracking the updates to an ast node
     Node.setDataPrototype("bool","isModified","= false",
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
 
#if 0
  // DQ (7/23/2005): Remove this flag since it is no longer used.  It is not particularly eligant to store 
  // the state associated with the traversal within the AST. Some state is required to avoid retraversal 
  // of IR nodes, but that state should be stored in the traversal directly.

  // MK: we need a boolean flag for the tree traversal
     Node.setDataPrototype("bool","isVisited","= false",
			   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     // MS: make file_info accessible in every AST node. Only set in SgLocatedNode(s) 
     //Node.setDataPrototype("Sg_File_Info*","file_info","= NULL",
     //		   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);
#endif

  // DQ (10/21/2005): Adding memory pool support variable via ROSETTA so that file I/O can be supported.
  // Node.setDataPrototype("$CLASSNAME *","freepointer","= NULL",
  //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     Node.setDataPrototype("$CLASSNAME*","freepointer","= AST_FileIO::IS_VALID_POINTER()",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

#if 0
  // DQ (4/10/2006): By deleting this from SgNode we save huge amounts of space in the Sg_File_Info
  // objects which there are a LOT of and which don't need to be given an attribute mechanism.
  // DQ (1/2/2006): Added attribute via ROSETTA (changed to pointer to AstAttributeMechanism)
     Node.setDataPrototype("AstAttributeMechanism*","attribute","= NULL",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
#endif

  // DQ (1/31/2006): We can introduce this here since it will be set in each constructor call.
  // but the trick is to set the initializer to an empty string so that the initialization in 
  // the constructor will not be output by ROSETTA.
  // DQ (1/31/2006): This is support for the single global function type table 
  // (stores all function types using mangled names).  To support this static data member
  // we have to build special version of the access functions (so that the access member functions
  // will be static as well).
  // Support for SgFunctionTypeTable* SgNode::globalFunctionTypeTable = new SgFunctionTypeTable();
  // Node.setDataPrototype("static SgFunctionTypeTable*","globalFunctionTypeTable","= new SgFunctionTypeTable()",
  //        NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     Node.setDataPrototype("static SgFunctionTypeTable*","globalFunctionTypeTable","",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

  // DQ (7/22/2010): Added support for type table supporting construction of unique types.
     Node.setDataPrototype("static SgTypeTable*","globalTypeTable","",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

  // DQ (3/12/2007): Added static mangled name map, used to improve performance of mangled name lookup.
  // Node.setDataPrototype("static SgMangledNameListPtr","globalMangledNameMap","",
  //        NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     Node.setDataPrototype("static std::map<SgNode*,std::string>","globalMangledNameMap","",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
  // DQ (6/26/2007): Added support from Jeremiah for shortened mangle names
     Node.setDataPrototype("static std::map<std::string, int>", "shortMangledNameCache", "",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

  // Not clear how to best to this, perhaps ROSETTA should define a function.
  // DQ (11/25/2007): Language classification field.  Now that we are supporting multiple languages
  // it is helpful to have a way to classify the IR nodes as to what language they belong.  Most are
  // shared (which can be the default) but many are language specific.
  // Node.setDataPrototype("static long","language_classification_bit_vector","",
  //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

     LocatedNode.setFunctionPrototype ( "HEADER", "../Grammar/LocatedNode.code");
  // LocatedNode.setDataPrototype     ( "Sg_File_Info*", "file_info", "= NULL",
  //              CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);
  // New interface functions for startOfConstruct and endOfConstruct information
     LocatedNode.setDataPrototype     ( "Sg_File_Info*", "startOfConstruct", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);
     LocatedNode.setDataPrototype     ( "Sg_File_Info*", "endOfConstruct", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);

  // DQ (7/26/2008): Any comments need to be copied to a new container (deep copy), else comments added 
  // to the copy will showup in the comments for the original AST.  Fixed as part of support for bug seeding.
  // LocatedNode.setDataPrototype     ( "AttachedPreprocessingInfoType*", "attachedPreprocessingInfoPtr", "= NULL", 
  //              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, COPY_DATA);
     LocatedNode.setDataPrototype     ( "AttachedPreprocessingInfoType*", "attachedPreprocessingInfoPtr", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);

#if 1
  // DQ (1/2/2006): Added attribute via ROSETTA (changed to pointer to AstAttributeMechanism)
  // Modified implementation to only be at specific IR nodes.

  // DQ (6/28/2008): Make this copy the attributes and define a copy function to be called to support this!
  // LocatedNode.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
  //              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
  // LocatedNode.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
  //              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     LocatedNode.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, CLONE_PTR);

     LocatedNode.setFunctionPrototype      ( "HEADER_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
     LocatedNode.setFunctionSource         ( "SOURCE_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
#endif

  // DQ (3/24/2007): Added support for tokens in the IR.
  // Token.setPredeclarationString ("HEADER_TOKEN_PREDECLARATION" , "../Grammar/LocatedNode.code");
     Token.setFunctionPrototype    ("HEADER_TOKEN", "../Grammar/LocatedNode.code");

  // DQ (3/24/2007): Should be be naming the string we hold in the token "lexeme", or is there a better name?
     Token.setDataPrototype ( "std::string", "lexeme_string", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     Token.setDataPrototype ( "unsigned int", "classification_code", "= 0",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

     LocatedNodeSupport.setFunctionPrototype ( "HEADER_LOCATED_NODE_SUPPORT", "../Grammar/LocatedNode.code");

  // DQ (10/6/2008): Moved to SgLocatedNodeSupport.
     RenamePair.setFunctionPrototype ( "HEADER_RENAME_PAIR", "../Grammar/LocatedNode.code");
     RenamePair.setDataPrototype     ( "SgName", "local_name", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     RenamePair.setDataPrototype     ( "SgName", "use_name", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (10/6/2008): This interferes with the specification in SgLocatedNode
  // RenamePair.setDataPrototype     ( "Sg_File_Info*", "startOfConstruct", "= NULL",
  //              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);

  // DQ (10/6/2008): Moved to SgLocatedNodeSupport.
  // DQ (10/6/2008): Added support for interface bodies so that we could capture the information 
  // used to specify function declaration ro function names in interface statements.
     InterfaceBody.setFunctionPrototype ( "HEADER_INTERFACE_BODY", "../Grammar/LocatedNode.code");

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

  // DQ (10/6/2008): This interferes with the specification in SgLocatedNode
  // InterfaceBody.setDataPrototype     ( "Sg_File_Info*", "startOfConstruct", "= NULL",
  //              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);
  // InterfaceBody.setDataPrototype     ( "Sg_File_Info*", "endOfConstruct", "= NULL",
  //              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);

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
  //   InitializedName.setDataPrototype ( "Sg_File_Info*", "startOfConstruct", "= NULL",
  //          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);

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
  //   InitializedName.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
  //          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

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



  // ***********************************************************************
  // ***********************************************************************
  //                       Source Code Definition
  // ***********************************************************************
  // ***********************************************************************

  // Source code for Node
     Node.setFunctionSource("SOURCE", "../Grammar/Common.code");
     Node.setSubTreeFunctionSource("SOURCE", "../Grammar/Common.code");

     Node.setFunctionSource( "SOURCE", "../Grammar/Node.code");
     Node.setFunctionSource( "SOURCE_ROOT_NODE_ERROR_FUNCTION", "../Grammar/Node.code");

     Node.setSubTreeFunctionSource( "SOURCE_ERROR_FUNCTION", "../Grammar/Node.code");
     Node.excludeFunctionSource( "SOURCE_ERROR_FUNCTION", "../Grammar/Node.code");

     Node.editSubstitute("CONSTRUCTOR_BODY"," ");

  // Parse functions are only built for the higher level grammars since they parse
  // from a lower level grammar into a higher level grammer (thus they are not defined 
  // within the root grammar (the C+ grammar)).
     if (isRootGrammar() == false)
          Node.setSubTreeFunctionSource ( "SOURCE_PARSER", "../Grammar/parserSourceCode.macro" );

  // Source code for LocatedNode
     LocatedNode.setFunctionSource( "SOURCE", "../Grammar/LocatedNode.code");
     LocatedNode.editSubstitute("CONSTRUCTOR_BODY"," ");
  // DQ (12/4/2004): Now we automate the generation of the destructors
  // LocatedNode.setAutomaticGenerationOfDestructor(false);

  // DQ (3/24/2007): Added support for tokens in the IR.
     Token.setFunctionSource ( "SOURCE_TOKEN", "../Grammar/LocatedNode.code");

     LocatedNodeSupport.setFunctionSource ( "SOURCE_LOCATED_NODE_SUPPORT", "../Grammar/LocatedNode.code");

  // DQ (10/6/2008): Moved from SgSupport.
     RenamePair.setFunctionSource ( "SOURCE_RENAME_PAIR", "../Grammar/LocatedNode.code");

  // DQ (10/6/2008): Moved from SgSupport.
     InterfaceBody.setFunctionSource ( "SOURCE_INTERFACE_BODY", "../Grammar/LocatedNode.code");

     InitializedName.setFunctionSource ( "SOURCE_INITIALIZED_NAME", "../Grammar/Support.code");
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


  // ***********************************************************************
  // ***********************************************************************
  //                       OpenMP Clauses
  // ***********************************************************************
  // ***********************************************************************


#if USE_OMP_IR_NODES     
     // supporting clause nodes
     // declared enum types within SgOmpClause
     OmpClause.setFunctionPrototype("HEADER_OMP_CLAUSE", "../Grammar/Support.code");
    
     // clauses with expressions
     OmpExpressionClause.setDataPrototype ( "SgExpression*", "expression", "= NULL",
                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, CLONE_PTR);
     // schedule (kind[, chunksize_exp])
     OmpScheduleClause.setDataPrototype("SgOmpClause::omp_schedule_kind_enum", "kind", "=e_omp_schedule_unkown",
                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     OmpScheduleClause.setDataPrototype ( "SgExpression*", "chunk_size", "= NULL",
                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, CLONE_PTR);
 
     // clauses with variable lists 
     // Liao 9/27/2010, per user's report, modeling the variable reference use SgVarRefExp
     //OmpVariablesClause.setDataPrototype ( "SgInitializedNamePtrList", "variables", "",
     OmpVariablesClause.setDataPrototype ( "SgVarRefExpPtrList", "variables", "",
                         NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

      // default (private | firstprivate | shared | none)
     OmpDefaultClause.setDataPrototype("SgOmpClause::omp_default_option_enum", "data_sharing", "=e_omp_default_unkown",
                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     // reduction(op:variables) 
     OmpReductionClause.setDataPrototype("SgOmpClause::omp_reduction_operator_enum", "operation", "=e_omp_reduction_unkown",
                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif




   } // end





















