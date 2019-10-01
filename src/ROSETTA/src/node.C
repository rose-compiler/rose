
#include "ROSETTA_macros.h"
#include "grammar.h"
#include "AstNodeClass.h"

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

     AstNodeClass & Expression = *lookupTerminal(terminalList, "Expression");
     AstNodeClass & Statement  = *lookupTerminal(terminalList, "Statement");

  // DQ (11/21/2007): This is part of support for the common block statement
     NEW_TERMINAL_MACRO (CommonBlockObject, "CommonBlockObject",     "TEMP_CommonBlockObject" );

  // Liao 11/1/2010, move SgInitializedName to SgLocatedNode
     NEW_TERMINAL_MACRO (InitializedName, "InitializedName", "InitializedNameTag" );

  // DQ (9/3/2014): Adding support for C++11 lambda expresions.
     NEW_TERMINAL_MACRO (LambdaCapture    , "LambdaCapture"    , "LambdaCaptureTag" );
     NEW_TERMINAL_MACRO (LambdaCaptureList, "LambdaCaptureList", "LambdaCaptureListTag" );

  // DQ(1/13/2014): Added Java support for JavaMemberValuePair
     NEW_TERMINAL_MACRO (JavaMemberValuePair, "JavaMemberValuePair", "JavaMemberValuePairTag" );

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
           * SgOmpBeginClause // experimental nodes for MPI begin..end segments
           * SgOmpEndClause
             // with some value
           * SgOmpDefaultClause
           * SgOmpProcBindClause
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
     NEW_TERMINAL_MACRO (OmpBeginClause, "OmpBeginClause", "OmpBeginClauseTag" );
     NEW_TERMINAL_MACRO (OmpEndClause, "OmpEndClause", "OmpEndClauseTag" );
     NEW_TERMINAL_MACRO (OmpUntiedClause, "OmpUntiedClause", "OmpUntiedClauseTag" );
     NEW_TERMINAL_MACRO (OmpMergeableClause, "OmpMergeableClause", "OmpMergeableClauseTag" );
     NEW_TERMINAL_MACRO (OmpDefaultClause, "OmpDefaultClause", "OmpDefaultClauseTag" );
     NEW_TERMINAL_MACRO (OmpAtomicClause, "OmpAtomicClause", "OmpAtomicClauseTag" );
     NEW_TERMINAL_MACRO (OmpProcBindClause, "OmpProcBindClause", "OmpProcBindClauseTag" );
     NEW_TERMINAL_MACRO (OmpInbranchClause, "OmpInbranchClause", "OmpInbranchClauseTag" );
     NEW_TERMINAL_MACRO (OmpNotinbranchClause, "OmpNotinbranchClause", "OmpNotinbranchClauseTag" );

     NEW_TERMINAL_MACRO (OmpCollapseClause, "OmpCollapseClause", "OmpCollapseClauseTag" );
     NEW_TERMINAL_MACRO (OmpIfClause, "OmpIfClause", "OmpIfClauseTag" );
     NEW_TERMINAL_MACRO (OmpFinalClause, "OmpFinalClause", "OmpFinalClauseTag" );
     NEW_TERMINAL_MACRO (OmpPriorityClause, "OmpPriorityClause", "OmpPriorityClauseTag" );
     NEW_TERMINAL_MACRO (OmpNumThreadsClause, "OmpNumThreadsClause", "OmpNumThreadsClauseTag" );
     NEW_TERMINAL_MACRO (OmpDeviceClause, "OmpDeviceClause", "OmpIfDeviceTag" );
     NEW_TERMINAL_MACRO (OmpSafelenClause, "OmpSafelenClause", "OmpSafelenTag" );
     NEW_TERMINAL_MACRO (OmpSimdlenClause, "OmpSimdlenClause", "OmpSimdlenTag" );

     NEW_NONTERMINAL_MACRO (OmpExpressionClause, OmpOrderedClause | OmpCollapseClause | OmpIfClause | OmpNumThreadsClause | OmpDeviceClause |
                            OmpSafelenClause | OmpSimdlenClause | OmpFinalClause | OmpPriorityClause
         ,"OmpExpressionClause", "OmpExpressionClauseTag",false );


     NEW_TERMINAL_MACRO (OmpCopyprivateClause, "OmpCopyprivateClause", "OmpCopyprivateClauseTag" );
     NEW_TERMINAL_MACRO (OmpPrivateClause, "OmpPrivateClause", "OmpPrivateClauseTag" );
     NEW_TERMINAL_MACRO (OmpFirstprivateClause, "OmpFirstprivateClause", "OmpFirstprivateClauseTag" );
     NEW_TERMINAL_MACRO (OmpSharedClause, "OmpSharedClause", "OmpSharedClauseTag" );
     NEW_TERMINAL_MACRO (OmpCopyinClause, "OmpCopyinClause", "OmpCopyinClauseTag" );
     NEW_TERMINAL_MACRO (OmpLastprivateClause, "OmpLastprivateClause", "OmpLastprivateClauseTag" );
     NEW_TERMINAL_MACRO (OmpReductionClause, "OmpReductionClause", "OmpReductionClauseTag" );
     NEW_TERMINAL_MACRO (OmpDependClause,    "OmpDependClause", "OmpDependClauseTag" );

     NEW_TERMINAL_MACRO (OmpMapClause, "OmpMapClause", "OmpMapClauseTag" );
     NEW_TERMINAL_MACRO (OmpLinearClause, "OmpLinearClause", "OmpLinearClauseTag" );
     NEW_TERMINAL_MACRO (OmpUniformClause, "OmpUniformClause", "OmpUniformClauseTag" );
     NEW_TERMINAL_MACRO (OmpAlignedClause, "OmpAlignedClause", "OmpAlignedClauseTag" );

     NEW_NONTERMINAL_MACRO (OmpVariablesClause, OmpCopyprivateClause| OmpPrivateClause |OmpFirstprivateClause|
         OmpSharedClause |OmpCopyinClause| OmpLastprivateClause| OmpReductionClause | OmpMapClause | 
         OmpUniformClause | OmpAlignedClause | OmpLinearClause | OmpDependClause ,
         "OmpVariablesClause", "OmpVariablesClauseTag", false);

     NEW_TERMINAL_MACRO (OmpScheduleClause, "OmpScheduleClause", "OmpScheduleClauseTag" );

     NEW_NONTERMINAL_MACRO (OmpClause, OmpNowaitClause | OmpBeginClause |OmpEndClause | OmpUntiedClause |
         OmpDefaultClause | OmpAtomicClause | OmpProcBindClause | OmpExpressionClause | OmpInbranchClause | OmpNotinbranchClause |
         OmpVariablesClause | OmpScheduleClause | OmpMergeableClause ,
         "OmpClause", "OmpClauseTag", false);
