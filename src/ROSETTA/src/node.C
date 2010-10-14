
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
  // should be moved to be here (e.g. SgInitializedName, SgTemplateArgument, SgTemplateParameter, and 
  // a number of the new Fortran specific IRnodes, etc.).
     NEW_NONTERMINAL_MACRO (LocatedNodeSupport, InterfaceBody | RenamePair | OmpClause , "LocatedNodeSupport", "LocatedNodeSupportTag", false );

  // DQ (3/24/2007): Added support for tokens in the IR (to support threading of the token stream 
  // onto the AST as part of an alternative, and exact, form of code generation within ROSE.
  // NEW_NONTERMINAL_MACRO (LocatedNode, Expression | Statement, "LocatedNode", "LocatedNodeTag" );
     NEW_TERMINAL_MACRO (Token, "Token", "TOKEN" );
     NEW_NONTERMINAL_MACRO (LocatedNode, Statement | Expression | LocatedNodeSupport | Token, "LocatedNode", "LocatedNodeTag", false );

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





















