

THIS FILE IS NOT USED!!!!


// ################################################################
// #                           Header Files                       #
// ################################################################

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// Define the C++ grammars used in ROSE
// commented out by Bobby 10/16/2001
//#include "rose.h"

// Define the grammar, terminals and nonterminal classes
#include "grammar.h"
#include "terminal.h"
#include "grammarString.h"
#include "nonterminal.h"
// ################################################################
// #                 Grammar Static Data Members                  #
// ################################################################


// ################################################################
// #                   Grammar Member Functions                   #
// ################################################################

void
Grammar::addNewTerminal ( SgProject & project, Terminal & X )
   {
  // This is the mechanism for adding new types to an existing grammar. This function
  // will more generally show how new terminals are added to an existing grammar.  It 
  // will be generalized later to the addition of nonterminals (implying the addition 
  // of subtrees of grammar being added to an existing grammar).  The later use of this 
  // will be important for the addition of the A++ type system 
  //   -->  (realArray:doubleArray,floatArray; intArray, Indexing: Index,Range).

     printf ("Inside of Grammar::addNewTerminal (Terminal) X(%s:%s) \n",X.lexeme,X.name);

  // For now we only call this for child terminals (so check this)
     ROSE_ASSERT(X.parentTerminal != NULL);

  // This is a strange little construction to premit us to use the exisitng macro
  // NonTerminal & result = *this;

  // MACRO_NonTerminal(orTokenPointerArray,orNonTerminalPointerArray,(*this))
  // orTokenPointerArray.addElement(Y);

  // Here the new terminal is attached to the grammar tree and depending upon the 
  // placement of the new terminal in the tree (in the X_Type, X_Expression, or X_Statement
  // branch the behavior is different).  

  // New terminals/nonterminals are to be placed in the X_??? sub-branch of the ??? branch
  // (where ??? is one of: X_Type, X_Expression, or X_Statement).  The nonterminal branches
  // are given a branch hierarchy (from highest to lowest) X_Type, X_Expression, and X_Statement.
  // Addition of the new terminal/nonterminal to a level in the branch causes the X_???
  // sub-branches in the lower level branches to be populated with full copies of the
  // terminal/nonterminals contained in the non_X_??? sub-branches.  

  // Note: subtrees of X version terminals/nonterminals must be organized similarly as to non X versions
  // of the Grammar.  For example: 
  //      The addition of a new ClassType requires the addition of a new NamedType with the 
  //      ClassType derived from the NamedType consistant with the C++ grammar (or non X 
  //      version of the terminal/nonterminal branches of the heirarchy representing the grammar).

  // Question:
  //    Should new types be derived from the ClassType class or the NamedType class?
  // At present new types (doubleArray for example) are derived from the NamedType, 
  // but this is a general question that effects all new terminals (do we copy of do 
  // we use derivation?).

  // Examples:
  //   1) Array Type: The addition of a new type terminal in the type system (Type branch, and X_Type sub-branch)
  //      causes the generation of expressions that could be used of that type (X_Expressions) and
  //      statements using the X_Expressions (X_Statements).  These new possible terminals and nonterminals
  //      must be built into the grammar (so that expressions and statements using the new type (X_Type) 
  //      can be defined (and recognized)).
  //   2) Where statements: The addition of a terminal to define a where statements would modify the 
  //      lowest level of the branch hierarchy (thus not affecting types or expressions).  The where
  //      statement is a contraint upon the "for" statement and so the where statement would be a 
  //      child of the "for" statement.  Thus whenever the "for" statement is parsed, the
  //      where statements constraint would be tested (or an attempt would be made to build the 
  //      where statement (by calling the child terminal's parse function)).

  // Handle the type issues represented by the new terminal
     ROSE_ASSERT (X.isType() == TRUE);

     if (X.isType() == TRUE)
        {
          printf ("Fixup type system defined in grammar! \n");

       // The addition of a type to the grammar implies that the new type is added 
       // to the X_Type nonterminal.
       // ROSE_ASSERT (Grammar::isSameName(name,"X_Type") == TRUE);
          ROSE_ASSERT (X.getParentTerminal() != NULL);
          ROSE_ASSERT (X.getParentTerminal()->getName() == "ClassType");

       // We can't do this since we must reproduce the correct subtree with Y as a leaf!
       // add the input terminal to the list of terminals
       // orTokenPointerArray.addElement (Y);

       // Now add a few other related types that support the new type:
       //      ReferenceType, PointerType, TypedefType, ArrayType, 
       //      MemberFunctionType, PartialFunctionType,
       //      ClassType (to recognize derived classes), FunctionType, PointerMemberType?

	  BUILD_CHILD_TERMINAL_MACRO ( X_ReferenceType       , "ReferenceType",
                                      "X_ReferenceType"      , "X_TYPE_REFERENCE")
          BUILD_CHILD_TERMINAL_MACRO ( X_PointerMemberType   , "PointerMemberType",
                                      "X_PointerMemberType"  , "X_T_MEMBER_POINTER" )
          BUILD_CHILD_TERMINAL_MACRO ( X_TypedefType         , "TypedefType",
                                      "X_TypedefType"        , "X_T_TYPEDEF" )
       // BUILD_CHILD_TERMINAL_MACRO ( X_ModifierType        , "ModifierType",
       //                             "X_ModifierType"       , "X_T_MODIFIER" )
          BUILD_CHILD_TERMINAL_MACRO ( X_ArrayType           , "ArrayType",
                                      "X_ArrayType"          , "X_T_ARRAY" )
#if 0
       // Skip X function stuff for now!
          BUILD_CHILD_TERMINAL_MACRO ( X_PartialFunctionType , "PartialFunctionType",
                                      "X_PartialFunctionType", "X_T_PARTIAL_FUNCTION" )

          BUILD_CHILD_NONTERMINAL_MACRO (X_MemberFunctionType, X_PartialFunctionType,
                            "MemberFunctionType","X_MemberFunctionType","X_T_MEMBERFUNCTION")

          BUILD_CHILD_NONTERMINAL_MACRO (X_FunctionType, X_MemberFunctionType,
                            "FunctionType","X_FunctionType","X_T_FUNCTION")
#endif

          BUILD_CHILD_NONTERMINAL_MACRO (X_PointerType, X_PointerMemberType,
                      "PointerType","X_PointerType","X_T_POINTER")

#if 0
       // Skip the construction of the X_ClassType as a nonterminal and the addition of X
          BUILD_CHILD_TERMINAL_MACRO ( X_ClassType           , "ClassType",
                                      "X_ClassType"          , "X_T_CLASS" )
#else
       // Build the X_ClassType as a nonterminal and add X to its list of terminals!

       // This makes this code rather specific to the addition 
       // of a new ClassType object (through derivation!)
       // BUILD_CHILD_NONTERMINAL_MACRO (X_ClassType, X, "ClassType","X_ClassType","X_T_CLASS");
          Terminal    &   ClassType    = getTerminal("ClassType");
          NonTerminal*  X_ClassTypePtr = new NonTerminal(ClassType);
	  ROSE_ASSERT(X_ClassTypePtr != NULL);
          NonTerminal & X_ClassType    = *X_ClassTypePtr;

          X_ClassType.setName("X_ClassType");
          X_ClassType.setTagName("X_T_CLASS");
          X_ClassType.setParentTerminal(&ClassType);

       // We only have to copy the terminal data since the lists specific to the nonterminal data are empty by definition!
          X_ClassType.copyTerminalData (&ClassType);

       // Now add the terminal to the list of terminals in the associated grammar
          addGrammarElement(X_ClassType);

          ClassType.addChild(&X_ClassType);

          ROSE_ASSERT(X_ClassType.isChild() == TRUE);

          ROSE_ASSERT(X_ClassType.automaticGenerationOfDestructor          ==
                      X_ClassType.getParentTerminal()->automaticGenerationOfDestructor);
          ROSE_ASSERT(X_ClassType.associatedGrammar != NULL);
          X_ClassType.setExpression ( X );
       // X_ClassType.setParentTerminal(&(getNonTerminal(strdup(ORIGINAL_NON_TERMINAL_NAME_STRING))));
          ROSE_ASSERT(X_ClassType.isTemporary() == FALSE);
          ROSE_ASSERT(X_ClassType.isChild() == TRUE);								     
          X_ClassType.setName("X_ClassType");
          X_ClassType.setLexeme("X_ClassType");
          X_ClassType.setTagName("X_T_CLASS");

       // Now we need to skip the generation of the parse friend function for the input X type
       // because it is a child terminal and for the X_ClassType since it no SgX_ClassType 
       // exists in the lower level grammar.
          X.excludeFunctionPrototype           ( "HEADER_PARSER", "Grammar/Node.code");
          X_ClassType.excludeFunctionPrototype ( "HEADER_PARSER", "Grammar/Node.code");
          X.excludeFunctionSource              ( "SOURCE_PARSER", "Grammar/parserSourceCode.macro" );
          X_ClassType.excludeFunctionSource    ( "SOURCE_PARSER", "Grammar/parserSourceCode.macro" );
#endif

          BUILD_CHILD_NONTERMINAL_MACRO (X_NamedType, X_ClassType | X_TypedefType,
                      "NamedType","X_NamedType","X_T_NAME");

          NonTerminal & X_Type = getNonTerminal("X_Type");

#if 1

       // Skip X function stuff for now!
       // X_Type.orTokenPointerArray.addElement(X_FunctionType);
          X_Type.addElement(X_Type.orTokenPointerArray, X_ReferenceType);
          X_Type.addElement(X_Type.orTokenPointerArray, X_PointerType);
       // X_Type.orTokenPointerArray.addElement(X_ClassType);
          X_Type.addElement(X_Type.orTokenPointerArray, X_NamedType);
       // X_Type.orTokenPointerArray.addElement(X_TypedefType);
       // X_Type.orTokenPointerArray.addElement(X_PartialFunctionType);
          X_Type.addElement(X_Type.orTokenPointerArray, X_ArrayType);

       // X_NamedType.orTokenPointerArray.addElement(X_TypedefType);
       // X_NamedType.orTokenPointerArray.addElement(X_ClassType);
       // X_FunctionType.orTokenPointerArray.addElement(X_MemberFunctionType);
       // X_MemberFunctionType.orTokenPointerArray.addElement(X_PartialFunctionType);

       // Now add the X type (input to this function!)
#if 0
       // Actually, we don't want to debugg it now!
       // X_NamedType.orTokenPointerArray.addElement(X);
          X_ClassType.orTokenPointerArray.addElement(X);
#endif

#endif

#if 0
       // DQ (10/13/2007): Commented out this output!
          printf ("X_PointerType ---> ");        X_PointerType.show(); printf ("\n\n");
       // printf ("X_MemberFunctionType ---> "); X_MemberFunctionType.show(); printf ("\n\n");
       // printf ("X_FunctionType ---> ");       X_FunctionType.show(); printf ("\n\n");
          printf ("X_NamedType ---> ");          X_NamedType.show(); printf ("\n\n");
          printf ("X_Type ---> ");               X_Type.show(); printf ("\n\n");
#endif

#if 0
       // print out all the available nonterminals (error checking/debugging)
          terminalList.display("Called from NonTerminal::operator|=()");
          nonTerminalList.display("Called from NonTerminal::operator|=()");
#endif

#if 0
	  printf ("Exiting after building X_Type subtree! \n");
	  ROSE_ABORT();
#endif
        }

  // Handle the expression issues represented by the new terminal
     if ( (X.isExpression() == TRUE) || (X.isType() == TRUE) )
        {
          printf ("Fixup expression system defined in grammar! \n");

       // If Y is a statement then the new terminal is added to the X_Statement nonterminal
          if (X.isType() == TRUE)
             {
               printf ("Fixup expression system with new type terminal! \n");

            // This builds all expressions except those associated with user defined member functions of the X class
            // these will be build in a later phase where the header file for the X class will be read and all member functions
            // will be added as function expressions (specified with a contraint on the class name and input types).

               BUILD_CHILD_TERMINAL_MACRO ( X_ExprListExp,            "ExprListExp",
                                           "X_ExprListExp",           "X_EXPR_LIST" );
               BUILD_CHILD_TERMINAL_MACRO ( X_VarRefExp,              "VarRefExp",
                                           "X_VarRefExp",             "X_VAR_REF" );
               BUILD_CHILD_TERMINAL_MACRO ( X_ClassNameRefExp,        "ClassNameRefExp",
                                           "X_ClassNameRefExp",       "X_CLASSNAME_REF" );
               BUILD_CHILD_TERMINAL_MACRO ( X_FunctionRefExp,         "FunctionRefExp",
                                           "X_FunctionRefExp",        "X_FUNCTION_REF" );
               BUILD_CHILD_TERMINAL_MACRO ( X_MemberFunctionRefExp,   "MemberFunctionRefExp",
                                           "X_MemberFunctionRefExp",  "X_MEMBER_FUNCTION_REF" );
               BUILD_CHILD_TERMINAL_MACRO ( X_FunctionCallExp,        "FunctionCallExp",
                                           "X_FunctionCallExp",       "X_FUNC_CALL" );
               BUILD_CHILD_TERMINAL_MACRO ( X_SizeOfOp,               "SizeOfOp",
                                           "X_SizeOfOp",              "X_SIZEOF_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_TypeIdOp,               "TypeIdOp",
                                           "X_TypeIdOp",              "X_TYPEID_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_ConditionalExp,         "ConditionalExp",
                                           "X_ConditionalExp",        "X_EXPR_CONDITIONAL" );
               BUILD_CHILD_TERMINAL_MACRO ( X_NewExp,                 "NewExp",
                                           "X_NewExp",                "X_NEW_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_DeleteExp,              "DeleteExp",
                                           "X_DeleteExp",             "X_DELETE_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_ThisExp,                "ThisExp",
                                           "X_ThisExp",               "X_THIS_NODE" );
               BUILD_CHILD_TERMINAL_MACRO ( X_RefExp,                 "RefExp",
                                           "X_RefExp",                "X_TYPE_REF" );
               BUILD_CHILD_TERMINAL_MACRO ( X_AggregateInitializer,   "AggregateInitializer",
                                           "X_AggregateInitializer",  "X_AGGREGATE_INIT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_ConstructorInitializer, "ConstructorInitializer",
                                           "X_ConstructorInitializer","X_CONSTRUCTOR_INIT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_AssignInitializer,      "AssignInitializer",
                                           "X_AssignInitializer",     "X_ASSIGN_INIT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_ExpressionRoot,         "ExpressionRoot",
                                           "X_ExpressionRoot",        "X_EXPRESSION_ROOT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_MinusOp,                "MinusOp",
                                           "X_MinusOp",               "X_UNARY_MINUS_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_UnaryAddOp,             "UnaryAddOp",
                                           "X_UnaryAddOp",            "X_UNARY_ADD_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_NotOp,                  "NotOp",
                                           "X_NotOp",                 "X_NOT_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_PointerDerefExp,        "PointerDerefExp",
                                           "X_PointerDerefExp",       "X_DEREF_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_AddressOfOp,            "AddressOfOp",
                                           "X_AddressOfOp",           "X_ADDRESS_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_MinusMinusOp,           "MinusMinusOp",
                                           "X_MinusMinusOp",          "X_MINUSMINUS_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_PlusPlusOp,             "PlusPlusOp",
                                           "X_PlusPlusOp",            "X_PLUSPLUS_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_BitComplementOp,        "BitComplementOp",
                                           "X_BitComplementOp",       "X_BIT_COMPLEMENT_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_CastExp,                "CastExp",
                                           "X_CastExp",               "X_CAST_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_ThrowOp,                "ThrowOp",
                                           "X_ThrowOp",               "X_THROW_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_ArrowExp,               "ArrowExp",
                                           "X_ArrowExp",              "X_POINTST_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_DotExp,                 "DotExp",
                                           "X_DotExp",                "X_RECORD_REF" );
               BUILD_CHILD_TERMINAL_MACRO ( X_DotStarOp,              "DotStarOp",
                                           "X_DotStarOp",             "X_DOTSTAR_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_ArrowStarOp,            "ArrowStarOp",
                                           "X_ArrowStarOp",           "X_ARROWSTAR_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_EqualityOp,             "EqualityOp",
                                           "X_EqualityOp",            "X_EQ_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_LessThanOp,             "LessThanOp",
                                           "X_LessThanOp",            "X_LT_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_GreaterThanOp,          "GreaterThanOp",
                                           "X_GreaterThanOp",         "X_GT_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_NotEqualOp,             "NotEqualOp",
                                           "X_NotEqualOp",            "X_NE_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_LessOrEqualOp,          "LessOrEqualOp",
                                           "X_LessOrEqualOp",         "X_LE_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_GreaterOrEqualOp,       "GreaterOrEqualOp",
                                           "X_GreaterOrEqualOp",      "X_GE_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_AddOp,                  "AddOp",
                                           "X_AddOp",                 "X_ADD_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_SubtractOp,             "SubtractOp",
                                           "X_SubtractOp",            "X_SUBT_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_MultiplyOp,             "MultiplyOp",
                                           "X_MultiplyOp",            "X_MULT_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_DivideOp,               "DivideOp",
                                           "X_DivideOp",              "X_DIV_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_IntegerDivideOp,        "IntegerDivideOp",
                                           "X_IntegerDivideOp",       "X_INTEGER_DIV_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_ModOp,                  "ModOp",
                                           "X_ModOp",                 "X_MOD_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_AndOp,                  "AndOp",
                                           "X_AndOp",                 "X_AND_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_OrOp,                   "OrOp",
                                           "X_OrOp",                  "X_OR_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_BitXorOp,               "BitXorOp",
                                           "X_BitXorOp",              "X_BITXOR_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_BitAndOp,               "BitAndOp",
                                           "X_BitAndOp",              "X_BITAND_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_BitOrOp,                "BitOrOp",
                                           "X_BitOrOp",               "X_BITOR_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_CommaOpExp,             "CommaOpExp",
                                           "X_CommaOpExp",            "X_COMMA_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_LshiftOp,               "LshiftOp",
                                           "X_LshiftOp",              "X_LSHIFT_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_RshiftOp,               "RshiftOp",
                                           "X_RshiftOp",              "X_RSHIFT_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_PntrArrRefExp,          "PntrArrRefExp",
                                           "X_PntrArrRefExp",         "X_ARRAY_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_ScopeOp,                "ScopeOp",
                                           "X_ScopeOp",               "X_SCOPE_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_AssignOp,               "AssignOp",
                                           "X_AssignOp",              "X_ASSIGN_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_PlusAssignOp,           "PlusAssignOp",
                                           "X_PlusAssignOp",          "X_PLUS_ASSIGN_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_MinusAssignOp,          "MinusAssignOp",
                                           "X_MinusAssignOp",         "X_MINUS_ASSIGN_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_AndAssignOp,            "AndAssignOp",
                                           "X_AndAssignOp",           "X_AND_ASSIGN_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_IorAssignOp,            "IorAssignOp",
                                           "X_IorAssignOp",           "X_IOR_ASSIGN_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_MultAssignOp,           "MultAssignOp",
                                           "X_MultAssignOp",          "X_MULT_ASSIGN_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_DivAssignOp,            "DivAssignOp",
                                           "X_DivAssignOp",           "X_DIV_ASSIGN_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_ModAssignOp,            "ModAssignOp",
                                           "X_ModAssignOp",           "X_MOD_ASSIGN_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_XorAssignOp,            "XorAssignOp",
                                           "X_XorAssignOp",           "X_XOR_ASSIGN_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_LshiftAssignOp,         "LshiftAssignOp",
                                           "X_LshiftAssignOp",        "X_LSHIFT_ASSIGN_OP" );
               BUILD_CHILD_TERMINAL_MACRO ( X_RshiftAssignOp,         "RshiftAssignOp",
                                           "X_RshiftAssignOp",        "X_RSHIFT_ASSIGN_OP" );
#if 1
            // The X Grammar likely does not need these value based expressions!
               BUILD_CHILD_TERMINAL_MACRO ( X_BoolValExp,             "BoolValExp",
                                           "X_BoolValExp",            "X_BOOL_VAL" );
               BUILD_CHILD_TERMINAL_MACRO ( X_StringVal,              "StringVal",
                                           "X_StringVal",             "X_STRING_VAL" );
               BUILD_CHILD_TERMINAL_MACRO ( X_ShortVal,               "ShortVal",
                                           "X_ShortVal",              "X_SHORT_VAL" );
               BUILD_CHILD_TERMINAL_MACRO ( X_CharVal,                "CharVal",
                                           "X_CharVal",               "X_CHAR_VAL" );
               BUILD_CHILD_TERMINAL_MACRO ( X_UnsignedCharVal,        "UnsignedCharVal",
                                           "X_UnsignedCharVal",       "X_UNSIGNED_CHAR_VAL" );
               BUILD_CHILD_TERMINAL_MACRO ( X_WcharVal,               "WcharVal",
                                           "X_WcharVal",              "X_WCHAR_VAL" );
               BUILD_CHILD_TERMINAL_MACRO ( X_UnsignedShortVal,       "UnsignedShortVal",
                                           "X_UnsignedShortVal",      "X_UNSIGNED_SHORT_VAL" );
               BUILD_CHILD_TERMINAL_MACRO ( X_IntVal,                 "IntVal",
                                           "X_IntVal",                "X_INT_VAL" );
               BUILD_CHILD_TERMINAL_MACRO ( X_EnumVal,                "EnumVal",
                                           "X_EnumVal",               "X_ENUM_VAL" );
               BUILD_CHILD_TERMINAL_MACRO ( X_UnsignedIntVal,         "UnsignedIntVal",
                                           "X_UnsignedIntVal",        "X_UNSIGNED_INT_VAL" );
               BUILD_CHILD_TERMINAL_MACRO ( X_LongIntVal,             "LongIntVal",
                                           "X_LongIntVal",            "X_LONG_INT_VAL" );
               BUILD_CHILD_TERMINAL_MACRO ( X_LongLongIntVal,         "LongLongIntVal",
                                           "X_LongLongIntVal",        "X_LONG_LONG_INT_VAL" );
               BUILD_CHILD_TERMINAL_MACRO ( X_UnsignedLongLongIntVal, "UnsignedLongLongIntVal",
                                           "X_UnsignedLongLongIntVal","X_UNSIGNED_LONG_LONG_INT_VAL" );
               BUILD_CHILD_TERMINAL_MACRO ( X_UnsignedLongVal,        "UnsignedLongVal",
                                           "X_UnsignedLongVal",       "X_UNSIGNED_LONG_INT_VAL" );
               BUILD_CHILD_TERMINAL_MACRO ( X_FloatVal,               "FloatVal",
                                            "X_FloatVal",             "X_FLOAT_VAL" );
               BUILD_CHILD_TERMINAL_MACRO ( X_DoubleVal,              "DoubleVal",
                                           "X_DoubleVal",             "X_DOUBLE_VAL" );
               BUILD_CHILD_TERMINAL_MACRO ( X_LongDoubleVal,          "LongDoubleVal",
                                           "X_LongDoubleVal",         "X_LONG_DOUBLE_VAL" );
#endif

            // associatedGrammar->nonTerminalList.display("Called from NonTerminal::operator|=() BEFORE X_Initializer");

               BUILD_CHILD_NONTERMINAL_MACRO (X_Initializer,
                            X_AggregateInitializer | X_ConstructorInitializer | X_AssignInitializer,
                            "Initializer","X_Initializer","X_EXPR_INIT");

            // associatedGrammar->nonTerminalList.display("Called from NT::operator|=() AFTER X_Initializer");

               ROSE_ASSERT(X_Initializer.associatedGrammar->isNonTerminal(X_Initializer.name) == TRUE);
               ROSE_ASSERT(X_Initializer.associatedGrammar->isNonTerminal("X_Initializer") == TRUE);
            // X_Initializer.display("X_Initializer in operator|= ");

            // associatedGrammar->nonTerminalList.display("Called from NonTerminal::operator|=() AFTER X_Initializer");

#if 1
               BUILD_CHILD_NONTERMINAL_MACRO (X_UnaryOp,
                    X_ExpressionRoot | X_MinusOp      | X_UnaryAddOp | X_NotOp           | X_PointerDerefExp | 
                    X_AddressOfOp    | X_MinusMinusOp | X_PlusPlusOp | X_BitComplementOp | X_CastExp | X_ThrowOp,
                           "UnaryOp","X_UnaryOp","X_UNARY_EXPRESSION");

               BUILD_CHILD_NONTERMINAL_MACRO (X_BinaryOp,
                    X_ArrowExp       | X_DotExp        | X_DotStarOp    | X_ArrowStarOp   | X_EqualityOp       | 
                    X_LessThanOp     | X_GreaterThanOp | X_NotEqualOp   | X_LessOrEqualOp | X_GreaterOrEqualOp |
                    X_AddOp          | X_SubtractOp    | X_MultiplyOp   | X_DivideOp      | X_IntegerDivideOp  |
                    X_ModOp          | X_AndOp         | X_OrOp         | X_BitXorOp      | X_BitAndOp         |
                    X_BitOrOp        | X_CommaOpExp    | X_LshiftOp     | X_RshiftOp      | X_PntrArrRefExp    |
                    X_ScopeOp        | X_AssignOp      | X_PlusAssignOp | X_MinusAssignOp | X_AndAssignOp      |
                    X_IorAssignOp    | X_MultAssignOp  | X_DivAssignOp  | X_ModAssignOp   | X_XorAssignOp      |
                    X_LshiftAssignOp | X_RshiftAssignOp,
                   "BinaryOp","X_BinaryOp","X_BINARY_EXPRESSION");

               BUILD_CHILD_NONTERMINAL_MACRO (X_ValueExp,
                    X_BoolValExp             | X_StringVal       | X_ShortVal         | X_CharVal        | 
                    X_UnsignedCharVal        | X_WcharVal        | X_UnsignedShortVal | X_IntVal         |
                    X_EnumVal                | X_UnsignedIntVal  | X_LongIntVal       | X_LongLongIntVal |
                    X_UnsignedLongLongIntVal | X_UnsignedLongVal | X_FloatVal         | X_DoubleVal      |
                    X_LongDoubleVal,
                   "ValueExp","X_ValueExp","X_ValueExpTag");

#if 0
            // This is already a part of the default grammar's definition
               BUILD_CHILD_NONTERMINAL_MACRO (X_Expression,
                    X_UnaryOp         | X_BinaryOp        | X_ExprListExp          | X_VarRefExp       | 
                    X_ClassNameRefExp | X_FunctionRefExp  | X_MemberFunctionRefExp | X_ValueExp        |
                    X_FunctionCallExp | X_SizeOfOp        | X_TypeIdOp             | X_ConditionalExp  |
                    X_NewExp          | X_DeleteExp       | X_ThisExp              | X_RefExp          |
                    X_Initializer,
                   "Expression","X_Expression","X_ExpressionTag");
#endif
#endif

               ROSE_ASSERT(X_Initializer.associatedGrammar->isNonTerminal(X_Initializer.name) == TRUE);
               ROSE_ASSERT(X_Initializer.associatedGrammar->isNonTerminal("X_Initializer") == TRUE);

               NonTerminal & local_X_Initializer = getNonTerminal("X_Initializer");

#if 1
               local_X_Initializer.addElement(local_X_Initializer.orTokenPointerArray, X_AggregateInitializer);
               local_X_Initializer.addElement(local_X_Initializer.orTokenPointerArray, X_ConstructorInitializer);
               local_X_Initializer.addElement(local_X_Initializer.orTokenPointerArray, X_AssignInitializer);
#endif

#if 0
            // DQ (10/13/2007): Commented out this output!
               printf ("local_X_Initializer ---> "); local_X_Initializer.show(); printf ("\n\n");
#endif
               NonTerminal & local_X_UnaryOp = getNonTerminal("X_UnaryOp");

#if 1
               local_X_UnaryOp.addElement(local_X_UnaryOp.orTokenPointerArray, X_ExpressionRoot);
               local_X_UnaryOp.addElement(local_X_UnaryOp.orTokenPointerArray, X_MinusOp);
               local_X_UnaryOp.addElement(local_X_UnaryOp.orTokenPointerArray, X_UnaryAddOp);
               local_X_UnaryOp.addElement(local_X_UnaryOp.orTokenPointerArray, X_NotOp);
               local_X_UnaryOp.addElement(local_X_UnaryOp.orTokenPointerArray, X_PointerDerefExp);
               local_X_UnaryOp.addElement(local_X_UnaryOp.orTokenPointerArray, X_AddressOfOp);
               local_X_UnaryOp.addElement(local_X_UnaryOp.orTokenPointerArray, X_MinusMinusOp);
               local_X_UnaryOp.addElement(local_X_UnaryOp.orTokenPointerArray, X_PlusPlusOp);
               local_X_UnaryOp.addElement(local_X_UnaryOp.orTokenPointerArray, X_BitComplementOp);
               local_X_UnaryOp.addElement(local_X_UnaryOp.orTokenPointerArray, X_CastExp);
               local_X_UnaryOp.addElement(local_X_UnaryOp.orTokenPointerArray, X_ThrowOp);
#endif
#if 0
            // DQ (10/13/2007): Commented out this output!
               printf ("local_X_UnaryOp ---> "); local_X_UnaryOp.show(); printf ("\n\n");
#endif
               NonTerminal & local_X_BinaryOp = getNonTerminal("X_BinaryOp");

#if 1
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_ArrowExp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_DotExp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_DotStarOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_ArrowStarOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_EqualityOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_LessThanOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_GreaterThanOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_NotEqualOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_LessOrEqualOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_GreaterOrEqualOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_AddOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_SubtractOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_MultiplyOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_DivideOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_IntegerDivideOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_ModOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_AndOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_OrOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_BitXorOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_BitAndOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_BitOrOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_CommaOpExp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_LshiftOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_RshiftOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_PntrArrRefExp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_ScopeOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_AssignOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_PlusAssignOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_MinusAssignOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_AndAssignOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_IorAssignOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_MultAssignOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_DivAssignOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_ModAssignOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_XorAssignOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_LshiftAssignOp);
               local_X_BinaryOp.addElement(local_X_BinaryOp.orTokenPointerArray, X_RshiftAssignOp);
#endif

#if 0
            // DQ (10/13/2007): Commented out this output!
               printf ("local_X_BinaryOp ---> "); local_X_BinaryOp.show(); printf ("\n\n");
#endif
               NonTerminal & local_X_ValueExp = getNonTerminal("X_ValueExp");

#if 1
               local_X_ValueExp.addElement(local_X_ValueExp.orTokenPointerArray, X_BoolValExp);
               local_X_ValueExp.addElement(local_X_ValueExp.orTokenPointerArray, X_StringVal);
               local_X_ValueExp.addElement(local_X_ValueExp.orTokenPointerArray, X_ShortVal);
               local_X_ValueExp.addElement(local_X_ValueExp.orTokenPointerArray, X_CharVal);
               local_X_ValueExp.addElement(local_X_ValueExp.orTokenPointerArray, X_UnsignedCharVal);
               local_X_ValueExp.addElement(local_X_ValueExp.orTokenPointerArray, X_WcharVal);
               local_X_ValueExp.addElement(local_X_ValueExp.orTokenPointerArray, X_UnsignedShortVal);
               local_X_ValueExp.addElement(local_X_ValueExp.orTokenPointerArray, X_IntVal);
               local_X_ValueExp.addElement(local_X_ValueExp.orTokenPointerArray, X_EnumVal);
               local_X_ValueExp.addElement(local_X_ValueExp.orTokenPointerArray, X_UnsignedIntVal);
               local_X_ValueExp.addElement(local_X_ValueExp.orTokenPointerArray, X_LongIntVal);
               local_X_ValueExp.addElement(local_X_ValueExp.orTokenPointerArray, X_LongLongIntVal);
               local_X_ValueExp.addElement(local_X_ValueExp.orTokenPointerArray, X_UnsignedLongLongIntVal);
               local_X_ValueExp.addElement(local_X_ValueExp.orTokenPointerArray, X_UnsignedLongVal);
               local_X_ValueExp.addElement(local_X_ValueExp.orTokenPointerArray, X_FloatVal);
               local_X_ValueExp.addElement(local_X_ValueExp.orTokenPointerArray, X_DoubleVal);
               local_X_ValueExp.addElement(local_X_ValueExp.orTokenPointerArray, X_LongDoubleVal);
#endif

#if 0
            // DQ (10/13/2007): Commented out this output!
               printf ("local_X_ValueExp ---> "); local_X_ValueExp.show(); printf ("\n\n");
#endif
               NonTerminal & local_X_Expression = getNonTerminal("X_Expression");
#if 1
               local_X_Expression.addElement(local_X_Expression.orTokenPointerArray, X_ExprListExp);
               local_X_Expression.addElement(local_X_Expression.orTokenPointerArray, X_VarRefExp);
               local_X_Expression.addElement(local_X_Expression.orTokenPointerArray, X_ClassNameRefExp);
               local_X_Expression.addElement(local_X_Expression.orTokenPointerArray, X_FunctionRefExp);
               local_X_Expression.addElement(local_X_Expression.orTokenPointerArray, X_MemberFunctionRefExp);
               local_X_Expression.addElement(local_X_Expression.orTokenPointerArray, X_FunctionCallExp);
               local_X_Expression.addElement(local_X_Expression.orTokenPointerArray, X_SizeOfOp);
               local_X_Expression.addElement(local_X_Expression.orTokenPointerArray, X_TypeIdOp);
               local_X_Expression.addElement(local_X_Expression.orTokenPointerArray, X_ConditionalExp);
               local_X_Expression.addElement(local_X_Expression.orTokenPointerArray, X_NewExp);
               local_X_Expression.addElement(local_X_Expression.orTokenPointerArray, X_DeleteExp);
               local_X_Expression.addElement(local_X_Expression.orTokenPointerArray, X_ThisExp);
               local_X_Expression.addElement(local_X_Expression.orTokenPointerArray, X_RefExp);

            // Add these to the nonTerminal lists
               local_X_Expression.addElement(local_X_Expression.orNonTerminalPointerArray, X_UnaryOp);
               local_X_Expression.addElement(local_X_Expression.orNonTerminalPointerArray, X_BinaryOp);
               local_X_Expression.addElement(local_X_Expression.orNonTerminalPointerArray, X_ValueExp);
               local_X_Expression.addElement(local_X_Expression.orNonTerminalPointerArray, X_Initializer);
#endif
#if 0
            // DQ (10/13/2007): Commented out this output!
               printf ("local_X_Expression ---> "); local_X_Expression.show(); printf ("\n\n");
#endif
            // nonTerminalList.display("Called from NonTerminal::operator|=() AFTER ADD_ELEMENT");
             }

          if ( X.isExpression() == TRUE )
             {
               printf ("Fixup expression system with new expression terminal! \n");

#if 1
               printf ("ERROR: Case of new expression terminal not implemented yet! \n");
               ROSE_ABORT();
#else

            // add to the list of terminals
               orTokenPointerArray.addElement (X);

            // Now add any additional supporting expression terminals (are there more we should add?)
               BUILD_CHILD_TERMINAL_MACRO ( X_ExprListExp,            "ExprListExp",
                                           "X_ExprListExp",           "X_EXPR_LIST" );

#endif
             }
        }

  // Handle the statement issues represented by the new terminal
     if (X.isStatement() || X.isExpression() || X.isType())
        {
       // printf ("Fixup statement system defined in grammar! \n");

       // printf ("ERROR: Case of new statement terminal not implemented yet! \n");
       // ROSE_ABORT();

       // If Y is an expression or type then all the subtree of the grammar represented by non_X_Statement is
       // copied and replicated for the X_Statement (since all possible statements could be built using the
       // new expressions (and types if Y is a type).
          if (X.isExpression() || X.isType())
             {
               printf ("Fixup statement system with new expression or type terminal! \n");

               BUILD_CHILD_TERMINAL_MACRO ( X_ParBlockStmt,              "ParBlockStmt",
                                           "X_ParBlockStmt",             "X_PAR_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_ParForStmt,                "ParForStmt",
                                           "X_ParForStmt",               "X_PARFOR_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_Global,                    "Global",
                                           "X_Global",                   "X_GLOBAL_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_IfStmt,                    "IfStmt",
                                           "X_IfStmt",                   "X_IF_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_FunctionDefinition,        "FunctionDefinition",
                                           "X_FunctionDefinition",       "X_FUNC_DEFN_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_ClassDefinition,           "ClassDefinition",
                                           "X_ClassDefinition",          "X_CLASS_DEFN_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_WhileStmt,                 "WhileStmt",
                                           "X_WhileStmt",                "X_WHILE_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_DoWhileStmt,               "DoWhileStmt",
                                           "X_DoWhileStmt",              "X_DO_WHILE_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_SwitchStatement,           "SwitchStatement",
                                           "X_SwitchStatement",          "X_SWITCH_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_CatchOptionStmt,           "CatchOptionStmt",
                                           "X_CatchOptionStmt",          "X_CATCH_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_MemberFunctionDeclaration, "MemberFunctionDeclaration",
                                           "X_MemberFunctionDeclaration","X_MFUNC_DECL_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_VariableDeclaration,       "VariableDeclaration",
                                           "X_VariableDeclaration",      "X_VAR_DECL_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_VariableDefinition,        "VariableDefinition",
                                           "X_VariableDefinition",       "X_VAR_DEFN_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_ClassDeclaration,          "ClassDeclaration",
                                           "X_ClassDeclaration",         "X_CLASS_DECL_STMT" );

            // I don't think we need this (we don't build an X_EnumType for 
            // example which would be required to support it)
            // BUILD_CHILD_TERMINAL_MACRO ( X_EnumDeclaration,           "EnumDeclaration",
            //                             "X_EnumDeclaration",          "X_ENUM_DECL_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_AsmStmt,                   "AsmStmt",
                                           "X_AsmStmt",                  "X_ASM_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_TypedefDeclaration,        "TypedefDeclaration",
                                           "X_TypedefDeclaration",       "X_TYPEDEF_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_TemplateDeclaration,       "TemplateDeclaration",
                                           "X_TemplateDeclaration",      "X_TEMPLATE_DECL_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_FunctionTypeTable,         "FunctionTypeTable",
                                           "X_FunctionTypeTable",        "X_FUNC_TBL_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_ExprStatement,             "ExprStatement",
                                           "X_ExprStatement",            "X_EXPR_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_LabelStatement,            "LabelStatement",
                                           "X_LabelStatement",           "X_LABEL_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_CaseOptionStmt,            "CaseOptionStmt",
                                           "X_CaseOptionStmt",           "X_CASE_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_TryStmt,                   "TryStmt",
                                           "X_TryStmt",                  "X_TRY_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_DefaultOptionStmt,         "DefaultOptionStmt",
                                           "X_DefaultOptionStmt",        "X_DEFAULT_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_BreakStmt,                 "BreakStmt",
                                           "X_BreakStmt",                "X_BREAK_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_ContinueStmt,              "ContinueStmt",
                                           "X_ContinueStmt",             "X_CONTINUE_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_ReturnStmt,                "ReturnStmt",
                                           "X_ReturnStmt",               "X_RETURN_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_GotoStatement,             "GotoStatement",
                                           "X_GotoStatement",            "X_GOTO_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_SpawnStmt,                 "SpawnStmt",
                                           "X_SpawnStmt",                "X_SPAWN_STMT" );
               BUILD_CHILD_TERMINAL_MACRO ( X_PragmaStatement,           "PragmaStatement",
                                           "X_PragmaStatement",          "X_PRAGMA_STMT" );

            // Addition to SAGE3 (not present in SAGE2) this permits the representation of unbound pragmas in SAGE3
            // BUILD_CHILD_TERMINAL_MACRO ( X_PragmaStatement, "PragmaStatement", "PRAGMA_STMT" );

               BUILD_CHILD_NONTERMINAL_MACRO (X_ForStatement,X_ParForStmt,"ForStatement","X_ForStatement","X_FOR_STMT");

               BUILD_CHILD_NONTERMINAL_MACRO (X_BasicBlock,X_ParBlockStmt,"BasicBlock","X_BasicBlock","X_BASIC_BLOCK_STMT");

               BUILD_CHILD_NONTERMINAL_MACRO (X_ScopeStatement,
                    X_Global          | X_BasicBlock | X_IfStmt      | X_ForStatement    | X_FunctionDefinition |
                    X_ClassDefinition | X_WhileStmt  | X_DoWhileStmt | X_SwitchStatement | X_CatchOptionStmt,
                    "ScopeStatement","X_ScopeStatement","X_SCOPE_STMT");

               BUILD_CHILD_NONTERMINAL_MACRO (X_FunctionDeclaration,X_MemberFunctionDeclaration,
                    "FunctionDeclaration","X_FunctionDeclaration","X_FUNC_DECL_STMT");

            // I don't think we need this (we don't build an X_EnumType for 
            // example which would be required to support it)
#if 0
               BUILD_CHILD_NONTERMINAL_MACRO (X_DeclarationStatement,
                    X_FunctionDeclaration | X_VariableDeclaration | X_VariableDefinition | X_ClassDeclaration |
                    X_EnumDeclaration     | X_AsmStmt             | X_TypedefDeclaration | X_TemplateDeclaration,
                   "DeclarationStatement","X_DeclarationStatement","X_DECL_STMT");
#else
               BUILD_CHILD_NONTERMINAL_MACRO (X_DeclarationStatement,
                    X_FunctionDeclaration | X_VariableDeclaration | X_VariableDefinition | X_ClassDeclaration |
                    X_AsmStmt             | X_TypedefDeclaration | X_TemplateDeclaration,
                   "DeclarationStatement","X_DeclarationStatement","X_DECL_STMT");
#endif

#if 0
            // This is already a part of the default grammar's definition
               BUILD_CHILD_NONTERMINAL_MACRO (X_Statement,
                    X_ScopeStatement | X_FunctionTypeTable | X_DeclarationStatement | X_ExprStatement     |
                    X_LabelStatement | X_CaseOptionStmt    | X_TryStmt              | X_DefaultOptionStmt |
                    X_BreakStmt      | X_ContinueStmt      | X_ReturnStmt           | X_GotoStatement     |
                    X_SpawnStmt      | X_PragmaStatement,
                    "Statement","X_Statement","X_StatementTag");
#endif

               NonTerminal & local_X_ForStatement = getNonTerminal("X_ForStatement");

#if 0
               local_X_ForStatement.orTokenPointerArray.addElement(X_ParForStmt);
#endif

#if 0
            // DQ (10/13/2007): Commented out this output!
               printf ("local_X_ForStatement ---> "); local_X_ForStatement.show(); printf ("\n\n");
#endif
               NonTerminal & local_X_BasicBlock = getNonTerminal("X_BasicBlock");

#if 0
               local_X_BasicBlock.orTokenPointerArray.addElement(X_ParBlockStmt);
#endif

#if 0
            // DQ (10/13/2007): Commented out this output!
               printf ("local_X_BasicBlock ---> "); local_X_BasicBlock.show(); printf ("\n\n");
#endif
               NonTerminal & local_X_ScopeStatement = getNonTerminal("X_ScopeStatement");

#if 0
               local_X_ScopeStatement.orTokenPointerArray.addElement(X_Global);
               local_X_ScopeStatement.orTokenPointerArray.addElement(X_BasicBlock);
               local_X_ScopeStatement.orTokenPointerArray.addElement(X_IfStmt);
               local_X_ScopeStatement.orTokenPointerArray.addElement(X_ForStatement);
               local_X_ScopeStatement.orTokenPointerArray.addElement(X_FunctionDefinition);
               local_X_ScopeStatement.orTokenPointerArray.addElement(X_ClassDefinition);
               local_X_ScopeStatement.orTokenPointerArray.addElement(X_WhileStmt);
               local_X_ScopeStatement.orTokenPointerArray.addElement(X_DoWhileStmt);
               local_X_ScopeStatement.orTokenPointerArray.addElement(X_SwitchStatement);
               local_X_ScopeStatement.orTokenPointerArray.addElement(X_CatchOptionStmt);
#endif

#if 0
            // DQ (10/13/2007): Commented out this output!
               printf ("local_X_ScopeStatement ---> "); local_X_ScopeStatement.show(); printf ("\n\n");
#endif
               NonTerminal & local_X_FunctionDeclaration = getNonTerminal("X_FunctionDeclaration");

#if 0
               local_X_FunctionDeclaration.orTokenPointerArray.addElement(X_MemberFunctionDeclaration);
#endif

#if 0
            // DQ (10/13/2007): Commented out this output!
               printf ("local_X_FunctionDeclaration ---> "); local_X_FunctionDeclaration.show(); printf ("\n\n");
#endif
               NonTerminal & local_X_DeclarationStatement = getNonTerminal("X_DeclarationStatement");

#if 0
               local_X_DeclarationStatement.orTokenPointerArray.addElement(X_FunctionDeclaration);
               local_X_DeclarationStatement.orTokenPointerArray.addElement(X_VariableDeclaration);
               local_X_DeclarationStatement.orTokenPointerArray.addElement(X_VariableDefinition);
               local_X_DeclarationStatement.orTokenPointerArray.addElement(X_ClassDeclaration);

            // I don't think we need this (we don't build an X_EnumType for 
            // example which would be required to support it)
            // local_X_DeclarationStatement.orTokenPointerArray.addElement(X_EnumDeclaration);

               local_X_DeclarationStatement.orTokenPointerArray.addElement(X_AsmStmt);
               local_X_DeclarationStatement.orTokenPointerArray.addElement(X_TypedefDeclaration);
               local_X_DeclarationStatement.orTokenPointerArray.addElement(X_TemplateDeclaration);
#endif

#if 0
            // DQ (10/13/2007): Commented out this output!
               printf ("local_X_DeclarationStatement ---> "); local_X_DeclarationStatement.show(); printf ("\n\n");
#endif
               NonTerminal & local_X_Statement = getNonTerminal("X_Statement");

#if 1
               local_X_Statement.addElement(local_X_Statement.orTokenPointerArray, X_FunctionTypeTable);
               local_X_Statement.addElement(local_X_Statement.orTokenPointerArray, X_ExprStatement);
               local_X_Statement.addElement(local_X_Statement.orTokenPointerArray, X_LabelStatement);
               local_X_Statement.addElement(local_X_Statement.orTokenPointerArray, X_CaseOptionStmt);
               local_X_Statement.addElement(local_X_Statement.orTokenPointerArray, X_TryStmt);
               local_X_Statement.addElement(local_X_Statement.orTokenPointerArray, X_DefaultOptionStmt);
               local_X_Statement.addElement(local_X_Statement.orTokenPointerArray, X_BreakStmt);
               local_X_Statement.addElement(local_X_Statement.orTokenPointerArray, X_ContinueStmt);
               local_X_Statement.addElement(local_X_Statement.orTokenPointerArray, X_ReturnStmt);
               local_X_Statement.addElement(local_X_Statement.orTokenPointerArray, X_GotoStatement);
               local_X_Statement.addElement(local_X_Statement.orTokenPointerArray, X_SpawnStmt);
               local_X_Statement.addElement(local_X_Statement.orTokenPointerArray, X_PragmaStatement);

            // Add these to the nonTerminal lists
               local_X_Statement.addElement(local_X_Statement.orNonTerminalPointerArray, X_ScopeStatement);
               local_X_Statement.addElement(local_X_Statement.orNonTerminalPointerArray, X_DeclarationStatement);
#endif

#if 0
            // DQ (10/13/2007): Commented out this output!
               printf ("local_X_Statement ---> "); local_X_Statement.show(); printf ("\n\n");
#endif
             }

       // If Y is a statement then the new terminal is added to the X_Statement nonterminal
          if (X.isStatement())
             {
               printf ("Fixup statement system with new statement terminal! \n");

               printf ("ERROR: Case of new statement terminal not implemented yet! \n");
               ROSE_ABORT();
             }
        }
       else
        {
       // Not sure what this could be since we don't worry about symbols (yet), so let's call this an error!
	  printf ("ERROR: in NonTerminal::operator |= ( const Terminal & Y ): not a type, expression, or a statement \n");
	  ROSE_ABORT();
        }

#if 0
     printf ("Need to address requirements of the branch hierarchy!!! \n");
     ROSE_ABORT();
#endif

  // nonTerminalList.display("Called from NonTerminal::operator|=() BASE of NonTerminal::operator=|");
   }



void
Grammar::removeNewTerminal (Terminal & X )
   {
  // This mechanism removes new child terminals added as part of the
  // addition of a new type, expression or statement.  This is the
  // principle mechanism for restriction of the grammars and compliments
  // the addNewTerminal member function for the expansion of grammars.

  // We need to undo the effect of:
  // BUILD_CHILD_NONTERMINAL_MACRO (X_PointerType, X_PointerMemberType,
  //             "PointerType","X_PointerType","X_T_POINTER")

  // We can only remove child terminals since we don't effect the
  // existing grammar (C++) through any modifications to the grammar
     ROSE_ASSERT(X.isChild() == TRUE);

#if 0
     NonTerminal & local_X_Parent = getNonTerminal(X.getParent().name());

     local_X_Parent.orTokenPointerArray.removeElement(X);
#endif

   }