#endif
     
  // DQ (10/3/2008): Support for the Fortran "USE" statement and its rename list option.
     NEW_TERMINAL_MACRO (RenamePair,     "RenamePair",     "TEMP_Rename_Pair" );

  // DQ (10/6/2008): Support for the Fortran "USE" statement and its rename list option.
     NEW_TERMINAL_MACRO (InterfaceBody,  "InterfaceBody",  "TEMP_Interface_Body" );

  // negara1 (08/10/2011): Support for included files (i.e. headers) bodies.
     NEW_TERMINAL_MACRO (HeaderFileBody,  "HeaderFileBody",  "TEMP_Header_File_Body" );


  // DQ (11/26/2013): Moved SgToken to be before the UntypedNode IR nodes.
  // NEW_TERMINAL_MACRO (Token, "Token", "TOKEN" );

  // ***************************************************************************************
  // ***************************************************************************************
  //                                 Untyped IR Node Support
  // ***************************************************************************************
  // ***************************************************************************************
  // DQ (11/26/2013): Adding support for untyped AST IR nodes to support translation of ATterm 
  // based untyped ASTs into ROSE so that we will have tools (inherited attribute and synthizied 
  // attribute traversals) from which to build the ROSE AST (typed AST) and define a proper frontend.

  // Additional IR nodes that we expect to require for expressions:
  // Rasmussen (09/04/2018): Added SgUntypedNamedExpression
     NEW_TERMINAL_MACRO (UntypedUnaryOperator,            "UntypedUnaryOperator",            "TEMP_UntypedUnaryOperator" );
     NEW_TERMINAL_MACRO (UntypedBinaryOperator,           "UntypedBinaryOperator",           "TEMP_UntypedBinaryOperator" );
     NEW_TERMINAL_MACRO (UntypedExprListExpression,       "UntypedExprListExpression",       "TEMP_UntypedExprListExpression" );
     NEW_TERMINAL_MACRO (UntypedNamedExpression,          "UntypedNamedExpression",          "TEMP_UntypedNamedExpression" );
     NEW_TERMINAL_MACRO (UntypedNullExpression,           "UntypedNullExpression",           "TEMP_UntypedNullExpression" );
     NEW_TERMINAL_MACRO (UntypedValueExpression,          "UntypedValueExpression",          "TEMP_UntypedValueExpression" );
     NEW_TERMINAL_MACRO (UntypedArrayReferenceExpression, "UntypedArrayReferenceExpression", "TEMP_UntypedArrayReferenceExpression" );
     NEW_TERMINAL_MACRO (UntypedSubscriptExpression,      "UntypedSubscriptExpression",      "TEMP_UntypedSubscriptExpression" );
     NEW_TERMINAL_MACRO (UntypedOtherExpression,          "UntypedOtherExpression",          "TEMP_UntypedOtherExpression" );
     NEW_TERMINAL_MACRO (UntypedFunctionCallOrArrayReferenceExpression, "UntypedFunctionCallOrArrayReferenceExpression",  "TEMP_UntypedFunctionCallOrArrayReferenceExpression" );

     NEW_TERMINAL_MACRO (UntypedReferenceExpression, "UntypedReferenceExpression", "TEMP_UntypedReferenceExpression" );

  // DQ (1/22/2016): Allow this IR node to be used explicitly in the AST.
  // NEW_NONTERMINAL_MACRO (UntypedExpression, UntypedUnaryOperator | UntypedBinaryOperator | UntypedValueExpression | 
  //     UntypedArrayReferenceExpression | UntypedOtherExpression | UntypedFunctionCallOrArrayReferenceExpression | 
  //     UntypedReferenceExpression, "UntypedExpression", "UntypedExpressionTag", false);
  // Rasmussen (12/20/2017): Added SgUntypedExprListExpression and SgUntypedNullExpression
  // Rasmussen (01/22/2018): Added SgUntypedSubscriptExpression
  // Rasmussen (09/04/2018): Added SgUntypedNamedExpression
     NEW_NONTERMINAL_MACRO (UntypedExpression,
         UntypedUnaryOperator            | UntypedBinaryOperator  | UntypedExprListExpression  | UntypedValueExpression |
         UntypedArrayReferenceExpression | UntypedOtherExpression | UntypedFunctionCallOrArrayReferenceExpression       |
         UntypedSubscriptExpression      | UntypedNamedExpression | UntypedNullExpression      | UntypedReferenceExpression,
         "UntypedExpression", "UntypedExpressionTag", true);

     NEW_TERMINAL_MACRO (UntypedNameListDeclaration,      "UntypedNameListDeclaration",      "TEMP_UntypedNameListDeclaration" );
     NEW_TERMINAL_MACRO (UntypedImplicitDeclaration,      "UntypedImplicitDeclaration",      "TEMP_UntypedImplicitDeclaration" );
     NEW_TERMINAL_MACRO (UntypedVariableDeclaration,      "UntypedVariableDeclaration",      "TEMP_UntypedVariableDeclaration" );
     NEW_TERMINAL_MACRO (UntypedProgramHeaderDeclaration, "UntypedProgramHeaderDeclaration", "TEMP_UntypedProgramHeaderDeclaration" );
     NEW_TERMINAL_MACRO (UntypedSubroutineDeclaration,    "UntypedSubroutineDeclaration",    "TEMP_UntypedSubroutineDeclaration" );
     NEW_TERMINAL_MACRO (UntypedInterfaceDeclaration,     "UntypedInterfaceDeclaration",     "TEMP_UntypedInterfaceDeclaration" );
     NEW_TERMINAL_MACRO (UntypedUseStatement,             "UntypedUseStatement",             "TEMP_UntypedUseStatement" );

  // Rasmussen (8/06/2018): Added SgUntypedInitializedNameListDeclaration
     NEW_TERMINAL_MACRO (UntypedInitializedNameListDeclaration, "UntypedInitializedNameListDeclaration",
                                                                "TEMP_UntypedInitializedNameListDeclaration");

  // DQ (10/3/2017): New statements specific to general language support.  The philosophy is to add the union of all statements
  // for all languages to the set of untyped IR nodes and provide an enum code to support the larger number of expressions.
  // Rasmussen (1/16/2018): Added UntypedNullDeclaration
  // Rasmussen (5/28/2018): Added UntypedLabelStatement
  // Rasmussen (6/ 5/2018): Added UntypedStopStatement
  // Rasmussen (10/2/2018): Added UntypedForAllStatement
     NEW_TERMINAL_MACRO (UntypedNullDeclaration,               "UntypedNullDeclaration",               "TEMP_UntypedNullDeclaration" );
     NEW_TERMINAL_MACRO (UntypedNullStatement,                 "UntypedNullStatement",                 "TEMP_UntypedNullStatement" );
     NEW_TERMINAL_MACRO (UntypedLabelStatement,                "UntypedLabelStatement",                "TEMP_UntypedLabelStatement" );
     NEW_TERMINAL_MACRO (UntypedIfStatement,                   "UntypedIfStatement",                   "TEMP_UntypedIfStatement" );
     NEW_TERMINAL_MACRO (UntypedCaseStatement,                 "UntypedCaseStatement",                 "TEMP_UntypedCaseStatement" );
     NEW_TERMINAL_MACRO (UntypedLoopStatement,                 "UntypedLoopStatement",                 "TEMP_UntypedLoopStatement" );
     NEW_TERMINAL_MACRO (UntypedWhileStatement,                "UntypedWhileStatement",                "TEMP_UntypedWhileStatement" );
     NEW_TERMINAL_MACRO (UntypedForStatement,                  "UntypedForStatement",                  "TEMP_UntypedForStatement" );
     NEW_TERMINAL_MACRO (UntypedForAllStatement,               "UntypedForAllStatement",               "TEMP_UntypedForAllStatement" );
  // NEW_TERMINAL_MACRO (UntypedBlockStatement,                "UntypedBlockStatement",                "TEMP_UntypedBlockStatement" );
     NEW_TERMINAL_MACRO (UntypedExitStatement,                 "UntypedExitStatement",                 "TEMP_UntypedExitStatement" );
     NEW_TERMINAL_MACRO (UntypedGotoStatement,                 "UntypedGotoStatement",                 "TEMP_UntypedGotoStatement" );
     NEW_TERMINAL_MACRO (UntypedProcedureCallStatement,        "UntypedProcedureCallStatement",        "TEMP_UntypedProcedureCallStatement" );
     NEW_TERMINAL_MACRO (UntypedReturnStatement,               "UntypedReturnStatement",               "TEMP_UntypedReturnStatement" );
     NEW_TERMINAL_MACRO (UntypedExtendedReturnStatement,       "UntypedExtendedReturnStatement",       "TEMP_UntypedExtendedReturnStatement" );
     NEW_TERMINAL_MACRO (UntypedStopStatement,                 "UntypedStopStatement",                 "TEMP_UntypedStopStatement" );
     NEW_TERMINAL_MACRO (UntypedAcceptStatement,               "UntypedAcceptStatement",               "TEMP_UntypedAcceptStatement" );
     NEW_TERMINAL_MACRO (UntypedEntryCallStatement,            "UntypedEntryCallStatement",            "TEMP_UntypedEntryCallStatement" );
     NEW_TERMINAL_MACRO (UntypedRequeueStatement,              "UntypedRequeueStatement",              "TEMP_UntypedRequeueStatement" );
     NEW_TERMINAL_MACRO (UntypedDelayUntilStatement,           "UntypedDelayUntilStatement",           "TEMP_UntypedDelayUntilStatement" );
     NEW_TERMINAL_MACRO (UntypedDelayRelativeStatement,        "UntypedDelayRelativeStatement",        "TEMP_UntypedDelayRelativeStatement" );
     NEW_TERMINAL_MACRO (UntypedTerminateAlternativeStatement, "UntypedTerminateAlternativeStatement", "TEMP_UntypedTerminateAlternativeStatement" );
     NEW_TERMINAL_MACRO (UntypedSelectiveAcceptStatement,      "UntypedSelectiveAcceptStatement",      "TEMP_UntypedSelectiveAcceptStatement" );
     NEW_TERMINAL_MACRO (UntypedTimedEntryCallStatement,       "UntypedTimedEntryCallStatement",       "TEMP_UntypedTimedEntryCallStatement" );
     NEW_TERMINAL_MACRO (UntypedConditionalEntryCallStatement, "UntypedConditionalEntryCallStatement", "TEMP_UntypedConditionalEntryCallStatement" );
     NEW_TERMINAL_MACRO (UntypedAsynchronousSelectStatement,   "UntypedAsynchronousSelectStatement",   "TEMP_UntypedAsynchronousSelectStatement" );
     NEW_TERMINAL_MACRO (UntypedAbortStatement,                "UntypedAbortStatement",                "TEMP_UntypedAbortStatement" );
     NEW_TERMINAL_MACRO (UntypedRaiseStatement,                "UntypedRaiseStatement",                "TEMP_UntypedRaiseStatement" );
     NEW_TERMINAL_MACRO (UntypedCodeStatement,                 "UntypedCodeStatement",                 "TEMP_UntypedCodeStatement" );
  // NEW_TERMINAL_MACRO (UntypedxStatement,                    "UntypedxStatement",                    "TEMP_UntypedxStatement" );

  // DQ (1/22/2016): Allow this IR node to be used explicitly in the AST.
  // NEW_TERMINAL_MACRO (UntypedFunctionDeclaration,      "UntypedFunctionDeclaration",      "TEMP_UntypedFunctionDeclaration" );
     NEW_NONTERMINAL_MACRO (UntypedFunctionDeclaration, UntypedProgramHeaderDeclaration | UntypedSubroutineDeclaration |
         UntypedInterfaceDeclaration, "UntypedFunctionDeclaration", "UntypedFunctionDeclarationTag", true);

  // DQ (3/6/2014): Added new IR node for untyped representation of module declarations.
     NEW_TERMINAL_MACRO (UntypedModuleDeclaration,        "UntypedModuleDeclaration",        "TEMP_UntypedModuleDeclaration" );

  // Rasmussen (8/16/2017): Added new IR node to represent a Fortran submodule (a submodule extends an existing module)
     NEW_TERMINAL_MACRO (UntypedSubmoduleDeclaration,     "UntypedSubmoduleDeclaration",     "TEMP_UntypedSubmoduleDeclaration" );

  // Rasmussen (8/17/2017): Added new IR node to represent a Fortran block-data program unit
     NEW_TERMINAL_MACRO (UntypedBlockDataDeclaration,     "UntypedBlockDataDeclaration",     "TEMP_UntypedBlockDataDeclaration" );

  // DQ (9/29/2017): Added new IR node for untyped representation of package declarations (Ada).
  // Rasmussen (02/22/2019): Added SgUntypedDirectiveDeclaration node for compiler directives
  // Rasmussen (03/04/2019): Added SgUntypedEnumDeclaration node
  // Rasmussen (09/30/2019): Added SgUntypedTypedefDeclaration
     NEW_TERMINAL_MACRO (UntypedPackageDeclaration,          "UntypedPackageDeclaration",          "TEMP_UntypedPackageDeclaration" );
     NEW_TERMINAL_MACRO (UntypedTaskDeclaration,             "UntypedTaskDeclaration",             "TEMP_UntypedTaskDeclaration" );
     NEW_TERMINAL_MACRO (UntypedStructureDeclaration,        "UntypedStructureDeclaration",        "TEMP_UntypedStructureDeclaration" );
     NEW_TERMINAL_MACRO (UntypedTypedefDeclaration,          "UntypedTypedefDeclaration",          "TEMP_UntypedTypedefDeclaration" );
     NEW_TERMINAL_MACRO (UntypedExceptionDeclaration,        "UntypedExceptionDeclaration",        "TEMP_UntypedExceptionDeclaration" );
     NEW_TERMINAL_MACRO (UntypedExceptionHandlerDeclaration, "UntypedExceptionHandlerDeclaration", "TEMP_UntypedExceptionHandlerDeclaration" );
     NEW_TERMINAL_MACRO (UntypedUnitDeclaration,             "UntypedUnitDeclaration",             "TEMP_UntypedUnitDeclaration" );
     NEW_TERMINAL_MACRO (UntypedDirectiveDeclaration,        "UntypedDirectiveDeclaration",        "TEMP_UntypedDirectiveDeclaration");
     NEW_TERMINAL_MACRO (UntypedEnumDeclaration,             "UntypedEnumDeclaration",             "TEMP_UntypedEnumDeclaration");

  // DQ (1/22/2016): Allow this IR node to be used explicitly in the AST.
  // NEW_NONTERMINAL_MACRO (UntypedDeclarationStatement, UntypedImplicitDeclaration | UntypedVariableDeclaration | 
  //     UntypedFunctionDeclaration | UntypedModuleDeclaration,
  //     "UntypedDeclarationStatement", "UntypedDeclarationStatementTag", false);
  // Rasmussen (8/16-17/2017): Added UntypedSubmoduleDeclaration, UntypedBlockDataDeclaration
  // Rasmussen (1/16/2018): Added UntypedNullDeclaration
  // Rasmussen (8/06/2018): Added SgUntypedInitializedNameListDeclaration
  // Rasmussen (02/22/2019): Added SgUntypedDirectiveDeclaration node for compiler directives
  // Rasmussen (09/30/2019): Added SgUntypedTypedefDeclaration for Jovial type declarations of primitive types
     NEW_NONTERMINAL_MACRO (UntypedDeclarationStatement,
         UntypedNullDeclaration      | UntypedNameListDeclaration  | UntypedUseStatement                |
         UntypedImplicitDeclaration  | UntypedVariableDeclaration  | UntypedFunctionDeclaration         |
         UntypedModuleDeclaration    | UntypedSubmoduleDeclaration | UntypedBlockDataDeclaration        |
         UntypedPackageDeclaration   | UntypedStructureDeclaration | UntypedExceptionHandlerDeclaration |
         UntypedExceptionDeclaration | UntypedTaskDeclaration      | UntypedUnitDeclaration             |
         UntypedDirectiveDeclaration | UntypedEnumDeclaration      | UntypedTypedefDeclaration          |
         UntypedInitializedNameListDeclaration,
         "UntypedDeclarationStatement", "UntypedDeclarationStatementTag", true);

     NEW_TERMINAL_MACRO (UntypedAssignmentStatement,   "UntypedAssignmentStatement",   "TEMP_UntypedAssignmentStatement" );
     NEW_TERMINAL_MACRO (UntypedBlockStatement,        "UntypedBlockStatement",        "TEMP_UntypedBlockStatement" );
     NEW_TERMINAL_MACRO (UntypedExpressionStatement,   "UntypedExpressionStatement",   "TEMP_UntypedExpressionStatement" );
     NEW_TERMINAL_MACRO (UntypedFunctionCallStatement, "UntypedFunctionCallStatement", "TEMP_UntypedFunctionCallStatement" );
     NEW_TERMINAL_MACRO (UntypedImageControlStatement, "UntypedImageControlStatement", "TEMP_UntypedImageControlStatement" );
     NEW_TERMINAL_MACRO (UntypedNamedStatement,        "UntypedNamedStatement",        "TEMP_UntypedNamedStatement" );
     NEW_TERMINAL_MACRO (UntypedOtherStatement,        "UntypedOtherStatement",        "TEMP_UntypedOtherStatement" );

  // DQ (3/6/2014): Added new IR node for untyped representation of scopes.
  // Rasmussen (05/22/2019): Added UntypedStructureDefinition to help make Jovial Table types easier to implement.
     NEW_TERMINAL_MACRO (UntypedFunctionScope, "UntypedFunctionScope",   "TEMP_UntypedFunctionScope" );
     NEW_TERMINAL_MACRO (UntypedModuleScope,   "UntypedModuleScope",     "TEMP_UntypedModuleScope" );
     NEW_TERMINAL_MACRO (UntypedGlobalScope,   "UntypedGlobalScope",     "TEMP_UntypedGlobalScope" );
     NEW_TERMINAL_MACRO (UntypedStructureDefinition, "UntypedStructureDefinition", "TEMP_UntypedStructureDefinition" );

  // DQ (10/15/2017): Allow the SgUntypedScope node to stand on it's own (need not be one of the derived classes in the AST).
  // Note that we might want to change this in the future, but for now it allows for greater flexability in constructing the untyped AST.
  // NEW_NONTERMINAL_MACRO (UntypedScope, UntypedFunctionScope | UntypedModuleScope | UntypedGlobalScope,
  //     "UntypedScope", "UntypedScopeTag", false);
     NEW_NONTERMINAL_MACRO (UntypedScope, UntypedFunctionScope | UntypedModuleScope | UntypedGlobalScope |
                            UntypedStructureDefinition,
                            "UntypedScope", "UntypedScopeTag", true);

     NEW_NONTERMINAL_MACRO (UntypedStatement, UntypedDeclarationStatement | UntypedAssignmentStatement           | UntypedBlockStatement                |
         UntypedExpressionStatement      | UntypedFunctionCallStatement   | UntypedImageControlStatement         | UntypedNamedStatement                |
         UntypedOtherStatement           | UntypedScope                   | UntypedNullStatement                 | UntypedIfStatement                   |
         UntypedCaseStatement            | UntypedLabelStatement          | UntypedLoopStatement                 | UntypedWhileStatement                |
         UntypedForStatement             | UntypedExitStatement           | UntypedGotoStatement                 | UntypedProcedureCallStatement        |
         UntypedReturnStatement          | UntypedExtendedReturnStatement | UntypedAcceptStatement               | UntypedEntryCallStatement            |
         UntypedRequeueStatement         | UntypedDelayUntilStatement     | UntypedDelayRelativeStatement        | UntypedTerminateAlternativeStatement |
         UntypedSelectiveAcceptStatement | UntypedTimedEntryCallStatement | UntypedConditionalEntryCallStatement | UntypedAsynchronousSelectStatement   |
         UntypedAbortStatement           | UntypedRaiseStatement          | UntypedStopStatement                 | UntypedCodeStatement                 |
         UntypedForAllStatement,
         "UntypedStatement", "UntypedStatementTag", false);

  // Rasmussen (06/13/2019): Added UntypedTableType for Jovial tables.
     NEW_TERMINAL_MACRO (UntypedArrayType, "UntypedArrayType", "TEMP_UntypedArrayType" );
     NEW_TERMINAL_MACRO (UntypedTableType, "UntypedTableType", "TEMP_UntypedTableType" );

     NEW_NONTERMINAL_MACRO (UntypedType, UntypedArrayType | UntypedTableType,
                            "UntypedType", "UntypedTypeTag", false);

     NEW_TERMINAL_MACRO (UntypedName,             "UntypedName",             "TEMP_UntypedName" );
     NEW_TERMINAL_MACRO (UntypedNameList,         "UntypedNameList",         "TEMP_UntypedNameList" );

     NEW_TERMINAL_MACRO (UntypedToken,            "UntypedToken",            "TEMP_UntypedToken" );
     NEW_TERMINAL_MACRO (UntypedTokenPair,        "UntypedTokenPair",        "TEMP_UntypedTokenPair" );
     NEW_TERMINAL_MACRO (UntypedTokenList,        "UntypedTokenList",        "TEMP_UntypedTokenList" );
     NEW_TERMINAL_MACRO (UntypedTokenPairList,    "UntypedTokenPairList",    "TEMP_UntypedTokenPairList" );

     NEW_TERMINAL_MACRO (UntypedAttribute,        "UntypedAttribute",        "TEMP_UntypedAttribute" );
     NEW_TERMINAL_MACRO (UntypedInitializedName,  "UntypedInitializedName",  "TEMP_UntypedInitializedName" );

  // DQ (3/6/2014): Added new IR node for untyped fortran file.
     NEW_TERMINAL_MACRO (UntypedFile,        "UntypedFile",        "TEMP_UntypedFile" );

  // We need separate IR nodes for these list so that (ROSETTA constraint).
     NEW_TERMINAL_MACRO (UntypedStatementList,           "UntypedStatementList",           "TEMP_UntypedStatementList" );

  // DQ (1/20/2016): Change the name to make this IR node more consistant with our naming policy for IR node.
  // NEW_TERMINAL_MACRO (UntypedDeclarationList,         "UntypedDeclarationList",         "TEMP_UntypedDeclarationList" );
     NEW_TERMINAL_MACRO (UntypedDeclarationStatementList, "UntypedDeclarationStatementList", "TEMP_UntypedDeclarationStatementList" );
     NEW_TERMINAL_MACRO (UntypedFunctionDeclarationList, "UntypedFunctionDeclarationList", "TEMP_UntypedFunctionDeclarationList" );
     NEW_TERMINAL_MACRO (UntypedInitializedNameList,     "UntypedInitializedNameList",     "TEMP_UntypedInitializedNameList" );

     NEW_NONTERMINAL_MACRO (UntypedNode, UntypedExpression | UntypedStatement | UntypedName | UntypedToken | UntypedTokenPair |
          UntypedType | UntypedAttribute | UntypedInitializedName | UntypedFile | UntypedStatementList |
          UntypedDeclarationStatementList | UntypedFunctionDeclarationList | UntypedInitializedNameList |
          UntypedNameList | UntypedTokenList | UntypedTokenPairList,
         "UntypedNode", "UntypedNodeTag", false);

  // ***************************************************************************************
  //                              END of Untyped IR Node Support
  // ***************************************************************************************

  // DQ(1/13/2014): Added Java support for JavaMemberValuePair
  // DQ (11/26/2013): Added UntypedNode to be derived from LocatedNodeSupport.
  // DQ (10/6/2008): Migrate some of the SgSupport derived IR nodes, that truly have a position in the 
  // source code, to SgLocatedNode.  Start with some of the newer IR nodes which are traversed and thus 
  // are forced to have an interface for the source position interface information (already present in 
  // the SgLocatedNode base class).  Eventually a number of the IR nodes currently derived from SgSupport
  // should be moved to be here (e.g. SgTemplateArgument, SgTemplateParameter, and 
  // a number of the new Fortran specific IRnodes, etc.).
  // NEW_NONTERMINAL_MACRO (LocatedNodeSupport, CommonBlockObject | InitializedName | InterfaceBody | HeaderFileBody | RenamePair | OmpClause , "LocatedNodeSupport", "LocatedNodeSupportTag", false );
  // NEW_NONTERMINAL_MACRO (LocatedNodeSupport, CommonBlockObject | InitializedName | InterfaceBody | HeaderFileBody | RenamePair | OmpClause | UntypedNode, "LocatedNodeSupport", "LocatedNodeSupportTag", false );
     NEW_NONTERMINAL_MACRO (LocatedNodeSupport, CommonBlockObject | InitializedName | InterfaceBody | 
                            HeaderFileBody | RenamePair | JavaMemberValuePair | OmpClause | UntypedNode | 
                            LambdaCapture | LambdaCaptureList, "LocatedNodeSupport", "LocatedNodeSupportTag", false );

  // DQ (3/24/2007): Added support for tokens in the IR (to support threading of the token stream 
  // onto the AST as part of an alternative, and exact, form of code generation within ROSE.
  // NEW_NONTERMINAL_MACRO (LocatedNode, Expression | Statement, "LocatedNode", "LocatedNodeTag" );

     NEW_TERMINAL_MACRO (Token, "Token", "TOKEN" );

  // Liao 11/2/2010, LocatedNodeSupport is promoted to the first location since SgInitializedName's internal type is used in some Statement  
  // NEW_NONTERMINAL_MACRO (LocatedNode, LocatedNodeSupport| Statement | Expression | Token, "LocatedNode", "LocatedNodeTag", false );
     NEW_NONTERMINAL_MACRO (LocatedNode, Token | LocatedNodeSupport| Statement | Expression, "LocatedNode", "LocatedNodeTag", false );

     AstNodeClass & Type    = *lookupTerminal(terminalList, "Type");
     AstNodeClass & Symbol  = *lookupTerminal(terminalList, "Symbol");
     AstNodeClass & Support = *lookupTerminal(terminalList, "Support");

  // DQ (3/14/2007): Added IR support for binaries
     AstNodeClass & AsmNode = *lookupTerminal(terminalList, "AsmNode");

  // printf ("nonTerminalList.size() = %" PRIuPTR " \n",nonTerminalList.size());

  // DQ (4/20/2014): Adding more support for ATerm library.
  // Rasmussen (04/17/2019): Support for ATerms has been deprecated.
  // NEW_TERMINAL_MACRO (Aterm, "Aterm", "ATERM" );

  // DQ (3/14/2007): Added IR support for binaries
  // NEW_NONTERMINAL_MACRO (Node, Type | Symbol | LocatedNode | Support, "Node", "NodeTag" );
  // NEW_NONTERMINAL_MACRO (Node, Support | Type | LocatedNode | Symbol | AsmNode, "Node", "NodeTag", false );
  // NEW_NONTERMINAL_MACRO (Node, Type | Symbol | LocatedNode | Support, "Node", "NodeTag" );
  // NEW_NONTERMINAL_MACRO (Node, Support | Type | LocatedNode | Symbol | AsmNode | Aterm, "Node", "NodeTag", false );
     NEW_NONTERMINAL_MACRO (Node, Support | Type | LocatedNode | Symbol | AsmNode, "Node", "NodeTag", false );

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

  // DQ (12/3/2014): We need a concept of contains modified code so that we can support the unparsing from the token stream.
     Node.setDataPrototype("bool","containsTransformation","= false",
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
     //            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);
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

  // DQ (5/28/2011): Added central location for qualified name maps (for names and types).
  // these maps store the required qualified name for where an IR node is referenced (not
  // at the IR node which has the qlocal qualifier).  Thus we can support multiple references 
  // to an IR node which might have different qualified names.  This is critical to the 
  // qualified name support.
     Node.setDataPrototype("static std::map<SgNode*,std::string>","globalQualifiedNameMapForNames","",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     Node.setDataPrototype("static std::map<SgNode*,std::string>","globalQualifiedNameMapForTypes","",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

  // DQ (9/7/2014): Added support for template headers as part of name qualification.
     Node.setDataPrototype("static std::map<SgNode*,std::string>","globalQualifiedNameMapForTemplateHeaders","",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

  // DQ (6/3/2011): Names of types that can have embedded qualified names have names that are dependent 
  // upon the location where they are referenced.  This map stored the generated names of such types
  // which are then used in the unparsing.  This is relevant only for C++ and is a part of the name 
  // qualification support in the unparser.
     Node.setDataPrototype("static std::map<SgNode*,std::string>","globalTypeNameMap","",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

  // DQ (3/13/2019): The fix for referencing types than contain many parts is to have a map of maps 
  // to the generated name qualification substrings for each type, all associted with a single reference 
  // node to the statement refering to the type.
     Node.setDataPrototype("static std::map<SgNode*,std::map<SgNode*,std::string> >","globalQualifiedNameMapForMapsOfTypes","",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

#if 0
  // DQ (6/21/2011): Since this type "std::set<SgNode*>" is not supported by our AST file I/O I will 
  // implement this in a way that does not require such support.
  // DQ (6/21/2011): Added support for global handling of list of seen declarations to be used to 
  // support the name qualification.  Name qualification is used at different locations defferently
  // depending upon if the declaration has been seen and what  sort of scope it was declared in and
  // if it was a defining declaration, etc.
  // Node.setDataPrototype("static std::set<SgNode*>","globalReferencedNameSet","",
  //        NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     Node.setDataPrototype("static SgNodeSet","globalReferencedNameSet","",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
#endif

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

  // DQ (11/1/2015): Build the access functions, but don't let the set_* access function set the "p_isModified" flag.
  // LocatedNode.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
  //              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, CLONE_PTR);
     LocatedNode.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);


     LocatedNode.setFunctionPrototype      ( "HEADER_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
     LocatedNode.setFunctionSource         ( "SOURCE_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
#endif

  // DQ (11/3/2015): Added support to use mechanism that to have set_* access functions not mark node using isModified flag.
  // DQ (1/15/2015): We need a concept of transformation restricted to the addition or deletion or transformation of the surounding comments and CPP directives.
  // LocatedNode.setDataPrototype("bool","containsTransformationToSurroundingWhitespace","= false",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     LocatedNode.setDataPrototype("bool","containsTransformationToSurroundingWhitespace","= false",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

  // DQ (3/24/2007): Added support for tokens in the IR.
  // Token.setPredeclarationString ("HEADER_TOKEN_PREDECLARATION" , "../Grammar/LocatedNode.code");
     Token.setFunctionPrototype    ("HEADER_TOKEN", "../Grammar/LocatedNode.code");

  // DQ (3/24/2007): Should be be naming the string we hold in the token "lexeme", or is there a better name?
     Token.setDataPrototype ( "std::string", "lexeme_string", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     Token.setDataPrototype ( "unsigned int", "classification_code", "= 0",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

     LocatedNodeSupport.setFunctionPrototype ( "HEADER_LOCATED_NODE_SUPPORT", "../Grammar/LocatedNode.code");


  // DQ (9/3/2014): Adding support for C++11 lambda expresions.
     LambdaCapture.setFunctionPrototype     ( "HEADER_LAMBDA_CAPTURE", "../Grammar/LocatedNode.code");
     LambdaCaptureList.setFunctionPrototype ( "HEADER_LAMBDA_CAPTURE_LIST", "../Grammar/LocatedNode.code");


  // ***************************************************************************************
  // ***************************************************************************************
  //                                 ATerm IR Node Support
  // ***************************************************************************************
  // ***************************************************************************************
  // DQ (4/20/2014): Added support for ATerms in the IR.  The goal is to support a new level
  // of reading ATerms (previously demonstrated in projects/AtermTranslation directory).
  // This level of support reads the Aterms and represents the Aterms in a ROSE AST using
  // specific SgAterm IR nodes that as fundamentally simpler then the more commonly use
  // ROSE IR nodes.  This work is in contrast to the Aterm API for the ROSE AST which has
  // become problematic to support beyond a specific level.  Current level of support for
  // the ATerm API in ROSE is limited to the demonstration using ATerm specific tools that
  // generate DOT graph files from any Aterm and can be make to work on the ROSE AST as 
  // well though the use of the ATerm API in ROSE (all this is demonstrated in the examples
  // in the projects/AtermTranslation directory).

  // Rasmussen (04/17/2019): Support for ATerms has been deprecated.
  // Aterm.setFunctionPrototype ( "HEADER_ATERM_NODE", "../Grammar/LocatedNode.code");
  // Aterm.setDataPrototype     ( "std::string", "name", "= \"\"",
  //              CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // ***************************************************************************************
  // ***************************************************************************************
  //                                 Untyped IR Node Support
  // ***************************************************************************************
  // ***************************************************************************************
  // DQ (11/26/2013): Adding support for untyped AST IR nodes to support translation of ATterm 
  // based untyped ASTs into ROSE so that we will have tools (inherited attribute and synthizied 
  // attribute traversals) from which to build the ROSE AST (typed AST) and define a proper frontend.

     UntypedNode.setFunctionPrototype       ( "HEADER_UNTYPED_NODE", "../Grammar/LocatedNode.code");

     UntypedExpression.setFunctionPrototype ( "HEADER_UNTYPED_EXPRESSION", "../Grammar/LocatedNode.code");
  // DQ (10/3/2017): Generalizing untyped IR nodes to be less language specific (for use with different languages).
  // UntypedExpression.setDataPrototype     ( "SgToken::ROSE_Fortran_Keywords", "statement_enum", "= SgToken::FORTRAN_UNKNOWN",
  //              CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedExpression.setDataPrototype     ( "int", "expression_enum", "= 0",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Rasmussen (10/17/2017): Removed SgToken::ROSE_Fortran_Operators operator_enum from UntypedUnaryOperator
     UntypedUnaryOperator.setFunctionPrototype ( "HEADER_UNTYPED_UNARY_OPERATOR", "../Grammar/LocatedNode.code");
     UntypedUnaryOperator.setDataPrototype     ( "std::string", "operator_name", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedUnaryOperator.setDataPrototype     ( "SgUntypedExpression*", "operand", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // Rasmussen (10/17/2017): Removed SgToken::ROSE_Fortran_Operators operator_enum from UntypedBinaryOperator
     UntypedBinaryOperator.setFunctionPrototype ( "HEADER_UNTYPED_BINARY_OPERATOR", "../Grammar/LocatedNode.code");
     UntypedBinaryOperator.setDataPrototype     ( "std::string", "operator_name", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedBinaryOperator.setDataPrototype     ( "SgUntypedExpression*", "lhs_operand", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedBinaryOperator.setDataPrototype     ( "SgUntypedExpression*", "rhs_operand", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // Rasmussen (12/20/2017): Added SgUntypedExprListExpression and SgUntypedNullExpression
     UntypedExprListExpression.setFunctionPrototype ( "HEADER_UNTYPED_EXPR_LIST_EXPRESSION", "../Grammar/LocatedNode.code");
     UntypedExprListExpression.setDataPrototype     ("SgUntypedExpressionPtrList", "expressions", "",
                                                     NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedNullExpression.setFunctionPrototype     ( "HEADER_UNTYPED_NULL_EXPRESSION", "../Grammar/LocatedNode.code");

  // Rasmussen (09/04/2018): Added SgUntypedNamedExpression
     UntypedNamedExpression.setFunctionPrototype    ( "HEADER_UNTYPED_NAMED_EXPRESSION", "../Grammar/LocatedNode.code");
     UntypedNamedExpression.setDataPrototype        ( "std::string", "expression_name", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedNamedExpression.setDataPrototype        ( "SgUntypedExpression*", "expression", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // Rasmussen (01/22/2018): Added SgUntypedSubscriptExpression
     UntypedSubscriptExpression.setDataPrototype    ( "SgUntypedExpression*", "lower_bound", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedSubscriptExpression.setDataPrototype    ( "SgUntypedExpression*", "upper_bound", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedSubscriptExpression.setDataPrototype    ( "SgUntypedExpression*", "stride",      "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     UntypedValueExpression.setFunctionPrototype ( "HEADER_UNTYPED_VALUE_EXPRESSION", "../Grammar/LocatedNode.code");
  // Save this as a string so that we can reproduce the exact value for floating point numbers.
     UntypedValueExpression.setDataPrototype     ( "std::string", "value_string", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedValueExpression.setDataPrototype     ( "SgUntypedType*", "type", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (3/6/2014): TODO: This array reference will require concepts of indexing, triplet notation, index sets, etc.
  // Rasmussen (11/07/2018): Added variable reference name and array and coarray subscripts
     UntypedArrayReferenceExpression.setFunctionPrototype ( "HEADER_UNTYPED_ARRAY_REFERENCE_EXPRESSION", "../Grammar/LocatedNode.code");
     UntypedArrayReferenceExpression.setDataPrototype     ( "std::string", "name", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL, NO_DELETE);
     UntypedArrayReferenceExpression.setDataPrototype     ( "SgUntypedExpression*", "array_subscripts", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedArrayReferenceExpression.setDataPrototype     ( "SgUntypedExpression*", "coarray_subscripts", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     UntypedOtherExpression.setFunctionPrototype          ( "HEADER_UNTYPED_OTHER_EXPRESSION", "../Grammar/LocatedNode.code");
     UntypedFunctionCallOrArrayReferenceExpression.setFunctionPrototype ( "HEADER_UNTYPED_FUNCTION_CALL_OR_ARRAY_REFERENCE_EXPRESSION", "../Grammar/LocatedNode.code");
     UntypedReferenceExpression.setFunctionPrototype ( "HEADER_UNTYPED_REFERENCE_EXPRESSION", "../Grammar/LocatedNode.code");
     UntypedReferenceExpression.setDataPrototype     ( "std::string", "name", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // UntypedReferenceExpression.setDataPrototype     ( "SgUntypedType*", "type", "= NULL",
  //              CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (3/3/2014): The type would contain the concept of it being constant.
  // UntypedReferenceExpression.setDataPrototype     ( "bool", "is_constant", "= false",
  //              CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     UntypedStatement.setFunctionPrototype             ( "HEADER_UNTYPED_STATEMENT", "../Grammar/LocatedNode.code");
  // Save this as a string so that we catch details such as "0025" instead of just 25 as an integer.
     UntypedStatement.setDataPrototype     ( "std::string", "label_string", "= \"\"",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (10/3/2017): Generalizing untyped IR nodes to be less language specific (for use with different languages).
  // UntypedStatement.setDataPrototype     ( "SgToken::ROSE_Fortran_Keywords", "statement_enum", "= SgToken::FORTRAN_UNKNOWN",
  //              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // UntypedStatement.setDataPrototype     ( "int", "statement_enum", "= SgToken::FORTRAN_UNKNOWN",
  //              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Rasmussen (10/3/2017): Added statement_enum (replacing it's removal from base class).
     UntypedNamedStatement.setFunctionPrototype      ( "HEADER_UNTYPED_NAMED_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedNamedStatement.setDataPrototype          ( "int", "statement_enum", "= 0",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedNamedStatement.setDataPrototype          ( "std::string", "statement_name", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Rasmussen (10/3/2017): Added statement_enum (replacing it's removal from base class).
     UntypedExpressionStatement.setFunctionPrototype ( "HEADER_UNTYPED_EXPRESSION_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedExpressionStatement.setDataPrototype     ( "int", "statement_enum", "= 0",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedExpressionStatement.setDataPrototype     ( "SgUntypedExpression*", "statement_expression", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // Rasmussen (4/19/2018): Added target_label so that the goto statement has a place to go to.
     UntypedGotoStatement.setDataPrototype           ( "std::string", "target_label", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     UntypedAssignmentStatement.setFunctionPrototype ( "HEADER_UNTYPED_ASSIGNMENT_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedAssignmentStatement.setDataPrototype     ( "SgUntypedExpression*", "lhs_operand", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedAssignmentStatement.setDataPrototype     ( "SgUntypedExpression*", "rhs_operand", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // Rasmussen (9/10/2018): Added additional parameters: statement_enum, function, args, and abort_name.
     UntypedFunctionCallStatement.setFunctionPrototype ( "HEADER_UNTYPED_FUNCTION_CALL_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedFunctionCallStatement.setDataPrototype     ( "int", "statement_enum", "= 0",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedFunctionCallStatement.setDataPrototype     ( "SgUntypedExpression*", "function", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedFunctionCallStatement.setDataPrototype     ( "SgUntypedExprListExpression*", "args", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedFunctionCallStatement.setDataPrototype     ( "std::string", "abort_name", "= \"\"",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Rasmussen (9/22/2018): This node contains information for Fortran image control statements, i.e., sync-all-stmt, form-team-stmt
     UntypedImageControlStatement.setFunctionPrototype ( "HEADER_UNTYPED_IMAGE_CONTROL_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedImageControlStatement.setDataPrototype     ( "int", "statement_enum", "= 0",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedImageControlStatement.setDataPrototype     ( "SgUntypedExpression*", "variable", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedImageControlStatement.setDataPrototype     ( "SgUntypedExpression*", "expression", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedImageControlStatement.setDataPrototype     ( "SgUntypedExprListExpression*", "status_list", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     UntypedBlockStatement.setFunctionPrototype        ( "HEADER_UNTYPED_BLOCK_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedBlockStatement.setDataPrototype            ( "SgUntypedScope*", "scope", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // Rasmussen (10/3/2017): Added statement_enum (replacing it's removal from base class).
     UntypedOtherStatement.setFunctionPrototype        ( "HEADER_UNTYPED_OTHER_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedOtherStatement.setDataPrototype            ( "int", "statement_enum", "= 0",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Rasmussen (6/ 5/2018): Modified untyped return statement to contain an expression
     UntypedReturnStatement.setFunctionPrototype       ( "HEADER_UNTYPED_RETURN_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedReturnStatement.setDataPrototype           ( "SgUntypedExpression*", "expression", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // Rasmussen (6/ 5/2018): Added untyped stop statement with an expression return
     UntypedStopStatement.setFunctionPrototype         ( "HEADER_UNTYPED_STOP_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedStopStatement.setDataPrototype             ( "SgUntypedExpression*", "expression", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     UntypedUseStatement.setFunctionPrototype          ( "HEADER_UNTYPED_USE_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedUseStatement.setDataPrototype              ( "std::string", "module_name", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedUseStatement.setDataPrototype              ( "SgToken::ROSE_Fortran_Keywords", "module_nature", "= SgToken::FORTRAN_UNKNOWN",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedUseStatement.setDataPrototype              ( "SgUntypedTokenPairList*", "rename_list", "",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedUseStatement.setDataPrototype              ( "bool", "isOnlyList", "= false",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (10/3/2017): New statements specific to general language support.  The philosophy is to add the union of all statements
  // for all languages to the set of untyped IR nodes and provide an enum code to support the larger number of expressions.
  // Rasmussen (1/16/2018): Added UntypedNullDeclaration
     UntypedNullDeclaration.setFunctionPrototype       ( "HEADER_UNTYPED_NULL_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedNullStatement.setFunctionPrototype         ( "HEADER_UNTYPED_NULL_STATEMENT", "../Grammar/LocatedNode.code");

  // Rasmussen (7/19/2018): Added statement_enum to specify the type of if statement, i.e., Fortran if-stmt or if-construct
  // Rasmussen (7/06/2018): Added conditional expression and changed true and false bodies to SgUntypedStatement (from
  // SgUntypedScope).  Also made all parameters CONSTRUCTOR_PARAMETER (always the case for untyped node constructors?)
     UntypedIfStatement.setFunctionPrototype           ( "HEADER_UNTYPED_IF_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedIfStatement.setDataPrototype               ( "int", "statement_enum", "= 0",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedIfStatement.setDataPrototype               ( "SgUntypedExpression*",  "conditional", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedIfStatement.setDataPrototype               ( "SgUntypedStatement*", "true_body", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedIfStatement.setDataPrototype               ( "SgUntypedStatement*", "false_body", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // Rasmussen (8/14/2018): Added statement_enum, expression, body, ..., to allow connection to Sage nodes.
     UntypedCaseStatement.setFunctionPrototype         ( "HEADER_UNTYPED_CASE_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedCaseStatement.setDataPrototype             ( "int", "statement_enum", "= 0",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedCaseStatement.setDataPrototype             ( "SgUntypedExpression*",  "expression", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedCaseStatement.setDataPrototype             ( "SgUntypedStatement*", "body", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedCaseStatement.setDataPrototype             ( "std::string", "case_construct_name", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedCaseStatement.setDataPrototype             ( "bool", "has_fall_through", "= false",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL, NO_DELETE);

  // Rasmussen (08/27/2018): Added initialization, increment, ..., to allow connection to Sage nodes.
  // Rasmussen (10/22/2018): Added statement_enum to specify the type of for statement, i.e., Jovial by or then usage
     UntypedForStatement.setFunctionPrototype          ( "HEADER_UNTYPED_FOR_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedForStatement.setDataPrototype              ( "int", "statement_enum", "= 0",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedForStatement.setDataPrototype              ( "SgUntypedExpression*", "initialization", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedForStatement.setDataPrototype              ( "SgUntypedExpression*",  "bound", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedForStatement.setDataPrototype              ( "SgUntypedExpression*",  "increment", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedForStatement.setDataPrototype              ( "SgUntypedStatement*", "body", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedForStatement.setDataPrototype              ( "std::string", "do_construct_name", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Rasmussen (10/02/2018): Added to support Fortran DO CONCURRENT construct
  // Rasmussen (10/28/2018): Added statement_enum to generalize to include Fortran FORALL construct
     UntypedForAllStatement.setFunctionPrototype ( "HEADER_UNTYPED_FORALL_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedForAllStatement.setDataPrototype     ( "int", "statement_enum", "= 0",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedForAllStatement.setDataPrototype     ( "SgUntypedType*", "type", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedForAllStatement.setDataPrototype     ( "SgUntypedExprListExpression*", "iterates", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedForAllStatement.setDataPrototype     ( "SgUntypedExprListExpression*", "local", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedForAllStatement.setDataPrototype     ( "SgUntypedExpression*", "mask", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedForAllStatement.setDataPrototype     ( "std::string", "do_construct_name", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Rasmussen (10/10/2018): Added condition, and body to allow connection to Sage nodes.
     UntypedWhileStatement.setFunctionPrototype       ( "HEADER_UNTYPED_WHILE_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedWhileStatement.setDataPrototype           ( "SgUntypedExpression*",  "condition", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedWhileStatement.setDataPrototype           ( "SgUntypedStatement*", "body", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     UntypedLoopStatement.setFunctionPrototype                 ( "HEADER_UNTYPED_LOOP_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedExitStatement.setFunctionPrototype                 ( "HEADER_UNTYPED_EXIT_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedGotoStatement.setFunctionPrototype                 ( "HEADER_UNTYPED_GOTO_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedProcedureCallStatement.setFunctionPrototype        ( "HEADER_UNTYPED_PROCEDURE_CALL_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedExtendedReturnStatement.setFunctionPrototype       ( "HEADER_UNTYPED_EXTENDED_RETURN_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedAcceptStatement.setFunctionPrototype               ( "HEADER_UNTYPED_ACCEPT_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedEntryCallStatement.setFunctionPrototype            ( "HEADER_UNTYPED_ENTRY_CALL_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedRequeueStatement.setFunctionPrototype              ( "HEADER_UNTYPED_REQUEUE_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedDelayUntilStatement.setFunctionPrototype           ( "HEADER_UNTYPED_DELAY_UNTIL_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedDelayRelativeStatement.setFunctionPrototype        ( "HEADER_UNTYPED_DELAY_RELATIVE_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedTerminateAlternativeStatement.setFunctionPrototype ( "HEADER_UNTYPED_TERMINATE_ALTERNATIVE_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedSelectiveAcceptStatement.setFunctionPrototype      ( "HEADER_UNTYPED_SELECTIVE_ACCEPT_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedTimedEntryCallStatement.setFunctionPrototype       ( "HEADER_UNTYPED_TIMED_ENTRY_CALL_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedConditionalEntryCallStatement.setFunctionPrototype ( "HEADER_UNTYPED_CONDITIONAL_ENTRY_CALL_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedAsynchronousSelectStatement.setFunctionPrototype   ( "HEADER_UNTYPED_ASYNCHRONOUS_SELECT_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedAbortStatement.setFunctionPrototype                ( "HEADER_UNTYPED_ABORT_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedRaiseStatement.setFunctionPrototype                ( "HEADER_UNTYPED_RAISE_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedCodeStatement.setFunctionPrototype                 ( "HEADER_UNTYPED_CODE_STATEMENT", "../Grammar/LocatedNode.code");

     UntypedDeclarationStatement.setFunctionPrototype  ( "HEADER_UNTYPED_DECLARATION_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedImplicitDeclaration.setFunctionPrototype   ( "HEADER_UNTYPED_IMPLICIT_DECLARATION", "../Grammar/LocatedNode.code");

  // Rasmussen (5/28/2018): Added UntypedLabelStatement
     UntypedLabelStatement.setFunctionPrototype ( "HEADER_UNTYPED_LABEL_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedLabelStatement.setDataPrototype     ( "SgUntypedStatement*", "statement", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // Rasmussen (10/3/2017): Added statement_enum (replacing it's removal from base class).
     UntypedNameListDeclaration.setFunctionPrototype ( "HEADER_UNTYPED_NAMELIST_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedNameListDeclaration.setDataPrototype     ( "int", "statement_enum", "= 0",
                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedNameListDeclaration.setDataPrototype     ( "SgUntypedNameList*", "names", "",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // Rasmussen (8/06/2018): Added SgUntypedInitializedNameListDeclaration
     UntypedInitializedNameListDeclaration.setFunctionPrototype ( "HEADER_UNTYPED_INITIALIZED_NAMELIST_DECLARATION",
                                                                    "../Grammar/LocatedNode.code" );
     UntypedInitializedNameListDeclaration.setDataPrototype     ( "int", "statement_enum", "= 0",
                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedInitializedNameListDeclaration.setDataPrototype     ( "SgUntypedInitializedNameList*", "variables", "",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // Rasmussen (02/22/2019): Added SgUntypedDirectiveDeclaration node for compiler directives
     UntypedDirectiveDeclaration.setFunctionPrototype( "HEADER_UNTYPED_DIRECTIVE_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedDirectiveDeclaration.setDataPrototype    ( "int", "statement_enum", "= 0",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedDirectiveDeclaration.setDataPrototype    ( "std::string", "directive_string", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Rasmussen (03/04/2019): Added SgUntypedEnumDeclaration node
     UntypedEnumDeclaration.setFunctionPrototype ( "HEADER_UNTYPED_ENUM_DECLARATION", "../Grammar/LocatedNode.code" );
     UntypedEnumDeclaration.setDataPrototype     ( "std::string", "enum_name", "= \"\"",
                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL, NO_DELETE);
     UntypedEnumDeclaration.setDataPrototype     ( "SgUntypedInitializedNameList*", "enumerators", "",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     UntypedVariableDeclaration.setFunctionPrototype ( "HEADER_UNTYPED_VARIABLE_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedVariableDeclaration.setDataPrototype     ( "SgUntypedType*", "type", "= NULL",
                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL, NO_DELETE);
     UntypedVariableDeclaration.setDataPrototype     ( "SgUntypedExprListExpression*", "modifiers", "",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL, NO_DELETE);
     UntypedVariableDeclaration.setDataPrototype     ( "SgUntypedInitializedNameList*", "variables", "",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     UntypedFunctionDeclaration.setFunctionPrototype ( "HEADER_UNTYPED_FUNCTION_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedFunctionDeclaration.setDataPrototype     ( "std::string", "name", "= \"\"",
                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL, NO_DELETE);
     UntypedFunctionDeclaration.setDataPrototype     ( "SgUntypedInitializedNameList*", "parameters", "",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedFunctionDeclaration.setDataPrototype     ( "SgUntypedType*", "type", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL, NO_DELETE);
     UntypedFunctionDeclaration.setDataPrototype     ( "SgUntypedFunctionScope*", "scope", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedFunctionDeclaration.setDataPrototype     ( "SgUntypedExprListExpression*", "modifiers", "",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedFunctionDeclaration.setDataPrototype     ( "SgUntypedNamedStatement*", "end_statement", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // These are derived from UntypedFunctionDeclaration
     UntypedProgramHeaderDeclaration.setFunctionPrototype ( "HEADER_UNTYPED_PROGRAM_HEADER_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedSubroutineDeclaration.setFunctionPrototype    ( "HEADER_UNTYPED_SUBROUTINE_DECLARATION", "../Grammar/LocatedNode.code");

  // Rasmussen (10/3/2017): Added statement_enum (replacing it's removal from base class).
     UntypedInterfaceDeclaration.setFunctionPrototype     ( "HEADER_UNTYPED_INTERFACE_DECLARATION",  "../Grammar/LocatedNode.code");
     UntypedInterfaceDeclaration.setDataPrototype         ( "int", "statement_enum", "= 0",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     UntypedModuleDeclaration.setFunctionPrototype      ( "HEADER_UNTYPED_MODULE_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedModuleDeclaration.setDataPrototype          ( "std::string", "name", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL, NO_DELETE);
     UntypedModuleDeclaration.setDataPrototype          ( "SgUntypedModuleScope*", "scope", "= NULL",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedModuleDeclaration.setDataPrototype          ( "SgUntypedNamedStatement*", "end_statement", "= NULL",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (9/29/2017): Adding package support.
     UntypedPackageDeclaration.setFunctionPrototype      ( "HEADER_UNTYPED_PACKAGE_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedPackageDeclaration.setDataPrototype          ( "std::string", "name", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL, NO_DELETE);
     UntypedPackageDeclaration.setDataPrototype          ( "SgUntypedScope*", "scope", "= NULL",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (9/30/2017): Adding general language neutral structure support (e.g. class, struct, union, etc.).
  // Rasmussen (05/19/2019): Added dim_info and rank (to delete or not to delete that is question)

     UntypedStructureDefinition.setFunctionPrototype     ( "HEADER_UNTYPED_STRUCTURE_DEFINITION", "../Grammar/LocatedNode.code");
     UntypedStructureDefinition.setDataPrototype         ( "std::string", "type_name", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL, NO_DELETE);
     UntypedStructureDefinition.setDataPrototype         ( "bool", "has_type_name", "= false",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL, NO_DELETE);
     UntypedStructureDefinition.setDataPrototype         ( "bool", "has_body", "= false",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL, NO_DELETE);
     UntypedStructureDefinition.setDataPrototype         ( "SgUntypedExprListExpression*", "initializer", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL, NO_DELETE);
     UntypedStructureDefinition.setDataPrototype         ( "SgUntypedExprListExpression*", "modifiers", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL, NO_DELETE);
     UntypedStructureDefinition.setDataPrototype         ( "SgUntypedType*", "base_type", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedStructureDefinition.setDataPrototype         ( "SgUntypedScope*", "scope", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     UntypedStructureDeclaration.setFunctionPrototype    ( "HEADER_UNTYPED_STRUCTURE_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedStructureDeclaration.setDataPrototype        ( "std::string", "name", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL, NO_DELETE);
     UntypedStructureDeclaration.setDataPrototype        ( "SgUntypedExprListExpression*", "modifiers", "= NULL",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL, NO_DELETE);
     UntypedStructureDeclaration.setDataPrototype        ( "SgUntypedExprListExpression*", "dim_info", "= NULL",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL, DEF_DELETE);
     UntypedStructureDeclaration.setDataPrototype        ( "SgUntypedStructureDefinition*", "definition", "= NULL",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // Rasmussen (09/30/2019): Added SgUntypedTypedefDeclaration node
     UntypedTypedefDeclaration.setFunctionPrototype ( "HEADER_UNTYPED_TYPEDEF_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedTypedefDeclaration.setDataPrototype     ( "std::string", "name", "= \"\"",
                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL, NO_DELETE);
     UntypedTypedefDeclaration.setDataPrototype     ( "SgUntypedType*", "base_type", "= NULL",
                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL, NO_DELETE);

  // DQ (9/29/2017): Adding task support.
     UntypedTaskDeclaration.setFunctionPrototype      ( "HEADER_UNTYPED_TASK_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedTaskDeclaration.setDataPrototype          ( "std::string", "name", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL, NO_DELETE);
     UntypedTaskDeclaration.setDataPrototype          ( "SgUntypedScope*", "scope", "= NULL",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (10/15/2017): Adding unit support.
     UntypedUnitDeclaration.setFunctionPrototype      ( "HEADER_UNTYPED_UNIT_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedUnitDeclaration.setDataPrototype          ( "std::string", "name", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL, NO_DELETE);
     UntypedUnitDeclaration.setDataPrototype          ( "SgUntypedScope*", "scope", "= NULL",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // Rasmussen (8/16/2017): Added UntypedSubmoduleDeclaration (note submodule_ancestor and submodule_parent)
     UntypedSubmoduleDeclaration.setFunctionPrototype   ( "HEADER_UNTYPED_SUBMODULE_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedSubmoduleDeclaration.setDataPrototype       ( "std::string", "name", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL, NO_DELETE);
     UntypedSubmoduleDeclaration.setDataPrototype       ( "std::string", "submodule_ancestor", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL, NO_DELETE);
     UntypedSubmoduleDeclaration.setDataPrototype       ( "std::string", "submodule_parent", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL, NO_DELETE);
     UntypedSubmoduleDeclaration.setDataPrototype       ( "SgUntypedModuleScope*", "scope", "= NULL",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedSubmoduleDeclaration.setDataPrototype       ( "SgUntypedNamedStatement*", "end_statement", "= NULL",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // Rasmussen (8/17/2017): Added UntypedBlockDataDeclaration
     UntypedBlockDataDeclaration.setFunctionPrototype   ( "HEADER_UNTYPED_BLOCK_DATA_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedBlockDataDeclaration.setDataPrototype       ( "std::string", "name", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL, NO_DELETE);
     UntypedBlockDataDeclaration.setDataPrototype       ( "bool", "has_name", "= false",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL, NO_DELETE);
     UntypedBlockDataDeclaration.setDataPrototype       ( "SgUntypedDeclarationStatementList*", "declaration_list", "= NULL",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedBlockDataDeclaration.setDataPrototype       ( "SgUntypedNamedStatement*", "end_statement", "= NULL",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (10/2/2017): Adding general language neutral exception declaration support (e.g. for Ada).
     UntypedExceptionDeclaration.setFunctionPrototype      ( "HEADER_UNTYPED_EXCEPTION_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedExceptionDeclaration.setDataPrototype          ( "SgUntypedStatement*", "statement", "= NULL",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (10/2/2017): Adding general language neutral exception handler declaration support (e.g. for Ada).
     UntypedExceptionHandlerDeclaration.setFunctionPrototype      ( "HEADER_UNTYPED_EXCEPTION_HANDLER_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedExceptionHandlerDeclaration.setDataPrototype          ( "SgUntypedStatement*", "statement", "= NULL",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     UntypedScope.setFunctionPrototype         ( "HEADER_UNTYPED_SCOPE", "../Grammar/LocatedNode.code");
  // Three sorts of list that can be in any scope.
     UntypedScope.setDataPrototype             ( "SgUntypedDeclarationStatementList*", "declaration_list", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedScope.setDataPrototype             ( "SgUntypedStatementList*", "statement_list", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedScope.setDataPrototype             ( "SgUntypedFunctionDeclarationList*", "function_list", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     UntypedFunctionScope.setFunctionPrototype ( "HEADER_UNTYPED_FUNCTION_SCOPE", "../Grammar/LocatedNode.code");
     UntypedModuleScope.setFunctionPrototype   ( "HEADER_UNTYPED_MODULE_SCOPE", "../Grammar/LocatedNode.code");
     UntypedGlobalScope.setFunctionPrototype   ( "HEADER_UNTYPED_GLOBAL_SCOPE", "../Grammar/LocatedNode.code");

     UntypedInitializedName.setFunctionPrototype ( "HEADER_UNTYPED_INITIALIZED_NAME", "../Grammar/LocatedNode.code");
     UntypedInitializedName.setDataPrototype     ( "SgUntypedType*", "type", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedInitializedName.setDataPrototype     ( "std::string", "name", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedInitializedName.setDataPrototype     ( "bool", "has_initializer", "= false",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedInitializedName.setDataPrototype     ( "SgUntypedExpression*", "initializer", "= NULL",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     UntypedAttribute.setFunctionPrototype ( "HEADER_UNTYPED_ATTRIBUTE", "../Grammar/LocatedNode.code");
     UntypedAttribute.setDataPrototype     ( "SgToken::ROSE_Fortran_Keywords", "type_name", "= SgToken::FORTRAN_UNKNOWN",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedAttribute.setDataPrototype     ( "std::string", "named_attribute", "= \"\"",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     UntypedFile.setFunctionPrototype      ( "HEADER_UNTYPED_FILE", "../Grammar/LocatedNode.code");
     UntypedFile.setDataPrototype          ( "SgUntypedGlobalScope*", "scope", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     UntypedStatementList.setFunctionPrototype           ( "HEADER_UNTYPED_STATEMENT_LIST", "../Grammar/LocatedNode.code");
     UntypedStatementList.setDataPrototype               ( "SgUntypedStatementPtrList", "stmt_list", "",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedDeclarationStatementList.setFunctionPrototype         ( "HEADER_UNTYPED_DECLARATION_STATEMENT_LIST", "../Grammar/LocatedNode.code");
     UntypedDeclarationStatementList.setDataPrototype             ( "SgUntypedDeclarationStatementPtrList", "decl_list", "",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedFunctionDeclarationList.setFunctionPrototype ( "HEADER_UNTYPED_FUNCTION_DECLARATION_LIST", "../Grammar/LocatedNode.code");
     UntypedFunctionDeclarationList.setDataPrototype     ( "SgUntypedFunctionDeclarationPtrList", "func_list", "",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedInitializedNameList.setFunctionPrototype     ( "HEADER_UNTYPED_INITIALIZED_NAME_LIST", "../Grammar/LocatedNode.code");
     UntypedInitializedNameList.setDataPrototype         ( "SgUntypedInitializedNamePtrList", "name_list", "",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     UntypedNameList.setFunctionPrototype                ( "HEADER_UNTYPED_NAME_LIST", "../Grammar/LocatedNode.code");
     UntypedNameList.setDataPrototype                    ( "SgUntypedNamePtrList", "name_list", "",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     UntypedTokenList.setFunctionPrototype               ( "HEADER_UNTYPED_TOKEN_LIST", "../Grammar/LocatedNode.code");
     UntypedTokenList.setDataPrototype                   ( "SgUntypedTokenPtrList", "token_list", "",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedTokenPairList.setFunctionPrototype           ( "HEADER_UNTYPED_TOKEN_PAIR_LIST", "../Grammar/LocatedNode.code");
     UntypedTokenPairList.setDataPrototype               ( "SgUntypedTokenPairPtrList", "token_pair_list", "",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     UntypedName.setFunctionPrototype  ( "HEADER_UNTYPED_NAME", "../Grammar/LocatedNode.code");
     UntypedName.setDataPrototype      ( "std::string", "name", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     UntypedToken.setFunctionPrototype ( "HEADER_UNTYPED_TOKEN", "../Grammar/LocatedNode.code");
     UntypedToken.setDataPrototype     ( "std::string", "lexeme_string", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     UntypedToken.setDataPrototype     ( "unsigned int", "classification_code", "= 0",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

     UntypedTokenPair.setFunctionPrototype ( "HEADER_UNTYPED_TOKEN_PAIR", "../Grammar/LocatedNode.code");
     UntypedTokenPair.setDataPrototype     ( "std::string", "lexeme_string_1", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     UntypedTokenPair.setDataPrototype     ( "unsigned int", "classification_code_1", "= 0",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     UntypedTokenPair.setDataPrototype     ( "std::string", "lexeme_string_2", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     UntypedTokenPair.setDataPrototype     ( "unsigned int", "classification_code_2", "= 0",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

  // Rasmussen (11/19/2018): Added modifiers to allow more type information (should remove some existing like is_constant)
     UntypedType.setFunctionPrototype ( "HEADER_UNTYPED_TYPE", "../Grammar/LocatedNode.code");
     UntypedType.setDataPrototype     ( "std::string", "type_name", "= \"\"",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedType.setDataPrototype     ( "SgUntypedExpression*", "type_kind", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UntypedType.setDataPrototype     ( "bool", "has_kind", "= false",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedType.setDataPrototype     ( "bool", "is_literal", "= false",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedType.setDataPrototype     ( "bool", "is_class", "= false",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedType.setDataPrototype     ( "bool", "is_intrinsic", "= false",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedType.setDataPrototype     ( "bool", "is_constant", "= false",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedType.setDataPrototype     ( "bool", "is_user_defined", "= false",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedType.setDataPrototype     ( "SgUntypedExpression*", "char_length_expression", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedType.setDataPrototype     ( "std::string", "char_length_string", "= \"\"",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedType.setDataPrototype     ( "bool", "char_length_is_string", "= false",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UntypedType.setDataPrototype     ( "SgUntypedExprListExpression*", "modifiers", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, DEF_DELETE);

  // DQ (12/9/2015): enum type for different types indentifable in the parsing.
     UntypedType.setDataPrototype     ( "SgUntypedType::type_enum", "type_enum_id", "= SgUntypedType::e_unknown",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // UntypedAttribute.setFunctionPrototype ( "HEADER_UNTYPED_ATTRIBUTE", "../Grammar/LocatedNode.code");
  // UntypedAttribute.setDataPrototype     ( "SgToken::ROSE_Fortran_Keywords", "type_name", "= SgToken::FORTRAN_UNKNOWN",
  //              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     UntypedArrayType.setFunctionPrototype ( "HEADER_UNTYPED_ARRAY_TYPE", "../Grammar/LocatedNode.code");
     UntypedTableType.setFunctionPrototype ( "HEADER_UNTYPED_TABLE_TYPE", "../Grammar/LocatedNode.code");

  // Rasmussen (12/20/2017): Making SgUntypedArrayType better able to carry information to SgArrayType (removed array_length_expression)
     UntypedArrayType.setDataPrototype ("SgUntypedExprListExpression*", "dim_info" , "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, DEF_DELETE);
     UntypedArrayType.setDataPrototype ("int", "rank" , "= 0",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Rasmussen (06/13/2019): Added UntypedTableType for Jovial tables.
     UntypedTableType.setDataPrototype ("SgUntypedType*", "base_type", "= NULL",
                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL,  NO_DELETE);
     UntypedTableType.setDataPrototype ("SgUntypedExprListExpression*", "dim_info" , "= NULL",
                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, DEF_DELETE);
     UntypedTableType.setDataPrototype ("int", "rank" , "= 0",
                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL,  NO_DELETE);
     UntypedTableType.setDataPrototype ("SgUntypedType::type_enum", "table_type_enum_id", "= SgUntypedType::e_unknown",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL,  NO_DELETE);

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

  // negara1 (08/10/2011): Added to SgLocatedNodeSupport, no need for additional functions.
     HeaderFileBody.setFunctionPrototype ( "HEADER_HEADER_FILE_BODY", "../Grammar/LocatedNode.code");

  // DQ (8/23/2018): Adding support for pointer to the SgSourceFile that will record a SgSourceFile 
  // IR node for the include file (so that we can save information about comments, CPP directives, 
  // and the token stream).  This allows us to build the associated file information in the frontend
  // instead of building it in the backend (when it is too late to generate the token stream and it's 
  // mapping to what might be an already modified AST).  It might be that this IR node (SgSourceFile) 
  // could replace the SgHeaderFileBody IR node (but let's hold of on that idea for now).
     HeaderFileBody.setDataPrototype ("SgSourceFile*", "include_file" , "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


     CommonBlockObject.setFunctionPrototype ( "HEADER_COMMON_BLOCK_OBJECT", "../Grammar/Support.code");
     CommonBlockObject.setDataPrototype     ( "std::string", "block_name", "=\"\"",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     CommonBlockObject.setDataPrototype     ( "SgExprListExp*", "variable_reference_list", "= NULL",
// Liao 12/9/2010, it should be traversable to reach varRefExp     
//                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

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

#if 0
  // DQ (10/10/2014): Older version of code (marked as NO_CONSTRUCTOR_PARAMETER).
     InitializedName.setDataPrototype("SgName","name", "= NULL",
          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#else
  // DQ (10/10/2014): Modified to make this more suitable for support via Aterm to AST generation.
     InitializedName.setDataPrototype("SgName","name", "= NULL",
          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif
#else
     InitializedName.setDataPrototype("SgName","name", "= \"\"",
          NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif
  // DQ (8/18/2014): Added Microsoft specific extension for the uuid string option.
     InitializedName.setDataPrototype("std::string", "microsoft_uuid_string", "=\"\"",
          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


  // FMZ (4/7/2009): Added for Cray pointer declaration
#if 0
#if USE_FORTRAN_IR_NODES
     InitializedName.setDataPrototype("bool","isCrayPointer", "= false",
          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     InitializedName.setDataPrototype("SgInitializedName*","crayPointee", "= NULL",
          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif 
#endif

#if 0
  // DQ (10/10/2014): Older version of code (marked as NO_CONSTRUCTOR_PARAMETER).
  // DQ (7/20/2004):  think this is the root of the problems in cycles when we traverse types!
     InitializedName.setDataPrototype("SgType*","typeptr", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#else
  // DQ (10/10/2014): Modified to make this more suitable for support via Aterm to AST generation.
     InitializedName.setDataPrototype("SgType*","typeptr", "= NULL",
                                      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

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

  // This is python specific support (added by Michael Driscoll 2011-08-04).
     InitializedName.setDataPrototype ( "SgInitializedName::excess_specifier_enum", "excess_specifier", "= SgInitializedName::e_excess_specifier_none",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (8/09/2006): Support for asm register names when defined via strings (more general than the EDG mapping to the GNU supported register names)
  // This requirement comes from an Elsa test case: "int foo asm ("myfoo") = 2;" where the register name is unknown and so held as a string.
     InitializedName.setDataPrototype ( "std::string", "register_name_string", "= \"\"",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (3/30/2019): I think this is redundent with the similar data member below.
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

  // DQ (3/1/2013): To support letting zero being an acceptable value, we want to make the default value -1.
  // To do this we need for this value to be signed, since the range of values is so small it can be a short value.
  // DQ (1/3/2009): Added support for alignment specfication using gnu attributes (zero is used as the default to imply no alignment specification).
  // InitializedName.setDataPrototype("unsigned long int", "gnu_attribute_alignment", "= 0",
  //            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // InitializedName.setDataPrototype("short", "gnu_attribute_alignment", "= -1",
  //            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     InitializedName.setDataPrototype("int", "gnu_attribute_alignment", "= -1",
                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (1/3/2009): Added support for GNU attributes (reuse the enum declaration at the SgDeclarationModifier IR node).
     InitializedName.setDataPrototype("SgDeclarationModifier::gnu_declaration_visability_enum", "gnu_attribute_visability","= SgDeclarationModifier::e_unknown_visibility",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (9/11/2010): Added support for fortran "protected" marking of variables.
     InitializedName.setDataPrototype("bool", "protected_declaration", "= false",
                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (3/30/2019): This is needed to support pointers to member type variables, currently supporting in the 
  // SgVariableDeclaration, but that support is not general enough as in where pointer to member types are passed 
  // to function.
     InitializedName.setDataPrototype ( "int", "name_qualification_length", "= 0",
                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (3/30/2019): This is needed to support pointers to member type variables, currently supporting in the 
  // SgVariableDeclaration, but that support is not general enough as in where pointer to member types are passed 
  // to function.
     InitializedName.setDataPrototype("bool","type_elaboration_required","= false",
                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (3/30/2019): This is needed to support pointers to member type variables, currently supporting in the 
  // SgVariableDeclaration, but that support is not general enough as in where pointer to member types are passed 
  // to function.
     InitializedName.setDataPrototype("bool","global_qualification_required","= false",
                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
  // DQ (5/12/2011): Added support for name qualification on the type referenced by the InitializedName
  // (not the SgInitializedName itself since it might be referenced from several places, I think).
  // InitializedName.setDataPrototype ( "int", "name_qualification_length_for_type", "= 0",
  //            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     InitializedName.setDataPrototype ( "int", "name_qualification_length_for_type", "= 0",
                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
  // DQ (5/12/2011): Added information required for new name qualification support.
  // InitializedName.setDataPrototype("bool","type_elaboration_required_for_type","= false",
  //            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     InitializedName.setDataPrototype("bool","type_elaboration_required_for_type","= false",
                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
  // DQ (5/12/2011): Added information required for new name qualification support.
  // InitializedName.setDataPrototype("bool","global_qualification_required_for_type","= false",
  //            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     InitializedName.setDataPrototype("bool","global_qualification_required_for_type","= false",
                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#if 0
  // DQ (11/18/2013): Added final to support Java (which can use it to represent const function parameters in function declarations).
  // This support is represented as a declaration modifier (but that is not sufficient for use in function parameters).
  // As supported in declaration modifiers, this use is semantically different than its use in function parameters.
     InitializedName.setDataPrototype("bool","isFinal","= false",
                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif
  // DQ (2/2/2014): The secondary declaration for an array may be specified using empty bracket sysntax.
  // For example: "int array[];" This can be important to preserve when the primary declaration uses an
  // array bound that is declared between the secondary and primary declarations.  See test2014_81.c and
  // test2014_06.C.
     InitializedName.setDataPrototype("bool", "hasArrayTypeWithEmptyBracketSyntax", "= false",
                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (7/26/2014): Added support for C11 "_Alignas" keyword (alternative alignment specification).
     InitializedName.setDataPrototype("bool","using_C11_Alignas_keyword","= false",
                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     InitializedName.setDataPrototype("SgNode*","constant_or_type_argument_for_Alignas_keyword","= NULL",
                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (8/2/2014): Using C++11 auto keyword.
     InitializedName.setDataPrototype("bool","using_auto_keyword","= false",
                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     InitializedName.setDataPrototype("SgType *","auto_decltype","= NULL",
                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (1/24/2016): Adding support to mark this to use the __device__ keyword.
     InitializedName.setDataPrototype("bool","using_device_keyword","= false",
                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (11/14/2016): This is C++11 syntax for direct brace initalization (e.g. int n{} for EDG 4.11 and greater).
     InitializedName.setDataPrototype     ( "bool", "is_braced_initialized", "= false",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (5/30/2019): Is initialized using the "A a = B" copy constructor syntax as opposed to the "A a(B)" syntax.
  // This appears to make a different in Cxx11_tests/test2019_454.C.
     InitializedName.setDataPrototype     ( "bool", "using_assignment_copy_constructor_syntax", "= false",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     InitializedName.setDataPrototype     ( "bool", "needs_definitions", "= false",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (2/10/2019): Need to be able to specify function parameters that are a part of C++11 parameter pack associated with variadic templates. 
     InitializedName.setDataPrototype     ( "bool", "is_parameter_pack", "= false",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     InitializedName.setDataPrototype     ( "bool", "is_pack_element", "= false",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ(1/13/2014): Added Java support for JavaMemberValuePair
     JavaMemberValuePair.setFunctionPrototype     ( "HEADER_JAVA_MEMBER_VALUE_PAIR", "../Grammar/LocatedNode.code");
     JavaMemberValuePair.setDataPrototype("SgName","name", "= NULL",
          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     JavaMemberValuePair.setDataPrototype("SgExpression*","value", "= NULL",
          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (9/3/2014): Adding support for C++11 lambda expresions.
#if 0
     LambdaCapture.setDataPrototype ( "SgInitializedName*", "capture_variable", "= NULL",
                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     LambdaCapture.setDataPrototype ( "SgInitializedName*", "source_closure_variable", "= NULL",
                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     LambdaCapture.setDataPrototype ( "SgInitializedName*", "closure_variable", "= NULL",
                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#else
  // DQ (/3/2014): I think this makes more sense to be an expression (typically a SgVarRefExp).
     LambdaCapture.setDataPrototype ( "SgExpression*", "capture_variable", "= NULL",
                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     LambdaCapture.setDataPrototype ( "SgExpression*", "source_closure_variable", "= NULL",
                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     LambdaCapture.setDataPrototype ( "SgExpression*", "closure_variable", "= NULL",
                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif
     LambdaCapture.setDataPrototype ( "bool", "capture_by_reference", "= false",
                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     LambdaCapture.setDataPrototype ( "bool", "implicit", "= false",
                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     LambdaCapture.setDataPrototype ( "bool", "pack_expansion", "= false",
                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     LambdaCaptureList.setDataPrototype ( "SgLambdaCapturePtrList", "capture_list", "",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);


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

  // DQ (9/3/2014): Adding support for C++11 lambda expresions.
     LambdaCapture.setFunctionSource ( "SOURCE_LAMBDA_CAPTURE", "../Grammar/LocatedNode.code");
     LambdaCaptureList.setFunctionSource ( "SOURCE_LAMBDA_CAPTURE_LIST", "../Grammar/LocatedNode.code");

  // ***************************************************************************************
  // ***************************************************************************************
  //                                 ATerm IR Node Support
  // ***************************************************************************************
  // ***************************************************************************************
  // DQ (4/20/2014): Added support for ATerms in the IR.

  // Rasmussen (04/17/2019): Support for ATerms has been deprecated.
  // Aterm.setFunctionSource    ( "SOURCE_ATERM_NODE", "../Grammar/LocatedNode.code");

  // ***************************************************************************************
  // ***************************************************************************************
  //                                 Untyped IR Node Support
  // ***************************************************************************************
  // ***************************************************************************************
  // DQ (11/26/2013): Adding support for untyped AST IR nodes to support translation of ATterm 
  // based untyped ASTs into ROSE so that we will have tools (inherited attribute and synthizied 
  // attribute traversals) from which to build the ROSE AST (typed AST) and define a proper frontend.
  // Rasmussen (12/20/2017): Added SgUntypedExprListExpression
  // Rasmussen (01/22/2018): Added SgUntypedSubscriptExpression
  // Rasmussen (09/04/2018): Added SgUntypedNamedExpression

     UntypedNode.setFunctionSource                  ( "SOURCE_UNTYPED_NODE", "../Grammar/LocatedNode.code");

     UntypedExpression.setFunctionSource            ( "SOURCE_UNTYPED_EXPRESSION", "../Grammar/LocatedNode.code");
     UntypedUnaryOperator.setFunctionSource         ( "SOURCE_UNTYPED_UNARY_OPERATOR", "../Grammar/LocatedNode.code");
     UntypedBinaryOperator.setFunctionSource        ( "SOURCE_UNTYPED_BINARY_OPERATOR", "../Grammar/LocatedNode.code");
     UntypedExprListExpression.setFunctionSource    ( "SOURCE_UNTYPED_EXPR_LIST_EXPRESSION", "../Grammar/LocatedNode.code");
     UntypedValueExpression.setFunctionSource       ( "SOURCE_UNTYPED_VALUE_EXPRESSION", "../Grammar/LocatedNode.code");
     UntypedOtherExpression.setFunctionSource       ( "SOURCE_UNTYPED_OTHER_EXPRESSION", "../Grammar/LocatedNode.code");
     UntypedNamedExpression.setFunctionSource       ( "SOURCE_UNTYPED_NAMED_EXPRESSION", "../Grammar/LocatedNode.code");
     UntypedNullExpression.setFunctionSource        ( "SOURCE_UNTYPED_NULL_EXPRESSION", "../Grammar/LocatedNode.code");
     UntypedReferenceExpression.setFunctionSource   ( "SOURCE_UNTYPED_REFERENCE_EXPRESSION", "../Grammar/LocatedNode.code");
     UntypedSubscriptExpression.setFunctionSource   ( "SOURCE_UNTYPED_SUBSCRIPT_EXPRESSION", "../Grammar/LocatedNode.code");
     UntypedArrayReferenceExpression.setFunctionSource ( "SOURCE_UNTYPED_ARRAY_REFERENCE_EXPRESSION", "../Grammar/LocatedNode.code");
     UntypedFunctionCallOrArrayReferenceExpression.setFunctionSource ( "SOURCE_UNTYPED_FUNCTION_CALL_OR_ARRAY_REFERENCE_EXPRESSION", "../Grammar/LocatedNode.code");

     UntypedStatement.setFunctionSource             ( "SOURCE_UNTYPED_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedAssignmentStatement.setFunctionSource   ( "SOURCE_UNTYPED_ASSIGNMENT_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedBlockStatement.setFunctionSource        ( "SOURCE_UNTYPED_BLOCK_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedExpressionStatement.setFunctionSource   ( "SOURCE_UNTYPED_EXPRESSION_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedFunctionCallStatement.setFunctionSource ( "SOURCE_UNTYPED_FUNCTION_CALL_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedImageControlStatement.setFunctionSource ( "SOURCE_UNTYPED_IMAGE_CONTROL_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedOtherStatement.setFunctionSource        ( "SOURCE_UNTYPED_OTHER_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedUseStatement.setFunctionSource          ( "SOURCE_UNTYPED_USE_STATEMENT", "../Grammar/LocatedNode.code");

  // DQ (10/3/2017): New statements specific to general language support.  The philosophy is to add the union of all statements
  // for all languages to the set of untyped IR nodes and provide an enum code to support the larger number of expressions.
  // Rasmussen (1/16/2018): Added UntypedNullDeclaration
  // Rasmussen (5/28/2018): Added UntypedLabelStatement
     UntypedNullDeclaration.setFunctionSource               ( "SOURCE_UNTYPED_NULL_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedNullStatement.setFunctionSource                 ( "SOURCE_UNTYPED_NULL_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedLabelStatement.setFunctionSource                ( "SOURCE_UNTYPED_LABEL_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedIfStatement.setFunctionSource                   ( "SOURCE_UNTYPED_IF_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedCaseStatement.setFunctionSource                 ( "SOURCE_UNTYPED_CASE_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedLoopStatement.setFunctionSource                 ( "SOURCE_UNTYPED_LOOP_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedWhileStatement.setFunctionSource                ( "SOURCE_UNTYPED_WHILE_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedForStatement.setFunctionSource                  ( "SOURCE_UNTYPED_FOR_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedForAllStatement.setFunctionSource               ( "SOURCE_UNTYPED_FORALL_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedExitStatement.setFunctionSource                 ( "SOURCE_UNTYPED_EXIT_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedGotoStatement.setFunctionSource                 ( "SOURCE_UNTYPED_GOTO_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedProcedureCallStatement.setFunctionSource        ( "SOURCE_UNTYPED_PROCEDURE_CALL_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedReturnStatement.setFunctionSource               ( "SOURCE_UNTYPED_RETURN_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedExtendedReturnStatement.setFunctionSource       ( "SOURCE_UNTYPED_EXTENDED_RETURN_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedStopStatement.setFunctionSource                 ( "SOURCE_UNTYPED_STOP_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedAcceptStatement.setFunctionSource               ( "SOURCE_UNTYPED_ACCEPT_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedEntryCallStatement.setFunctionSource            ( "SOURCE_UNTYPED_ENTRY_CALL_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedRequeueStatement.setFunctionSource              ( "SOURCE_UNTYPED_REQUEUE_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedDelayUntilStatement.setFunctionSource           ( "SOURCE_UNTYPED_DELAY_UNTIL_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedDelayRelativeStatement.setFunctionSource        ( "SOURCE_UNTYPED_DELAY_RELATIVE_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedTerminateAlternativeStatement.setFunctionSource ( "SOURCE_UNTYPED_TERMINATE_ALTERNATIVE_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedSelectiveAcceptStatement.setFunctionSource      ( "SOURCE_UNTYPED_SELECTIVE_ACCEPT_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedTimedEntryCallStatement.setFunctionSource       ( "SOURCE_UNTYPED_TIMED_ENTRY_CALL_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedConditionalEntryCallStatement.setFunctionSource ( "SOURCE_UNTYPED_CONDITIONAL_ENTRY_CALL_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedAsynchronousSelectStatement.setFunctionSource   ( "SOURCE_UNTYPED_ASYNCHRONOUS_SELECT_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedAbortStatement.setFunctionSource                ( "SOURCE_UNTYPED_ABORT_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedRaiseStatement.setFunctionSource                ( "SOURCE_UNTYPED_RAISE_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedCodeStatement.setFunctionSource                 ( "SOURCE_UNTYPED_CODE_STATEMENT", "../Grammar/LocatedNode.code");

     UntypedDeclarationStatement.setFunctionSource     ( "SOURCE_UNTYPED_DECLARATION_STATEMENT", "../Grammar/LocatedNode.code");
     UntypedDirectiveDeclaration.setFunctionSource     ( "SOURCE_UNTYPED_DIRECTIVE_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedEnumDeclaration.setFunctionSource          ( "SOURCE_UNTYPED_ENUM_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedNameListDeclaration.setFunctionSource      ( "SOURCE_UNTYPED_NAMELIST_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedImplicitDeclaration.setFunctionSource      ( "SOURCE_UNTYPED_IMPLICIT_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedVariableDeclaration.setFunctionSource      ( "SOURCE_UNTYPED_VARIABLE_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedProgramHeaderDeclaration.setFunctionSource ( "SOURCE_UNTYPED_PROGRAM_HEADER_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedFunctionDeclaration.setFunctionSource      ( "SOURCE_UNTYPED_FUNCTION_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedSubroutineDeclaration.setFunctionSource    ( "SOURCE_UNTYPED_SUBROUTINE_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedInterfaceDeclaration.setFunctionSource     ( "SOURCE_UNTYPED_INTERFACE_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedModuleDeclaration.setFunctionSource        ( "SOURCE_UNTYPED_MODULE_DECLARATION", "../Grammar/LocatedNode.code");

  // Rasmussen (8/06/2018): Added SgUntypedInitializedNameListDeclaration
     UntypedInitializedNameListDeclaration.setFunctionSource ( "SOURCE_UNTYPED_INITIALIZED_NAMELIST_DECLARATION",
                                                               "../Grammar/LocatedNode.code" );

     UntypedPackageDeclaration.setFunctionSource          ( "SOURCE_UNTYPED_PACKAGE_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedStructureDeclaration.setFunctionSource        ( "SOURCE_UNTYPED_STRUCTURE_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedTypedefDeclaration.setFunctionSource          ( "SOURCE_UNTYPED_TYPEDEF_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedExceptionDeclaration.setFunctionSource        ( "SOURCE_UNTYPED_EXCEPTION_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedExceptionHandlerDeclaration.setFunctionSource ( "SOURCE_UNTYPED_EXCEPTION_HANDLER_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedTaskDeclaration.setFunctionSource             ( "SOURCE_UNTYPED_TASK_DECLARATION", "../Grammar/LocatedNode.code");
     UntypedUnitDeclaration.setFunctionSource             ( "SOURCE_UNTYPED_UNIT_DECLARATION", "../Grammar/LocatedNode.code");

  // Rasmussen (8/16/2017): Added new IR node to represent a Fortran submodule (a submodule extends an existing module)
     UntypedSubmoduleDeclaration.setFunctionSource     ( "SOURCE_UNTYPED_SUBMODULE_DECLARATION", "../Grammar/LocatedNode.code");

  // Rasmussen (8/17/2017): Added new IR node to represent a Fortran block-data program unit
     UntypedBlockDataDeclaration.setFunctionSource     ( "SOURCE_UNTYPED_BLOCK_DATA_DECLARATION", "../Grammar/LocatedNode.code");

     UntypedScope.setFunctionSource         ( "SOURCE_UNTYPED_SCOPE", "../Grammar/LocatedNode.code");
     UntypedFunctionScope.setFunctionSource ( "SOURCE_UNTYPED_FUNCTION_SCOPE", "../Grammar/LocatedNode.code");
     UntypedModuleScope.setFunctionSource   ( "SOURCE_UNTYPED_MODULE_SCOPE", "../Grammar/LocatedNode.code");
     UntypedGlobalScope.setFunctionSource   ( "SOURCE_UNTYPED_GLOBAL_SCOPE", "../Grammar/LocatedNode.code");
     UntypedStructureDefinition.setFunctionSource ( "SOURCE_UNTYPED_STRUCTURE_DEFINITION", "../Grammar/LocatedNode.code");

  // DQ (3/3/2014): Added new IR nodes specific to work with Craig on Fortran support.
     UntypedInitializedName.setFunctionSource ( "SOURCE_UNTYPED_INITIALIZED_NAME", "../Grammar/LocatedNode.code");
     UntypedAttribute.setFunctionSource       ( "SOURCE_UNTYPED_ATTRIBUTE", "../Grammar/LocatedNode.code");
     UntypedFile.setFunctionSource            ( "SOURCE_UNTYPED_FILE", "../Grammar/LocatedNode.code");

     UntypedStatementList.setFunctionSource           ( "SOURCE_UNTYPED_STATEMENT_LIST", "../Grammar/LocatedNode.code");
     UntypedDeclarationStatementList.setFunctionSource( "SOURCE_UNTYPED_DECLARATION_STATEMENT_LIST", "../Grammar/LocatedNode.code");
     UntypedFunctionDeclarationList.setFunctionSource ( "SOURCE_UNTYPED_FUNCTION_DECLARATION_LIST", "../Grammar/LocatedNode.code");
     UntypedInitializedNameList.setFunctionSource     ( "SOURCE_UNTYPED_INITIALIZED_NAME_LIST", "../Grammar/LocatedNode.code");

     UntypedName.setFunctionSource                    ( "SOURCE_UNTYPED_NAME", "../Grammar/LocatedNode.code");
     UntypedNameList.setFunctionSource                ( "SOURCE_UNTYPED_NAME_LIST", "../Grammar/LocatedNode.code");

     UntypedToken.setFunctionSource                   ( "SOURCE_UNTYPED_TOKEN", "../Grammar/LocatedNode.code");
     UntypedTokenPair.setFunctionSource               ( "SOURCE_UNTYPED_TOKEN_PAIR", "../Grammar/LocatedNode.code");
     UntypedTokenList.setFunctionSource               ( "SOURCE_UNTYPED_TOKEN_LIST", "../Grammar/LocatedNode.code");
     UntypedTokenPairList.setFunctionSource           ( "SOURCE_UNTYPED_TOKEN_PAIR_LIST", "../Grammar/LocatedNode.code");

     UntypedType.setFunctionSource      ( "SOURCE_UNTYPED_TYPE", "../Grammar/LocatedNode.code");
     UntypedArrayType.setFunctionSource ( "SOURCE_UNTYPED_ARRAY_TYPE", "../Grammar/LocatedNode.code");
     UntypedTableType.setFunctionSource ( "SOURCE_UNTYPED_TABLE_TYPE", "../Grammar/LocatedNode.code");

  // DQ (10/6/2008): Moved from SgSupport.
     RenamePair.setFunctionSource ( "SOURCE_RENAME_PAIR", "../Grammar/LocatedNode.code");

  // DQ (10/6/2008): Moved from SgSupport.
     InterfaceBody.setFunctionSource ( "SOURCE_INTERFACE_BODY", "../Grammar/LocatedNode.code");

  // negara1 (08/10/2011): Added to SgLocatedNodeSupport.
     HeaderFileBody.setFunctionSource ( "SOURCE_HEADER_FILE_BODY", "../Grammar/LocatedNode.code");     

  // DQ (11/21/2007): support for common block statements
     CommonBlockObject.setFunctionSource ( "SOURCE_COMMON_BLOCK_OBJECT", "../Grammar/Support.code");

     InitializedName.setFunctionSource ( "SOURCE_INITIALIZED_NAME", "../Grammar/Support.code");
  // Some functions we want to only be defined for higher level grammars (not the root C++ grammar)
     ROSE_ASSERT(InitializedName.associatedGrammar != NULL);
  // ROSE_ASSERT(InitializedName.associatedGrammar->getParentGrammar() != NULL);
  // ROSE_ASSERT(InitializedName.associatedGrammar->getParentGrammar()->getGrammarName() != NULL);
     if (verbose) {
         printf ("### InitializedName.associatedGrammar->getParentGrammar()->getGrammarName() = %s \n",
                 (InitializedName.associatedGrammar->getParentGrammar() == NULL) ?
                 "ROOT GRAMMAR" :
                 InitializedName.associatedGrammar->getParentGrammar()->getGrammarName().c_str());
     }

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


  // DQ(1/13/2014): Added Java support for JavaMemberValuePair
     JavaMemberValuePair.setFunctionSource ("SOURCE_JAVA_MEMBER_VALUE_PAIR", "../Grammar/LocatedNode.code");


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
     OmpScheduleClause.setDataPrototype("SgOmpClause::omp_schedule_kind_enum", "kind", "=e_omp_schedule_unknown",
                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     OmpScheduleClause.setDataPrototype ( "SgExpression*", "chunk_size", "= NULL",
                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, CLONE_PTR);
 
     // clauses with variable lists 
     // Liao 9/27/2010, per user's report, modeling the variable reference use SgVarRefExp
     //OmpVariablesClause.setDataPrototype ( "SgInitializedNamePtrList", "variables", "",
     //OmpVariablesClause.setDataPrototype ( "SgVarRefExpPtrList", "variables", "",
     //                    NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     // Using a SgNode for variable list, avoiding mixed container + simple member
     OmpVariablesClause.setDataPrototype ( "SgExprListExp*", "variables", "= NULL",
                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, CLONE_PTR);
     // linear (varlist[:step])
     OmpLinearClause.setDataPrototype ( "SgExpression*", "step", "= NULL",
                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, CLONE_PTR);
     
     // aligned (varlist[:alignment])
     OmpAlignedClause.setDataPrototype ( "SgExpression*", "alignment", "= NULL",
                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, CLONE_PTR);

      // default (private | firstprivate | shared | none)
     OmpDefaultClause.setDataPrototype("SgOmpClause::omp_default_option_enum", "data_sharing", "=e_omp_default_unknown",
                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

      // atomic clause is one of : read, write, update, or capture
     OmpAtomicClause.setDataPrototype("SgOmpClause::omp_atomic_clause_enum", "atomicity", "=e_omp_atomic_clause_unknown",
                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
 
     // proc_bind(master | close | spread) 
     OmpProcBindClause.setDataPrototype("SgOmpClause::omp_proc_bind_policy_enum", "policy", "=e_omp_proc_bind_policy_unknown",
                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     // reduction(op:variables) 
     OmpReductionClause.setDataPrototype("SgOmpClause::omp_reduction_operator_enum", "operation", "=e_omp_reduction_unknown",
                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     
     // depend(type:variables) 
     OmpDependClause.setDataPrototype("SgOmpClause::omp_dependence_type_enum", "dependence_type", "=e_omp_depend_unknown",
                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     OmpDependClause.setDataPrototype("std::map<SgSymbol*,  std::vector < std::pair <SgExpression*, SgExpression*> > >", "array_dimensions", "",
                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     // map (inout|alloc|in|out:variable_list) , a variable could be array type with additional dimension info, such as a[0:n][0:m]
     OmpMapClause.setDataPrototype("SgOmpClause::omp_map_operator_enum", "operation", "=e_omp_map_unknown",
                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // TODO how to traverse this map?  the bound variables may need to be visited.
     OmpMapClause.setDataPrototype("std::map<SgSymbol*,  std::vector < std::pair <SgExpression*, SgExpression*> > >", "array_dimensions", "",
                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // experimental data distribution information for each mapped array : dist_data (duplicate, block(n), cyclic(4)),  up to size 3
     // I choose to attach this info. to map clauses since dist_data() does not exist alone. It must follow a map() clause
     OmpMapClause.setDataPrototype("std::map<SgSymbol*,  std::vector < std::pair <SgOmpClause::omp_map_dist_data_enum, SgExpression*> > >", "dist_data_policies", "",
                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif




   } // end





















