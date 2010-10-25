
#include "grammar.h"
#include "ROSETTA_macros.h"
#include "terminal.h"

// What should be the behavior of the default constructor for Grammar

void
Grammar::setUpExpressions ()
   {
  // This function sets up the type system for the grammar.  In this case it implements the
  // C++ grammar, but this will be modified to permit all grammars to contain elements of the
  // C++ grammar.  Modified grammars will add and subtract elements from this default C++ grammar.

     NEW_TERMINAL_MACRO (ExprListExp,            "ExprListExp",            "EXPR_LIST" );
     NEW_TERMINAL_MACRO (VarRefExp,              "VarRefExp",              "VAR_REF" );

  // DQ (12/30/2007): New IR node to support references to labels (much like references to variables 
  // for the VarRefExp, but with a internal pointer ot a SgLabelSymbol) This IR nodes will eventually 
  // be used in the C/C++ goto, but is introduced to hnadle references to labels in Fortran (e.g. format 
  // expression in "read" and "write" statements, and a cleanr implementation of references to labels in 
  // Fortran loop constructs etc.).
     NEW_TERMINAL_MACRO (LabelRefExp,            "LabelRefExp",            "LABEL_REF" );

     NEW_TERMINAL_MACRO (ClassNameRefExp,        "ClassNameRefExp",        "CLASSNAME_REF" );
     NEW_TERMINAL_MACRO (FunctionRefExp,         "FunctionRefExp",         "FUNCTION_REF" );
     NEW_TERMINAL_MACRO (MemberFunctionRefExp,   "MemberFunctionRefExp",   "MEMBER_FUNCTION_REF" );
     NEW_TERMINAL_MACRO (FunctionCallExp,        "FunctionCallExp",        "FUNC_CALL" );
     NEW_TERMINAL_MACRO (SizeOfOp,               "SizeOfOp",               "SIZEOF_OP" );

#if USE_UPC_IR_NODES
  // DQ and Liao (6/10/2008): Added new IR nodes specific to UPC.
     NEW_TERMINAL_MACRO (UpcLocalsizeofExpression,    "UpcLocalsizeofExpression",    "UPC_LOCAL_SIZEOF_EXPR" );
     NEW_TERMINAL_MACRO (UpcBlocksizeofExpression,    "UpcBlocksizeofExpression",    "UPC_BLOCK_SIZEOF_EXPR" );
     NEW_TERMINAL_MACRO (UpcElemsizeofExpression,     "UpcElemsizeofExpression",     "UPC_ELEM_SIZEOF_EXPR" );
#endif

  // DQ (2/5/2004): EDG 3.3 now separates out vararg functions explicitly in the AST 
  // (something I always wanted to see done), so we will do the same in SAGE.
  // This provides for the best possible vararg handling!
     NEW_TERMINAL_MACRO (VarArgStartOp,          "VarArgStartOp",          "VA_START_OP" );
     NEW_TERMINAL_MACRO (VarArgStartOneOperandOp,"VarArgStartOneOperandOp","VA_START_ONE_OPERAND_OP" );
     NEW_TERMINAL_MACRO (VarArgOp,               "VarArgOp",               "VA_OP" );
     NEW_TERMINAL_MACRO (VarArgEndOp,            "VarArgEndOp",            "VA_END_OP" );
     NEW_TERMINAL_MACRO (VarArgCopyOp,           "VarArgCopyOp",           "VA_COPY_OP" );

     NEW_TERMINAL_MACRO (TypeIdOp,               "TypeIdOp",               "TYPEID_OP" );
     NEW_TERMINAL_MACRO (ConditionalExp,         "ConditionalExp",         "EXPR_CONDITIONAL" );
     NEW_TERMINAL_MACRO (NewExp,                 "NewExp",                 "NEW_OP" );
     NEW_TERMINAL_MACRO (DeleteExp,              "DeleteExp",              "DELETE_OP" );
     NEW_TERMINAL_MACRO (ThisExp,                "ThisExp",                "THIS_NODE" );
     NEW_TERMINAL_MACRO (RefExp,                 "RefExp",                 "TYPE_REF" );
     NEW_TERMINAL_MACRO (AggregateInitializer,   "AggregateInitializer",   "AGGREGATE_INIT" );
     NEW_TERMINAL_MACRO (ConstructorInitializer, "ConstructorInitializer", "CONSTRUCTOR_INIT" );
     NEW_TERMINAL_MACRO (AssignInitializer,      "AssignInitializer",      "ASSIGN_INIT" );
     NEW_TERMINAL_MACRO (ExpressionRoot,         "ExpressionRoot",         "EXPRESSION_ROOT" );
     NEW_TERMINAL_MACRO (MinusOp,                "MinusOp",                "UNARY_MINUS_OP" );
     NEW_TERMINAL_MACRO (UnaryAddOp,             "UnaryAddOp",             "UNARY_ADD_OP" );
     NEW_TERMINAL_MACRO (NotOp,                  "NotOp",                  "NOT_OP" );
     NEW_TERMINAL_MACRO (PointerDerefExp,        "PointerDerefExp",        "DEREF_OP" );
     NEW_TERMINAL_MACRO (AddressOfOp,            "AddressOfOp",            "ADDRESS_OP" );
     NEW_TERMINAL_MACRO (MinusMinusOp,           "MinusMinusOp",           "MINUSMINUS_OP" );
     NEW_TERMINAL_MACRO (PlusPlusOp,             "PlusPlusOp",             "PLUSPLUS_OP" );
     NEW_TERMINAL_MACRO (BitComplementOp,        "BitComplementOp",        "BIT_COMPLEMENT_OP" );
     NEW_TERMINAL_MACRO (RealPartOp,             "RealPartOp",             "REAL_PART_OP" );
     NEW_TERMINAL_MACRO (ImagPartOp,             "ImagPartOp",             "IMAG_PART_OP" );
     NEW_TERMINAL_MACRO (ConjugateOp,            "ConjugateOp",            "CONJUGATE_OP" );
     NEW_TERMINAL_MACRO (CastExp,                "CastExp",                "CAST_OP" );
     NEW_TERMINAL_MACRO (ThrowOp,                "ThrowOp",                "THROW_OP" );
     NEW_TERMINAL_MACRO (ArrowExp,               "ArrowExp",               "POINTST_OP" );
     NEW_TERMINAL_MACRO (DotExp,                 "DotExp",                 "RECORD_REF" );
     NEW_TERMINAL_MACRO (DotStarOp,              "DotStarOp",              "DOTSTAR_OP" );
     NEW_TERMINAL_MACRO (ArrowStarOp,            "ArrowStarOp",            "ARROWSTAR_OP" );
     NEW_TERMINAL_MACRO (EqualityOp,             "EqualityOp",             "EQ_OP" );
     NEW_TERMINAL_MACRO (LessThanOp,             "LessThanOp",             "LT_OP" );
     NEW_TERMINAL_MACRO (GreaterThanOp,          "GreaterThanOp",          "GT_OP" );
     NEW_TERMINAL_MACRO (NotEqualOp,             "NotEqualOp",             "NE_OP" );
     NEW_TERMINAL_MACRO (LessOrEqualOp,          "LessOrEqualOp",          "LE_OP" );
     NEW_TERMINAL_MACRO (GreaterOrEqualOp,       "GreaterOrEqualOp",       "GE_OP" );
     NEW_TERMINAL_MACRO (AddOp,                  "AddOp",                  "ADD_OP" );
     NEW_TERMINAL_MACRO (SubtractOp,             "SubtractOp",             "SUBT_OP" );
     NEW_TERMINAL_MACRO (MultiplyOp,             "MultiplyOp",             "MULT_OP" );
     NEW_TERMINAL_MACRO (DivideOp,               "DivideOp",               "DIV_OP" );
     NEW_TERMINAL_MACRO (IntegerDivideOp,        "IntegerDivideOp",        "INTEGER_DIV_OP" );
     NEW_TERMINAL_MACRO (ModOp,                  "ModOp",                  "MOD_OP" );
     NEW_TERMINAL_MACRO (AndOp,                  "AndOp",                  "AND_OP" );
     NEW_TERMINAL_MACRO (OrOp,                   "OrOp",                   "OR_OP" );
     NEW_TERMINAL_MACRO (BitXorOp,               "BitXorOp",               "BITXOR_OP" );
     NEW_TERMINAL_MACRO (BitAndOp,               "BitAndOp",               "BITAND_OP" );
     NEW_TERMINAL_MACRO (BitOrOp,                "BitOrOp",                "BITOR_OP" );
     NEW_TERMINAL_MACRO (CommaOpExp,             "CommaOpExp",             "COMMA_OP" );
     NEW_TERMINAL_MACRO (LshiftOp,               "LshiftOp",               "LSHIFT_OP" );
     NEW_TERMINAL_MACRO (RshiftOp,               "RshiftOp",               "RSHIFT_OP" );
     NEW_TERMINAL_MACRO (PntrArrRefExp,          "PntrArrRefExp",          "ARRAY_OP" );
     NEW_TERMINAL_MACRO (ScopeOp,                "ScopeOp",                "SCOPE_OP" );
     NEW_TERMINAL_MACRO (AssignOp,               "AssignOp",               "ASSIGN_OP" );
     NEW_TERMINAL_MACRO (PlusAssignOp,           "PlusAssignOp",           "PLUS_ASSIGN_OP" );
     NEW_TERMINAL_MACRO (MinusAssignOp,          "MinusAssignOp",          "MINUS_ASSIGN_OP" );
     NEW_TERMINAL_MACRO (AndAssignOp,            "AndAssignOp",            "AND_ASSIGN_OP" );
     NEW_TERMINAL_MACRO (IorAssignOp,            "IorAssignOp",            "IOR_ASSIGN_OP" );
     NEW_TERMINAL_MACRO (MultAssignOp,           "MultAssignOp",           "MULT_ASSIGN_OP" );
     NEW_TERMINAL_MACRO (DivAssignOp,            "DivAssignOp",            "DIV_ASSIGN_OP" );
     NEW_TERMINAL_MACRO (ModAssignOp,            "ModAssignOp",            "MOD_ASSIGN_OP" );
     NEW_TERMINAL_MACRO (XorAssignOp,            "XorAssignOp",            "XOR_ASSIGN_OP" );
     NEW_TERMINAL_MACRO (LshiftAssignOp,         "LshiftAssignOp",         "LSHIFT_ASSIGN_OP" );
     NEW_TERMINAL_MACRO (RshiftAssignOp,         "RshiftAssignOp",         "RSHIFT_ASSIGN_OP" );

  // DQ (12/13/2007): Added support for Fortran string concatenation operator
     NEW_TERMINAL_MACRO (ConcatenationOp,        "ConcatenationOp",        "CONCATENATION_OP" );

     NEW_TERMINAL_MACRO (BoolValExp,             "BoolValExp",             "BOOL_VAL" );
     NEW_TERMINAL_MACRO (StringVal,              "StringVal",              "STRING_VAL" );
     NEW_TERMINAL_MACRO (ShortVal,               "ShortVal",               "SHORT_VAL" );
     NEW_TERMINAL_MACRO (CharVal,                "CharVal",                "CHAR_VAL" );
     NEW_TERMINAL_MACRO (UnsignedCharVal,        "UnsignedCharVal",        "UNSIGNED_CHAR_VAL" );
     NEW_TERMINAL_MACRO (WcharVal,               "WcharVal",               "WCHAR_VAL" );
     NEW_TERMINAL_MACRO (UnsignedShortVal,       "UnsignedShortVal",       "UNSIGNED_SHORT_VAL" );
     NEW_TERMINAL_MACRO (IntVal,                 "IntVal",                 "INT_VAL" );
     NEW_TERMINAL_MACRO (EnumVal,                "EnumVal",                "ENUM_VAL" );
     NEW_TERMINAL_MACRO (UnsignedIntVal,         "UnsignedIntVal",         "UNSIGNED_INT_VAL" );
     NEW_TERMINAL_MACRO (LongIntVal,             "LongIntVal",             "LONG_INT_VAL" );
     NEW_TERMINAL_MACRO (LongLongIntVal,         "LongLongIntVal",         "LONG_LONG_INT_VAL" );
     NEW_TERMINAL_MACRO (UnsignedLongLongIntVal, "UnsignedLongLongIntVal", "UNSIGNED_LONG_LONG_INT_VAL" );
     NEW_TERMINAL_MACRO (UnsignedLongVal,        "UnsignedLongVal",        "UNSIGNED_LONG_INT_VAL" );
     NEW_TERMINAL_MACRO (FloatVal,               "FloatVal",               "FLOAT_VAL" );
     NEW_TERMINAL_MACRO (DoubleVal,              "DoubleVal",              "DOUBLE_VAL" );
     NEW_TERMINAL_MACRO (LongDoubleVal,          "LongDoubleVal",          "LONG_DOUBLE_VAL" );

  // Liao 6/18/2008: Support UPC constant THREADS, MYTHREAD
     NEW_TERMINAL_MACRO (UpcThreads,              "UpcThreads",                 "UPC_THREADS" );
     NEW_TERMINAL_MACRO (UpcMythread,             "UpcMythread",                 "UPC_MYTHREAD" );

  // DQ (8/27/2006): Added support for complex values (We will use a ComplexVal to stand for a imaginary number as well).
     NEW_TERMINAL_MACRO (ComplexVal,             "ComplexVal",             "COMPLEX_VAL" );

  // DQ (12/13/2005): Added support for empty expression (and empty statement).
     NEW_TERMINAL_MACRO (NullExpression,         "NullExpression",             "NULL_EXPR" );

  // DQ (12/13/2005): Added variant expression to support future patterns 
  // specifications (contains RegEx string specifier for SgStatement IR node).
     NEW_TERMINAL_MACRO (VariantExpression,      "VariantExpression",          "VARIANT_EXPR" );

  // DQ (7/21/2006): Added support for GNU Statement Expression extension.
     NEW_TERMINAL_MACRO (StatementExpression,    "StatementExpression",        "STMT_EXPR" );

  // DQ (7/22/2006): Support for ASM operands that are contained in an SgAsmStmt
     NEW_TERMINAL_MACRO (AsmOp,                  "AsmOp",                      "ASM_OP" );
  
  // TV (04/22/2010): CUDA support
     // sgCudaKernelExecConfig is the '<<< grid, block, shared_size, stream >>>' part of a kernel call
     NEW_TERMINAL_MACRO (CudaKernelExecConfig,     "CudaKernelExecConfig",     "EXEC_CONF" );
     // sgCudaKernelCallExp is a node for CUDA support, it catch kernel's call.
     NEW_TERMINAL_MACRO (CudaKernelCallExp,        "CudaKernelCallExp",        "KERN_CALL" );

#if USE_FORTRAN_IR_NODES
  // Intrisic function are just like other functions, but explicitly marked to be intrinsic.
  // DQ (2/2/2006): Support for Fortran IR nodes (contributed by Rice)
  // NEW_TERMINAL_MACRO (IntrinsicFn,            "IntrinsicFn",            "INTRINSICFN" );
  // NEW_TERMINAL_MACRO (SubscriptColon,         "SubscriptColon",         "SUBSCRIPT_COLON" );
  // NEW_TERMINAL_MACRO (Colon,                  "Colon",                  "COLON" );
  // NEW_TERMINAL_MACRO (SubscriptAsterisk,      "SubscriptAsterisk",      "SUBSCRIPT_ASTERISK" );

     NEW_TERMINAL_MACRO (SubscriptExpression,    "SubscriptExpression",    "SUBSCRIPT_EXPR" );

     NEW_TERMINAL_MACRO (ColonShapeExp,          "ColonShapeExp",          "TEMP_ColonShapeExp" );
     NEW_TERMINAL_MACRO (AsteriskShapeExp,       "AsteriskShapeExp",       "TEMP_AsteriskShapeExp" );

  // DQ (10/4/2008): I no longer agree that these are expressions, they are just parts of the SgUseStatment.
  // NEW_TERMINAL_MACRO (UseOnlyExpression,      "UseOnlyExpression",      "USE_ONLY_EXPR" );
  // NEW_TERMINAL_MACRO (UseRenameExpression,    "UseRenameExpression",    "USE_RENAME_EXPR" );

     NEW_TERMINAL_MACRO (IOItemExpression,       "IOItemExpression",       "IO_ITEM_EXPR" );
  // NEW_TERMINAL_MACRO (IOImpliedDo,            "IOImpliedDo",            "IO_IMPLIED_DO" );
     NEW_TERMINAL_MACRO (ImpliedDo,              "ImpliedDo",              "IMPLIED_DO" );

  // Binary operator
     NEW_TERMINAL_MACRO (ExponentiationOp,       "ExponentiationOp",       "EXPONENTIATION_OP" );

  // DQ (11/24/2007): Added new IR node for locations in the AST where post-processing is required.
     NEW_TERMINAL_MACRO (UnknownArrayOrFunctionReference, "UnknownArrayOrFunctionReference","TEMP_UnknownArrayOrFunctionReference" );

  // DQ (12/31/2007): This expression represents constructions such as "DIM=N" which binds 
  // a function parameter name with a function argument as in: "sum(array,DIM=1)".
     NEW_TERMINAL_MACRO (ActualArgumentExpression, "ActualArgumentExpression", "ACTUAL_ARGUMENT_EXPRESSION");

  // User defined operator for Fortran named operators.
     NEW_TERMINAL_MACRO (UserDefinedBinaryOp,   "UserDefinedBinaryOp",   "USER_DEFINED_BINARY_OP" );

  // DQ (1/31/2009): Added Fortran pointer assignment operator (to support pointer assignment statement).
     NEW_TERMINAL_MACRO (PointerAssignOp,       "PointerAssignOp",       "POINTER_ASSIGN_OP" );

  // FMZ (2/6/2009): Added CoArray Reference Expression
     NEW_TERMINAL_MACRO (CAFCoExpression,    "CAFCoExpression",    "COARRAY_REF_EXPR" );

#endif

  // An expression with a designator, used for designated initialization in
  // SgAggregateInitializer
     NEW_TERMINAL_MACRO (DesignatedInitializer, "DesignatedInitializer", "DESIGNATED_INITIALIZER" );

     NEW_NONTERMINAL_MACRO (Initializer,
                            AggregateInitializer | ConstructorInitializer | AssignInitializer | DesignatedInitializer,
                            "Initializer","EXPR_INIT", false);

  // User defined operator for Fortran named operators.
     NEW_TERMINAL_MACRO (UserDefinedUnaryOp,    "UserDefinedUnaryOp",    "USER_DEFINED_UNARY_OP" );

     NEW_TERMINAL_MACRO (PseudoDestructorRefExp, "PseudoDestructorRefExp", "PSEUDO_DESTRUCTOR_REF");

     NEW_NONTERMINAL_MACRO (UnaryOp,
                            ExpressionRoot | MinusOp            | UnaryAddOp | NotOp           | PointerDerefExp | 
                            AddressOfOp    | MinusMinusOp       | PlusPlusOp | BitComplementOp | CastExp         |
                            ThrowOp        | RealPartOp         | ImagPartOp | ConjugateOp     | UserDefinedUnaryOp,
                            "UnaryOp","UNARY_EXPRESSION", false);

  // DQ (2/2/2006): Support for Fortran IR nodes (contributed by Rice) (adding ExponentiationOp binary operator)
     NEW_NONTERMINAL_MACRO (BinaryOp,
          ArrowExp       | DotExp           | DotStarOp       | ArrowStarOp      | EqualityOp    | LessThanOp     | 
          GreaterThanOp  | NotEqualOp       | LessOrEqualOp   | GreaterOrEqualOp | AddOp         | SubtractOp     | 
          MultiplyOp     | DivideOp         | IntegerDivideOp | ModOp            | AndOp         | OrOp           |
          BitXorOp       | BitAndOp         | BitOrOp         | CommaOpExp       | LshiftOp      | RshiftOp       |
          PntrArrRefExp  | ScopeOp          | AssignOp        | PlusAssignOp     | MinusAssignOp | AndAssignOp    |
          IorAssignOp    | MultAssignOp     | DivAssignOp     | ModAssignOp      | XorAssignOp   | LshiftAssignOp |
          RshiftAssignOp | ExponentiationOp | ConcatenationOp | PointerAssignOp  | UserDefinedBinaryOp,"BinaryOp","BINARY_EXPRESSION", false);

     NEW_NONTERMINAL_MACRO (ValueExp,
          BoolValExp     | StringVal        | ShortVal               | CharVal         | UnsignedCharVal |
          WcharVal       | UnsignedShortVal | IntVal                 | EnumVal         | UnsignedIntVal  | 
          LongIntVal     | LongLongIntVal   | UnsignedLongLongIntVal | UnsignedLongVal | FloatVal        | 
          DoubleVal      | LongDoubleVal    | ComplexVal             |  UpcThreads     | UpcMythread,
          "ValueExp","ValueExpTag", false);

     NEW_NONTERMINAL_MACRO (Expression,
          UnaryOp             | BinaryOp                | ExprListExp         | VarRefExp           | ClassNameRefExp          |
          FunctionRefExp      | MemberFunctionRefExp    | ValueExp            | FunctionCallExp     | SizeOfOp                 |
          UpcLocalsizeofExpression| UpcBlocksizeofExpression| UpcElemsizeofExpression|
          TypeIdOp            | ConditionalExp          | NewExp              | DeleteExp           | ThisExp                  |
          RefExp              | Initializer             | VarArgStartOp       | VarArgOp            | VarArgEndOp              |
          VarArgCopyOp        | VarArgStartOneOperandOp | NullExpression      | VariantExpression   | SubscriptExpression      |
          ColonShapeExp       | AsteriskShapeExp        | /*UseOnlyExpression |*/ ImpliedDo         | IOItemExpression         |
       /* UseRenameExpression | */ StatementExpression  | AsmOp               | LabelRefExp         | ActualArgumentExpression |
          UnknownArrayOrFunctionReference               | PseudoDestructorRefExp | CAFCoExpression  |
          CudaKernelCallExp   | CudaKernelExecConfig, /* TV (04/22/2010): CUDA support */
          "Expression","ExpressionTag", false);

  // ***********************************************************************
  // ***********************************************************************
  //                       Header Code Declaration
  // ***********************************************************************
  // ***********************************************************************

#if 0
  // MK: I moved the following data member declaration from ../Grammar/Expression.code to this position:
     Expression.setDataPrototype("SgAttributePtrList", "uattributes", "",
				 NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif
#if 1
  // DQ (5/20/2004): Add need_paren to all expression objects so that we can trigger
  // it for any expression and use the value as set in EDG.  See how this works!
  // Added here to fix required paren in SgDotExp where it could not be set properly
  // See test code abstract_op.C line 418:
  //      Rhs.Array_Descriptor.Array_Domain.Push_Array_ID(rhsArrayID);
  // which is translated to 
  //      (*(&(Rhs.Array_Descriptor).Array_Domain)).Push_Array_ID(rhsArrayID);
  // That we have build generated the addess operator followed by the deref operator 
  // is a separate problem, I think!
     Expression.setDataPrototype ( "bool", "need_paren", "= false",
				    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE );
#endif

  // DQ (8/21/2004): Allow expressions to be marked as lvalues
     Expression.setDataPrototype ( "bool", "lvalue", "= false",
				    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE );
#if 0
  // DQ (12/18/2005): Added to support reference to global functions when locally 
  // scopes ones are available (see test2005_179.C).
  // Expression.setDataPrototype ( "bool", "global_qualified_name", "= false",
  //           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // Expression.setDataPrototype ( "SgQualifiedName*", "qualified_name", "= NULL",
  //           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Expression.setDataPrototype ( "SgQualifiedNamePtrList", "qualifiedNameList", "",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif
  // DQ (12/5/2006): Added back boolean to record use of explicit global qualifier.
  // This avoids some over qualification, this only applies to use of "::" prefix 
  // not more complex (longer) forms of name qualification.
     Expression.setDataPrototype ( "bool", "global_qualified_name", "= false",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Expression.setSubTreeFunctionPrototype ( "HEADER", "../Grammar/Common.code" );
  // Expression.excludeFunctionPrototype    ( "HEADER", "../Grammar/Common.code" );

     Expression.setFunctionPrototype ( "HEADER", "../Grammar/Expression.code" );

     Expression.setDataPrototype     ( "Sg_File_Info*", "operatorPosition", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);

  // Expression.setSubTreeFunctionPrototype ( "HEADER_FUNCTIONS", "../Grammar/Expression.code" );
  // Expression.excludeFunctionPrototype    ( "HEADER_FUNCTIONS", "../Grammar/Expression.code" );

#ifdef HL_GRAMMARS
     X_Expression.setFunctionPrototype          ( "HEADER_X_EXPRESSION",     "../Grammar/Expression.code" );
     X_Expression.setAutomaticGenerationOfCopyFunction(false);
     non_X_Expression.setFunctionPrototype      ( "HEADER_NON_X_EXPRESSION", "../Grammar/Expression.code" );
#endif

     UnaryOp.setFunctionPrototype             ( "HEADER_EXTRA_FUNCTIONS", "../Grammar/Expression.code" );
     BinaryOp.setFunctionPrototype            ( "HEADER_EXTRA_FUNCTIONS", "../Grammar/Expression.code" );

#if 0
  // DQ (1/14/2006): Removing the set_type() function since it shuld be computed from the operands directly
     Expression.setSubTreeFunctionPrototype   ( "HEADER_SET_TYPE", "../Grammar/Expression.code" );
     Expression.excludeFunctionPrototype      ( "HEADER_SET_TYPE", "../Grammar/Expression.code" );

#ifdef HL_GRAMMARS
     X_Expression.excludeFunctionPrototype    ( "HEADER_SET_TYPE", "../Grammar/Expression.code" );
     X_Expression.excludeFunctionPrototype    ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );
#endif

     UnaryOp.excludeFunctionPrototype         ( "HEADER_SET_TYPE", "../Grammar/Expression.code" );
     BinaryOp.excludeFunctionPrototype        ( "HEADER_SET_TYPE", "../Grammar/Expression.code" );
     Initializer.excludeFunctionPrototype     ( "HEADER_SET_TYPE", "../Grammar/Expression.code" );
     RefExp.excludeFunctionPrototype          ( "HEADER_SET_TYPE", "../Grammar/Expression.code" );
  // DQ (2/27/2005): Don't exclude this function for SgDotStarOp
  // DotStarOp.excludeFunctionPrototype       ( "HEADER_SET_TYPE", "../Grammar/Expression.code" );
     ValueExp.excludeFunctionPrototype        ( "HEADER_SET_TYPE", "../Grammar/Expression.code" );
     ValueExp.excludeSubTreeFunctionPrototype ( "HEADER_SET_TYPE", "../Grammar/Expression.code" );

  // We can't add this after it has been placed on the exclude list for the ValueExp subtree!
  // ValueExp.setFunctionPrototype            ( "HEADER_SET_TYPE", "../Grammar/Expression.code" );
     ValueExp.setFunctionPrototype            ( "HEADER_VALUE_EXPRESSION", "../Grammar/Expression.code" );
#endif

     Expression.setSubTreeFunctionPrototype   ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );

  // Within Expression get_type() is specific directly (as a virtual function)
     Expression.excludeFunctionPrototype      ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );

  // get_type is included directly within UnaryOp and BinaryOp 
     UnaryOp.excludeFunctionPrototype         ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );
     UnaryOp.excludeSubTreeFunctionPrototype  ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );

     BinaryOp.excludeFunctionPrototype        ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );
     BinaryOp.excludeSubTreeFunctionPrototype ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );

     ClassNameRefExp.excludeFunctionPrototype ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );
     ValueExp.excludeFunctionPrototype        ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );
     RefExp.excludeFunctionPrototype          ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );
     Initializer.excludeFunctionPrototype     ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );

  // This is the easiest solution, then where any post_construction_initialization() function
  // was ment to call the base class post_construction_initialization() function, we just do 
  // so directly in thederived class post_construction_initialization() function.
  // MK: the following two function calls could be wrapped into a single one:
     Expression.setFunctionPrototype( "HEADER_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     Expression.setSubTreeFunctionPrototype ( "HEADER_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
  // Expression.excludeFunctionPrototype ( "HEADER_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );


#if USE_UPC_IR_NODES
  // DQ and Liao (6/10/2008): Added new IR nodes specific to UPC.
     UpcLocalsizeofExpression.setFunctionPrototype ( "HEADER_UPC_LOCAL_SIZEOF_EXPRESSION", "../Grammar/Expression.code" );
     UpcLocalsizeofExpression.setDataPrototype ( "SgExpression*", "expression", "= NULL",
            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     UpcBlocksizeofExpression.setFunctionPrototype ( "HEADER_UPC_BLOCK_SIZEOF_EXPRESSION", "../Grammar/Expression.code" );
     UpcBlocksizeofExpression.setDataPrototype ( "SgExpression*", "expression", "= NULL",
            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     UpcElemsizeofExpression.setFunctionPrototype  ( "HEADER_UPC_ELEM_SIZEOF_EXPRESSION",  "../Grammar/Expression.code" );
     UpcElemsizeofExpression.setDataPrototype ( "SgExpression*", "expression", "= NULL",
            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif


  // DQ (1/14/2006): We should be using SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION instead of 
  // SOURCE_POST_CONSTRUCTION_INITIALIZATION_USING_SET_TYPE since we don't want to have a set_type
  // function on certain types of expressions (because the type should be computed from the operands 
  // or the value types directly).
     VarRefExp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );

     LabelRefExp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );

     ClassNameRefExp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );

     ArrowExp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     DotExp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );

  // DQ (2/27/2005): We need to have the type of the DotStarOp set (so uncomment this)
  // Bugfix (2/27/2001) we want to use the empty version of this function since then we don't 
  // have to build an empty version of the set_type member function (this just makes more sense).
     DotStarOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                    "../Grammar/Expression.code" );
     ArrowStarOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     EqualityOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     LessThanOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     GreaterThanOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     NotEqualOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     LessOrEqualOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     GreaterOrEqualOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     AddOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     SubtractOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     MultiplyOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     DivideOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     IntegerDivideOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     ModOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     AndOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     OrOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     BitXorOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     BitAndOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     BitOrOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     CommaOpExp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     LshiftOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     RshiftOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     MinusOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     UnaryAddOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     SizeOfOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     TypeIdOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );

     NotOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     PointerDerefExp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     AddressOfOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                  "../Grammar/Expression.code" );

     BitComplementOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );

     RealPartOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );

     ImagPartOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );

     ConjugateOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );

     PntrArrRefExp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );

     ScopeOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     AssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     PlusAssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     MinusAssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     AndAssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     IorAssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     MultAssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     DivAssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     ModAssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     XorAssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     LshiftAssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     RshiftAssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );
     PointerAssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );

     ThrowOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );

     ConcatenationOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );

  // DQ (2/5/2004): Adding support for varargs in AST
     VarArgStartOp.setFunctionSource           ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     VarArgStartOneOperandOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     VarArgOp.setFunctionSource      ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     VarArgEndOp.setFunctionSource   ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     VarArgCopyOp.setFunctionSource  ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );

  // *** Expression objects using empty (non-existant) post_construction_initialization() member functions
     ExpressionRoot.setFunctionSource   ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     ExprListExp.setFunctionSource      ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     ValueExp.setFunctionSource         ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     BoolValExp.setFunctionSource       ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     ShortVal.setFunctionSource         ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     CharVal.setFunctionSource          ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     UnsignedCharVal.setFunctionSource  ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     UnsignedShortVal.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     IntVal.setFunctionSource           ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     EnumVal.setFunctionSource          ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     UnsignedIntVal.setFunctionSource   ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     LongIntVal.setFunctionSource       ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     LongLongIntVal.setFunctionSource   ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     UnsignedLongLongIntVal.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     UnsignedLongVal.setFunctionSource  ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     FloatVal.setFunctionSource         ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     DoubleVal.setFunctionSource        ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     LongDoubleVal.setFunctionSource    ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     ComplexVal.setFunctionSource       ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     ThisExp.setFunctionSource          ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     RefExp.setFunctionSource           ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     Initializer.setFunctionSource      ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );

     NullExpression.setFunctionSource   ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     VariantExpression.setFunctionSource( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );

     StatementExpression.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     AsmOp.setFunctionSource               ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );

     UpcThreads.setFunctionSource          ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     UpcMythread.setFunctionSource         ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );

     UserDefinedUnaryOp.setFunctionSource  ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     UserDefinedBinaryOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );

  // DQ (2/27/2005): We want to post_construction_initialization to call set_type so we don't want 
  // and empty function here plus I have added a set_type function for DotStarOp.
  // Bugfix (2/27/2001) Generate this empty function instead of one with a call to an empty setType() function
  // DotStarOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );

  // It is almost true that the presendence operators exist only on classes derived 
  // from UnaryOp and BinaryOp plus the SizeOfOp and TypeIdOp, but in fact 
  // the ExpressionRoot is derived from UnaryOp and does not have a precedence 
  // member function.
     UnaryOp.setSubTreeFunctionPrototype     ( "HEADER_PRECEDENCE", "../Grammar/Expression.code" );
     UnaryOp.excludeFunctionPrototype        ( "HEADER_PRECEDENCE", "../Grammar/Expression.code" );
     ExpressionRoot.excludeFunctionPrototype ( "HEADER_PRECEDENCE", "../Grammar/Expression.code" );
     BinaryOp.setSubTreeFunctionPrototype    ( "HEADER_PRECEDENCE", "../Grammar/Expression.code" );
     BinaryOp.excludeFunctionPrototype       ( "HEADER_PRECEDENCE", "../Grammar/Expression.code" );


  // DQ (2/1/2009: Added comment.
  // ***********************************************************
  //    This whole mechanism is not used presently.  Someone
  //    implemented a table in: "int GetPrecedence(int variant)"
  //    in file: modified_sage.C in the unparser.  This table 
  //    is used to get all operator precedence information.
  //    The information set below is an older mechanism (that
  //    was ignored by the person who reimplemented the precedence
  //    support. Either this code should be kept and the GetPrecedence()
  //    function modified to use it, or this code should be removed.
  //    I would like to have the precedence be part of the operators
  //    instead of accessed via the unparser, so I would prefer to
  //    modify the GetPrecedence() function in the unparser.
  //    This may be done soon.
  // ***********************************************************

  // Now set the precedence values for each leaf of the grammar 
  // (where the precedence member function is defined)
     SizeOfOp.editSubstitute        ( "PRECEDENCE_VALUE", "16" );
     TypeIdOp.editSubstitute        ( "PRECEDENCE_VALUE", "16" );
     ArrowExp.editSubstitute        ( "PRECEDENCE_VALUE", "16" );
     DotExp.editSubstitute          ( "PRECEDENCE_VALUE", "16" );
     DotStarOp.editSubstitute       ( "PRECEDENCE_VALUE", "14" );
     ArrowStarOp.editSubstitute     ( "PRECEDENCE_VALUE", "14" );
  // DQ (8/8/2006): A review of operator precedence with Jeremiah pointed out a number of errors!
  // EqualityOp.editSubstitute      ( "PRECEDENCE_VALUE", " 2" );
     EqualityOp.editSubstitute      ( "PRECEDENCE_VALUE", " 9" );
     LessThanOp.editSubstitute      ( "PRECEDENCE_VALUE", "10" );
     GreaterThanOp.editSubstitute   ( "PRECEDENCE_VALUE", "10" );
     NotEqualOp.editSubstitute      ( "PRECEDENCE_VALUE", " 9" );
     LessOrEqualOp.editSubstitute   ( "PRECEDENCE_VALUE", "10" );
     GreaterOrEqualOp.editSubstitute( "PRECEDENCE_VALUE", "10" );
     AddOp.editSubstitute           ( "PRECEDENCE_VALUE", "12" );
     SubtractOp.editSubstitute      ( "PRECEDENCE_VALUE", "12" );
     MultiplyOp.editSubstitute      ( "PRECEDENCE_VALUE", "13" );
     DivideOp.editSubstitute        ( "PRECEDENCE_VALUE", "13" );
     IntegerDivideOp.editSubstitute ( "PRECEDENCE_VALUE", "13" );
     ModOp.editSubstitute           ( "PRECEDENCE_VALUE", "13" );
     AndOp.editSubstitute           ( "PRECEDENCE_VALUE", " 5" );
     OrOp.editSubstitute            ( "PRECEDENCE_VALUE", " 4" );
     BitXorOp.editSubstitute        ( "PRECEDENCE_VALUE", " 7" );
     BitAndOp.editSubstitute        ( "PRECEDENCE_VALUE", " 8" );
     BitOrOp.editSubstitute         ( "PRECEDENCE_VALUE", " 6" );
     CommaOpExp.editSubstitute      ( "PRECEDENCE_VALUE", " 1" ); // lowest precedence
     LshiftOp.editSubstitute        ( "PRECEDENCE_VALUE", "11" );
     RshiftOp.editSubstitute        ( "PRECEDENCE_VALUE", "11" );
     MinusOp.editSubstitute         ( "PRECEDENCE_VALUE", "15" );
     UnaryAddOp.editSubstitute      ( "PRECEDENCE_VALUE", "15" );
  // SizeOfOp.editSubstitute        ( "PRECEDENCE_VALUE", "16" );
  // TypeIdOp.editSubstitute        ( "PRECEDENCE_VALUE", "16" );
     NotOp.editSubstitute           ( "PRECEDENCE_VALUE", "15" );
     PointerDerefExp.editSubstitute ( "PRECEDENCE_VALUE", "15" );
     AddressOfOp.editSubstitute     ( "PRECEDENCE_VALUE", "15" );
     MinusMinusOp.editSubstitute    ( "PRECEDENCE_VALUE", "15" );
     PlusPlusOp.editSubstitute      ( "PRECEDENCE_VALUE", "15" );
     BitComplementOp.editSubstitute ( "PRECEDENCE_VALUE", "15" );
     RealPartOp.editSubstitute      ( "PRECEDENCE_VALUE", "15" );
     ImagPartOp.editSubstitute      ( "PRECEDENCE_VALUE", "15" );
     ConjugateOp.editSubstitute      ( "PRECEDENCE_VALUE", "15" );
     CastExp.editSubstitute         ( "PRECEDENCE_VALUE", "15" );
     PntrArrRefExp.editSubstitute   ( "PRECEDENCE_VALUE", "16" );
     ScopeOp.editSubstitute         ( "PRECEDENCE_VALUE", "17" ); // highest precedence
     AssignOp.editSubstitute        ( "PRECEDENCE_VALUE", " 2" );
     PlusAssignOp.editSubstitute    ( "PRECEDENCE_VALUE", " 2" );
     MinusAssignOp.editSubstitute   ( "PRECEDENCE_VALUE", " 2" );
     AndAssignOp.editSubstitute     ( "PRECEDENCE_VALUE", " 2" );
     IorAssignOp.editSubstitute     ( "PRECEDENCE_VALUE", " 2" );
     MultAssignOp.editSubstitute    ( "PRECEDENCE_VALUE", " 2" );
     DivAssignOp.editSubstitute     ( "PRECEDENCE_VALUE", " 2" );
     ModAssignOp.editSubstitute     ( "PRECEDENCE_VALUE", " 2" );
     XorAssignOp.editSubstitute     ( "PRECEDENCE_VALUE", " 2" );
     LshiftAssignOp.editSubstitute  ( "PRECEDENCE_VALUE", " 2" );
     RshiftAssignOp.editSubstitute  ( "PRECEDENCE_VALUE", " 2" );
     PointerAssignOp.editSubstitute ( "PRECEDENCE_VALUE", " 2" );
     ThrowOp.editSubstitute         ( "PRECEDENCE_VALUE", "15" );

     ConcatenationOp.editSubstitute ( "PRECEDENCE_VALUE", " 3" );

  // DQ (2/5/2004): Adding support for varargs in AST
     VarArgStartOp.editSubstitute   ( "PRECEDENCE_VALUE", "16" );
     VarArgStartOneOperandOp.editSubstitute   ( "PRECEDENCE_VALUE", "16" );
     VarArgOp.editSubstitute        ( "PRECEDENCE_VALUE", "16" );
     VarArgEndOp.editSubstitute     ( "PRECEDENCE_VALUE", "16" );
     VarArgCopyOp.editSubstitute    ( "PRECEDENCE_VALUE", "16" );

     NullExpression.editSubstitute    ( "PRECEDENCE_VALUE", "16" );
     VariantExpression.editSubstitute ( "PRECEDENCE_VALUE", "16" );

  // DQ (7/21/2006): Added support for GNU statement expression extension.
     StatementExpression.editSubstitute ( "PRECEDENCE_VALUE", "16" );
     AsmOp.editSubstitute ( "PRECEDENCE_VALUE", "16" );

#if USE_FORTRAN_IR_NODES
  // DQ (3/19/2007): Support for Fortran IR nodes (not sure if these are correct values)
  // IntrinsicFn.editSubstitute         ( "PRECEDENCE_VALUE", " 2" );

     SubscriptExpression.editSubstitute ( "PRECEDENCE_VALUE", " 2" );

  // The more general SubscriptExpression has replaced this IR node in ROSE.
  // SubscriptColon.editSubstitute      ( "PRECEDENCE_VALUE", " 2" );

  // Note that SgColonExp and SgAsteriskExp are used in array shape specification, but not indexing, 
  // so they are not part of subscript expressions.
  // DQ (11/18/2007): I think this should have lowest precedence (unclear if this really 
  // made any difference, in the end I need to handle this as a special case to about over use of "()"
  // Colon.editSubstitute               ( "PRECEDENCE_VALUE", " 2" );
     ColonShapeExp.editSubstitute       ( "PRECEDENCE_VALUE", " 17" );

  // DQ (11/24/2007): renamed
  // SubscriptAsterisk.editSubstitute   ( "PRECEDENCE_VALUE", " 2" );
     AsteriskShapeExp.editSubstitute    ( "PRECEDENCE_VALUE", " 2" );

  // DQ (10/4/2008): I no longer agree that these are expressions, they are just parts of the SgUseStatment.
  // UseOnlyExpression.editSubstitute   ( "PRECEDENCE_VALUE", " 2" );
  // UseRenameExpression.editSubstitute ( "PRECEDENCE_VALUE", " 2" );

     IOItemExpression.editSubstitute    ( "PRECEDENCE_VALUE", " 2" );
     ImpliedDo.editSubstitute           ( "PRECEDENCE_VALUE", " 2" );
     ExponentiationOp.editSubstitute    ( "PRECEDENCE_VALUE", " 2" );

  // DQ (11/24/2007): Added new IR node for locations in the AST where post-processing is required.
     UnknownArrayOrFunctionReference.editSubstitute ( "PRECEDENCE_VALUE", " 2" );

     ActualArgumentExpression.editSubstitute ( "PRECEDENCE_VALUE", " 2" );

     UserDefinedUnaryOp.editSubstitute  ( "PRECEDENCE_VALUE", " 2" );
     UserDefinedBinaryOp.editSubstitute ( "PRECEDENCE_VALUE", " 2" );

     PseudoDestructorRefExp.editSubstitute ( "PRECEDENCE_VALUE", " 2" );

     // FMZ (2/6/2009): Added for SgCAFCoExpression--following SgPntrArrRefExp
     CAFCoExpression.editSubstitute ( "PRECEDENCE_VALUE", " 16" );

#if 0
  // Extra required Fortran IR nodes
     KeywordValueExpression
#endif

#endif

     CudaKernelExecConfig.editSubstitute ( "PRECEDENCE_VALUE", " 0" );
     CudaKernelCallExp.editSubstitute ( "PRECEDENCE_VALUE", " 0" );


     UnaryOp.setFunctionPrototype ( "HEADER_GET_NEXT_EXPRESSION", "../Grammar/Expression.code" );
     BinaryOp.setFunctionPrototype ( "HEADER_GET_NEXT_EXPRESSION", "../Grammar/Expression.code" );
     FunctionCallExp.setFunctionPrototype ( "HEADER_GET_NEXT_EXPRESSION", "../Grammar/Expression.code" );
     ConditionalExp.setFunctionPrototype ( "HEADER_GET_NEXT_EXPRESSION", "../Grammar/Expression.code" );
     NewExp.setFunctionPrototype ( "HEADER_GET_NEXT_EXPRESSION", "../Grammar/Expression.code" );
     DeleteExp.setFunctionPrototype ( "HEADER_GET_NEXT_EXPRESSION", "../Grammar/Expression.code" );

     Initializer.setSubTreeFunctionPrototype ( "HEADER_GET_NEXT_EXPRESSION", "../Grammar/Expression.code" );
     Initializer.excludeFunctionPrototype ( "HEADER_GET_NEXT_EXPRESSION", "../Grammar/Expression.code" );

     UnaryOp.setFunctionPrototype ( "HEADER_UNARY_EXPRESSION", "../Grammar/Expression.code" );
     UnaryOp.setDataPrototype ( "SgExpression*", "operand_i", "= NULL",
				CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (1/14/2006): We should not store the type of unary operators but instead obtain it from the operand directly.
  // However, we can't do that because in a few cases the type is changed as a result of the operator (e.g. SgAddressOp, SgPointerDerefExp).
  // The solution is to have specially built versions of the get_type() function for those operators.
  // An incremental solution is to first eliminate the access functions.
  // UnaryOp.setDataPrototype ( "SgType*", "expression_type", "= NULL",
  //        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);
     UnaryOp.setDataPrototype ( "SgType*", "expression_type", "= NULL",
            CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);

     UnaryOp.setDataPrototype ( "SgUnaryOp::Sgop_mode", "mode", "= prefix",
				NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // MK: UnaryOp.excludeDataPrototype ( "SgUnaryOp::Sgop_mode", "mode", "= prefix");

     BinaryOp.setFunctionPrototype ( "HEADER_BINARY_EXPRESSION", "../Grammar/Expression.code" );
     BinaryOp.setDataPrototype ( "SgExpression*", "lhs_operand_i"  , "= NULL",
            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     BinaryOp.setDataPrototype ( "SgExpression*", "rhs_operand_i"  , "= NULL",
            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  // DQ (1/14/2006): We should not store the type of unary operators but instead obtain it from the operand directly.
  // BinaryOp.setDataPrototype ( "SgType*"      , "expression_type", "= NULL",
  //        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);
     BinaryOp.setDataPrototype ( "SgType*"      , "expression_type", "= NULL",
            CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);

     ExpressionRoot.setFunctionPrototype ( "HEADER_EXPRESSION_ROOT_EXPRESSION", "../Grammar/Expression.code" );
  // QY:9/30/04: remove statement pointer. use parent pointer instead
  // ExpressionRoot.setDataPrototype ( "SgStatement*", "statement", "= NULL", 
  //      CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // ExpressionRoot.setAutomaticGenerationOfDataAccessFunctions(false);

     ExprListExp.setFunctionPrototype ( "HEADER_EXPRESSION_LIST_EXPRESSION", "../Grammar/Expression.code" );

  // MK: I moved the following data member declaration from ../Grammar/Expression.code to this position:
#if 0
     ExprListExp.setDataPrototype("SgExpressionPtrList", "expressions", "= NULL",
				  NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#else
     ExprListExp.setDataPrototype("SgExpressionPtrList", "expressions", "",
				  NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif
 
     ExprListExp.editSubstitute       ( "HEADER_LIST_DECLARATIONS", "HEADER_LIST_FUNCTIONS", "../Grammar/Expression.code" );
  // ExprListExp.editSubstitute       ( "LIST_DATA_TYPE", "Expression" );
     ExprListExp.editSubstitute       ( "LIST_NAME", "expression" );

     VarRefExp.setFunctionPrototype ( "HEADER_VAR_REF_EXPRESSION", "../Grammar/Expression.code" );
     VarRefExp.setDataPrototype ( "SgVariableSymbol*", "symbol", "= NULL",
				  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     LabelRefExp.setFunctionPrototype ( "HEADER_LABEL_REF_EXPRESSION", "../Grammar/Expression.code" );
     LabelRefExp.setDataPrototype ( "SgLabelSymbol*", "symbol", "= NULL",
				  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     ClassNameRefExp.setFunctionPrototype ( "HEADER_CLASS_NAME_REF_EXPRESSION", "../Grammar/Expression.code" );
     ClassNameRefExp.setDataPrototype ( "SgClassSymbol*", "symbol", "= NULL",
					CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     FunctionRefExp.setFunctionPrototype ( "HEADER_FUNCTION_REF_EXPRESSION", "../Grammar/Expression.code" );
     FunctionRefExp.setDataPrototype ( "SgFunctionSymbol*", "symbol_i"     , "= NULL",
				       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (1/14/2006): The function type should be computed from the function declaration (instead of being stored)
  // Leave the type in the constructor for storage internally and build a special version of get_type() to access 
  // this value or later compute it directly.
  // FunctionRefExp.setDataPrototype ( "SgFunctionType*"  , "function_type", "= NULL",
  //        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);
     FunctionRefExp.setDataPrototype ( "SgFunctionType*"  , "function_type", "= NULL",
            CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);

     MemberFunctionRefExp.setFunctionPrototype ( "HEADER_MEMBER_FUNCTION_REF_EXPRESSION", "../Grammar/Expression.code" );
     MemberFunctionRefExp.setDataPrototype ( "SgMemberFunctionSymbol*", "symbol_i", "= NULL",
            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     MemberFunctionRefExp.setDataPrototype ( "int", "virtual_call", "= 0",
            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (1/14/2006): The function type should be computed from the function declaration (instead of being stored)
  // Leave the type in the constructor for storage internally and build a special version of get_type() to access 
  // this value or later compute it directly.
  // MemberFunctionRefExp.setDataPrototype ( "SgFunctionType*", "function_type", "= NULL",
  //        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);
     MemberFunctionRefExp.setDataPrototype ( "SgFunctionType*", "function_type", "= NULL",
            CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);

  // DQ (4/13/2004): Changed false to true in default setting (and removed resetting of value in 
  //                 post_constructor_initialization(), as suggested by Qing).
     MemberFunctionRefExp.setDataPrototype ( "int", "need_qualifier", "= true",
					     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     ValueExp.setFunctionPrototype ( "HEADER_VALUE_EXPRESSION", "../Grammar/Expression.code" );

  // DQ (6/19/2006): Changed name of data member to be consitant with more general use in SgCastExp
  // DQ (11/9/2005): Added reference to expression tree for original unfolded constant expressions.
  // Constant folding in EDG and ROSE allows us to ignore this subtree, but it is here to to permit
  // the original source code to be faithfully represented.
     ValueExp.setDataPrototype ( "SgExpression*", "originalExpressionTree", "= NULL",
				 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     BoolValExp.setFunctionPrototype ( "HEADER_BOOLEAN_VALUE_EXPRESSION", "../Grammar/Expression.code" );
     BoolValExp.setDataPrototype ( "int", "value", "= 0",
				   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     StringVal.setFunctionPrototype ( "HEADER_STRING_VALUE_EXPRESSION", "../Grammar/Expression.code" );

  // DQ (3/25/2006): We can have ROSETTA generate the constructor now that we use a C++ style std::string
  // StringVal.setAutomaticGenerationOfConstructor(false);
  // DQ (12/4/2004): Now we automate the generation of the destructors
  // StringVal.setAutomaticGenerationOfDestructor (false);

  // DQ (3/25/2006): This should take a const char (since we don't modified it and it make for a simpler interface)
  // StringVal.setDataPrototype ( "char*", "value", "= NULL",
  //          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // StringVal.setDataPrototype ( "const char*", "value", "= NULL",
  //          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     StringVal.setDataPrototype ( "std::string", "value", "= \"\"",
              CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     StringVal.setDataPrototype ( "bool", "wcharString", "= false",
              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     StringVal.setDataPrototype ( "bool", "usesSingleQuotes", "= false",
              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (12/23/2007): Added support for distinguishing double quotes (permits use of sing, double, or un-quoted strings in the SgFormatItem object).
     StringVal.setDataPrototype ( "bool", "usesDoubleQuotes", "= false",
              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // URK (08/22/2006): Added string to hold source code constants of integer and character types precisely.
     ShortVal.setFunctionPrototype ( "HEADER_SHORT_VALUE_EXPRESSION", "../Grammar/Expression.code" );
     ShortVal.setDataPrototype ( "short", "value", "= 0",
				 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     ShortVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
				       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     CharVal.setFunctionPrototype ( "HEADER_CHAR_VALUE_EXPRESSION", "../Grammar/Expression.code" );
     CharVal.setDataPrototype ( "char", "value", "= 0",
				CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     CharVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
				       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     UnsignedCharVal.setFunctionPrototype ( "HEADER_UNSIGNED_CHAR_VALUE_EXPRESSION", "../Grammar/Expression.code" );
     UnsignedCharVal.setDataPrototype ( "unsigned char", "value", "= 0",
					CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UnsignedCharVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
				       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // This stores values such as L'ab' where as a SgCharVal would store 'x'.
     WcharVal.setFunctionPrototype ( "HEADER_WCHAR_VALUE_EXPRESSION", "../Grammar/Expression.code" );
     WcharVal.setDataPrototype ( "unsigned long", "valueUL", "= 0",
				 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     WcharVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
				       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     UnsignedShortVal.setFunctionPrototype ( "HEADER_UNSIGNED_SHORT_VALUE_EXPRESSION", "../Grammar/Expression.code" );
     UnsignedShortVal.setDataPrototype ( "unsigned short", "value", "= 0",
					 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UnsignedShortVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
				       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     IntVal.setFunctionPrototype ( "HEADER_INT_VALUE_EXPRESSION", "../Grammar/Expression.code" );
     IntVal.setDataPrototype ( "int", "value", "= 0",
			       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     IntVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
				       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     EnumVal.setFunctionPrototype ( "HEADER_ENUM_VALUE_EXPRESSION", "../Grammar/Expression.code" );
     EnumVal.setDataPrototype ( "int", "value", "= 0",
				CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     EnumVal.setDataPrototype ( "SgEnumDeclaration*", "declaration", "= NULL",
				CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // We do not traverse the following data member for the moment!
     EnumVal.setDataPrototype ( "SgName", "name", "= \"\"",
				CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (12/22/2006): Record if name qualification is required where used as a constant. 
  // See test2003_01.C for an example of where this is required.
     EnumVal.setDataPrototype("bool", "requiresNameQualification", "= false",
                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     UnsignedIntVal.setFunctionPrototype ( "HEADER_UNSIGNED_INT_VALUE_EXPRESSION", "../Grammar/Expression.code" );
     UnsignedIntVal.setDataPrototype ( "unsigned int", "value", "= 0",
				       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UnsignedIntVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
				       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     LongIntVal.setFunctionPrototype ( "HEADER_LONG_INT_VALUE_EXPRESSION", "../Grammar/Expression.code" );
     LongIntVal.setDataPrototype ( "long int", "value", "= 0",
				   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     LongIntVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
				       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     LongLongIntVal.setFunctionPrototype ( "HEADER_LONG_LONG_VALUE_EXPRESSION", "../Grammar/Expression.code" );
     LongLongIntVal.setDataPrototype ( "long long int", "value", "= 0",
				       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     LongLongIntVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
				       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     UnsignedLongLongIntVal.setFunctionPrototype ( "HEADER_UNSIGNED_LONG_LONG_VALUE_EXPRESSION", "../Grammar/Expression.code" );
     UnsignedLongLongIntVal.setDataPrototype ( "unsigned long long int", "value", "= 0",
					       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UnsignedLongLongIntVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
				       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     UnsignedLongVal.setFunctionPrototype ( "HEADER_UNSIGNED_LONG_VALUE_EXPRESSION", "../Grammar/Expression.code" );
     UnsignedLongVal.setDataPrototype ( "unsigned long", "value", "= 0",
					CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UnsignedLongVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
				       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     FloatVal.setFunctionPrototype ( "HEADER_FLOAT_VALUE_EXPRESSION", "../Grammar/Expression.code" );
     FloatVal.setDataPrototype ( "float", "value", "= 0.0",
				 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (11/9/2005): Added string to hold source code constant precisely (part of work with Andreas)
     FloatVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
				 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     DoubleVal.setFunctionPrototype ( "HEADER_DOUBLE_VALUE_EXPRESSION", "../Grammar/Expression.code" );
     DoubleVal.setDataPrototype ( "double", "value", "= 0.0",
				  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (11/9/2005): Added string to hold source code constant precisely (part of work with Andreas)
     DoubleVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
				 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     LongDoubleVal.setFunctionPrototype ( "HEADER_LONG_DOUBLE_VALUE_EXPRESSION", "../Grammar/Expression.code" );
     LongDoubleVal.setDataPrototype ( "long double", "value", "= 0.0",
				      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (11/9/2005): Added string to hold source code constant precisely (part of work with Andreas)
     LongDoubleVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
				 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (8/27/2006): Added support for Complex values (save the values as long doubles internally within the AST)
  // JJW (11/22/2008): Changed members to SgValueExp*; real_value can be NULL for imaginary numbers
     ComplexVal.setFunctionPrototype ( "HEADER_COMPLEX_VALUE_EXPRESSION", "../Grammar/Expression.code" );
     ComplexVal.setDataPrototype ( "SgValueExp*", "real_value", "",
				 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     ComplexVal.setDataPrototype ( "SgValueExp*", "imaginary_value", "",
				 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     ComplexVal.setDataPrototype ( "SgType*", "precisionType", "= NULL",
             CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (11/9/2005): Added string to hold source code constant precisely (part of work with Andreas)
     ComplexVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
				 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Liao 6/18/2008, UPC THREADS, MYTHREAD 
     UpcThreads.setFunctionPrototype ( "HEADER_UPC_THREADS_EXPRESSION", "../Grammar/Expression.code" );
     UpcThreads.setDataPrototype ( "int", "value", "= 0",
			       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UpcThreads.setDataPrototype ( "std::string", "valueString", "= \"\"",
				       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     UpcMythread.setFunctionPrototype ( "HEADER_UPC_MYTHREAD_EXPRESSION", "../Grammar/Expression.code" );
     UpcMythread.setDataPrototype ( "int", "value", "= 0",
			       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UpcMythread.setDataPrototype ( "std::string", "valueString", "= \"\"",
				       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     FunctionCallExp.setFunctionPrototype ( "HEADER_FUNCTION_CALL_EXPRESSION", "../Grammar/Expression.code" );
     FunctionCallExp.editSubstitute       ( "HEADER_LIST_DECLARATIONS", "HEADER_LIST_FUNCTIONS", "../Grammar/Expression.code" );
     FunctionCallExp.editSubstitute       ( "LIST_NAME", "arg" );
  // FunctionCallExp.editSubstitute       ( "LIST_FUNCTION_RETURN_TYPE", "void" );
     FunctionCallExp.setDataPrototype ( "SgExpression*", "function", "= NULL",
					CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     FunctionCallExp.setDataPrototype ( "SgExprListExp*", "args", "= NULL",
					CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  // DQ (1/14/2006): We should not store the type of unary operators but instead obtain it from the operand directly.
  // FunctionCallExp.setDataPrototype ( "SgType*", "expression_type", "= NULL",
  //        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);
     FunctionCallExp.setDataPrototype ( "SgType*", "expression_type", "= NULL",
            CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);

     ArrowExp.setFunctionPrototype ( "HEADER_ARROW_EXPRESSION", "../Grammar/Expression.code" );

     DotExp.setFunctionPrototype ( "HEADER_DOT_EXPRESSION", "../Grammar/Expression.code" );

     DotStarOp.setFunctionPrototype ( "HEADER_DOT_STAR_OPERATOR", "../Grammar/Expression.code" );
     ArrowStarOp.setFunctionPrototype ( "HEADER_ARROW_STAR_OPERATOR", "../Grammar/Expression.code" );


     EqualityOp.setFunctionPrototype ( "HEADER_EQUALITY_OPERATOR", "../Grammar/Expression.code" );
     LessThanOp.setFunctionPrototype ( "HEADER_LESS_THAN_OPERATOR", "../Grammar/Expression.code" );
     GreaterThanOp.setFunctionPrototype ( "HEADER_GREATER_THAN_OPERATOR", "../Grammar/Expression.code" );
     NotEqualOp.setFunctionPrototype ( "HEADER_NOT_EQUAL_OPERATOR", "../Grammar/Expression.code" );
     LessOrEqualOp.setFunctionPrototype ( "HEADER_LESS_OR_EQUAL_OPERATOR", "../Grammar/Expression.code" );
     GreaterOrEqualOp.setFunctionPrototype ( "HEADER_GREATER_OR_EQUAL_OPERATOR", "../Grammar/Expression.code" );

  // DQ (6/20/2006): Relational Operators must return bool type
     EqualityOp.editSubstitute       ( "HEADER_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "HEADER_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );
     LessThanOp.editSubstitute       ( "HEADER_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "HEADER_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );
     GreaterThanOp.editSubstitute    ( "HEADER_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "HEADER_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );
     NotEqualOp.editSubstitute       ( "HEADER_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "HEADER_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );
     LessOrEqualOp.editSubstitute    ( "HEADER_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "HEADER_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );
     GreaterOrEqualOp.editSubstitute ( "HEADER_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "HEADER_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );

     AddOp.setFunctionPrototype ( "HEADER_ADD_OPERATOR", "../Grammar/Expression.code" );
     SubtractOp.setFunctionPrototype ( "HEADER_SUBTRACT_OPERATOR", "../Grammar/Expression.code" );
     MultiplyOp.setFunctionPrototype ( "HEADER_MULTIPLY_OPERATOR", "../Grammar/Expression.code" );
     DivideOp.setFunctionPrototype ( "HEADER_DIVIDE_OPERATOR", "../Grammar/Expression.code" );
     IntegerDivideOp.setFunctionPrototype ( "HEADER_INTEGER_DIVIDE_OPERATOR", "../Grammar/Expression.code" );
     ModOp.setFunctionPrototype ( "HEADER_MOD_OPERATOR", "../Grammar/Expression.code" );
     AndOp.setFunctionPrototype ( "HEADER_AND_OPERATOR", "../Grammar/Expression.code" );
     OrOp.setFunctionPrototype ( "HEADER_OR_OPERATOR", "../Grammar/Expression.code" );
     BitXorOp.setFunctionPrototype ( "HEADER_BIT_XOR_OPERATOR", "../Grammar/Expression.code" );
     BitAndOp.setFunctionPrototype ( "HEADER_BIT_AND_OPERATOR", "../Grammar/Expression.code" );
     BitOrOp.setFunctionPrototype ( "HEADER_BIT_OR_OPERATOR", "../Grammar/Expression.code" );
     CommaOpExp.setFunctionPrototype ( "HEADER_COMMA_OPERATOR_EXPRESSION", "../Grammar/Expression.code" );
     LshiftOp.setFunctionPrototype ( "HEADER_LEFT_SHIFT_OPERATOR", "../Grammar/Expression.code" );
     RshiftOp.setFunctionPrototype ( "HEADER_RIGHT_SHIFT_OPERATOR", "../Grammar/Expression.code" );
     MinusOp.setFunctionPrototype ( "HEADER_MINUS_OPERATOR", "../Grammar/Expression.code" );
     UnaryAddOp.setFunctionPrototype ( "HEADER_UNARY_ADD_OPERATOR", "../Grammar/Expression.code" );

     SizeOfOp.setFunctionPrototype ( "HEADER_SIZEOF_OPERATOR", "../Grammar/Expression.code" );
     SizeOfOp.setDataPrototype ( "SgExpression*", "operand_expr", "= NULL",
				 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     SizeOfOp.setDataPrototype ( "SgType*", "operand_type", "= NULL",
				 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);
  // DQ (1/14/2006): We should not store the type of unary operators but instead obtain it from the operand directly.
  // SizeOfOp.setDataPrototype ( "SgType*", "expression_type", "= NULL",
  //        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);
     SizeOfOp.setDataPrototype ( "SgType*", "expression_type", "= NULL",
            CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);

     TypeIdOp.setFunctionPrototype ( "HEADER_TYPE_ID_OPERATOR", "../Grammar/Expression.code" );
     TypeIdOp.setDataPrototype ( "SgExpression*", "operand_expr"   , "= NULL",
				 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     TypeIdOp.setDataPrototype ( "SgType*"      , "operand_type"   , "= NULL",
				 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);
  // DQ (1/14/2006): We should not store the type of unary operators but instead obtain it from the operand directly.
  // TypeIdOp.setDataPrototype ( "SgType*"      , "expression_type", "= NULL",
  //        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);
     TypeIdOp.setDataPrototype ( "SgType*"      , "expression_type", "= NULL",
            CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);

  // DQ (2/5/2004): Adding vararg support for SAGE AST
     VarArgStartOp.setFunctionPrototype ( "HEADER_VARARG_START_OPERATOR", "../Grammar/Expression.code" );
     VarArgStartOp.setDataPrototype ( "SgExpression*", "lhs_operand"   , "= NULL",
				 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     VarArgStartOp.setDataPrototype ( "SgExpression*", "rhs_operand"   , "= NULL",
				 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     VarArgStartOp.setDataPrototype ( "SgType*", "expression_type", "= NULL",
				 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);
     VarArgStartOneOperandOp.setDataPrototype ( "SgExpression*", "operand_expr"   , "= NULL",
				 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     VarArgStartOneOperandOp.setDataPrototype ( "SgType*", "expression_type", "= NULL",
				 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);

     VarArgOp.setFunctionPrototype ( "HEADER_VARARG_OPERATOR", "../Grammar/Expression.code" );
     VarArgOp.setDataPrototype  ( "SgExpression*", "operand_expr"   , "= NULL",
				 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     VarArgOp.setDataPrototype ( "SgType*", "expression_type", "= NULL",
				 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);

     VarArgEndOp.setFunctionPrototype ( "HEADER_VARARG_END_OPERATOR", "../Grammar/Expression.code" );
     VarArgEndOp.setDataPrototype  ( "SgExpression*", "operand_expr"   , "= NULL",
				 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     VarArgEndOp.setDataPrototype ( "SgType*", "expression_type", "= NULL",
				 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);
     VarArgCopyOp.setDataPrototype  ( "SgExpression*", "lhs_operand"   , "= NULL",
				 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     VarArgCopyOp.setDataPrototype  ( "SgExpression*", "rhs_operand"   , "= NULL",
				 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     VarArgCopyOp.setDataPrototype ( "SgType*", "expression_type", "= NULL",
				 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);

  // DQ (1/17/2008): this was implemented twice (see few lines above)!
  // DQ (1/16/2006): Added support for custom get_type() member function (returns explicitly stored type)
  // VarArgOp.setFunctionPrototype ( "HEADER_VARARG_OPERATOR", "../Grammar/Expression.code" );
  // VarArgOp.setFunctionPrototype ( "HEADER_VARARG_OPERATOR", "../Grammar/Expression.code" );

     NotOp.setFunctionPrototype    ( "HEADER_NOT_OPERATOR", "../Grammar/Expression.code" );

     VarArgEndOp.setFunctionPrototype ( "HEADER_VARARG_END_OPERATOR", "../Grammar/Expression.code" );
     PointerDerefExp.setFunctionPrototype ( "HEADER_POINTER_DEREF_EXPRESSION", "../Grammar/Expression.code" );

     AddressOfOp.setFunctionPrototype ( "HEADER_ADDRESS_OF_OPERATOR", "../Grammar/Expression.code" );

     MinusMinusOp.setFunctionPrototype ( "HEADER_MINUS_MINUS_OPERATOR", "../Grammar/Expression.code" );

     PlusPlusOp.setFunctionPrototype ( "HEADER_PLUS_PLUS_OPERATOR", "../Grammar/Expression.code" );

     BitComplementOp.setFunctionPrototype ( "HEADER_BIT_COMPLEMENT_OPERATOR", "../Grammar/Expression.code" );

     RealPartOp.setFunctionPrototype ( "HEADER_REAL_PART_OPERATOR", "../Grammar/Expression.code" );
     ImagPartOp.setFunctionPrototype ( "HEADER_IMAG_PART_OPERATOR", "../Grammar/Expression.code" );
     ConjugateOp.setFunctionPrototype ( "HEADER_CONJUGATE_OPERATOR", "../Grammar/Expression.code" );

     ConditionalExp.setFunctionPrototype ( "HEADER_CONDITIONAL_EXPRESSION", "../Grammar/Expression.code" );
     ConditionalExp.setDataPrototype ( "SgExpression*", "conditional_exp", "= NULL",
				       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     ConditionalExp.setDataPrototype ( "SgExpression*", "true_exp"       , "= NULL",
				       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     ConditionalExp.setDataPrototype ( "SgExpression*", "false_exp"      , "= NULL",
				       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  // DQ (1/14/2006): We should not store the type of unary operators but instead obtain it from the operand directly.
  // ConditionalExp.setDataPrototype ( "SgType*"      , "expression_type", "= NULL",
  //        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);
     ConditionalExp.setDataPrototype ( "SgType*"      , "expression_type", "= NULL",
            CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);

     CastExp.setFunctionPrototype ( "HEADER_CAST_EXPRESSION", "../Grammar/Expression.code" );

#if 0
  // DQ (5/20/2004): removed need_paren from this class and added it to the base class so that 
  // all expression could allow it to be set (so that we can use the value as set in EDG)!
     CastExp.setDataPrototype     ( "bool"  , "need_paren", "= true",
				    CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif
#if 0
  // DQ (6/14/2005): Modified to make enum name consistant with elsewhere in ROSE (Sage III)
     CastExp.setDataPrototype     ( "SgCastExp::Sge_cast_type", "cast_type" ,
				    "= SgCastExp::c_cast_e",
				    CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#else
     CastExp.setDataPrototype     ( "SgCastExp::cast_type_enum", "cast_type" , "= SgCastExp::e_C_style_cast",
				    CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif
  // DQ (6/19/2006): Added reference to expression tree for unfolded constant expressions, where this happens
  // in a cast it is because EDG has generated an alternative expression tree and yet holds the original one
  // (similar to the unfolded constant expression) as an alternative.  We actually want the original tree
  // in most cases and it has a SgCastExp as a root of the subexpression instead of a simple value.
  // The unfoled constant expression is also availabel from the SgValue IR node. It does not appear to
  // be required that we handle the more gneral case of adding this sort of support in the SgExpression
  // and I would like to avoid the extra memory requirement of this design, since expressions are numerous
  // within the AST and so they need to be kept as small as possible.  So we handle it in SgValue and SgCastExp 
  // explicitly instead of at the SgExpression level.
     CastExp.setDataPrototype ( "SgExpression*", "originalExpressionTree", "= NULL",
				 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     PntrArrRefExp.setFunctionPrototype ( "HEADER_POINTER_ARRAY_REFERENCE_EXPRESSION", "../Grammar/Expression.code" );

     NewExp.setFunctionPrototype ( "HEADER_NEW_OPERATOR_EXPRESSION", "../Grammar/Expression.code" );
  // DQ (1/14/2006): We should not store the type of unary operators but instead obtain it from the operand directly.
  // NewExp.setDataPrototype     ( "SgType*", "expression_type", "= NULL",
  //        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);
  // NewExp.setDataPrototype     ( "SgType*", "expression_type", "= NULL",
  //        CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);
     NewExp.setDataPrototype     ( "SgType*", "specified_type", "= NULL",
            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);
     NewExp.setDataPrototype     ( "SgExprListExp*", "placement_args", "= NULL",
				   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     NewExp.setDataPrototype     ( "SgConstructorInitializer*", "constructor_args", "= NULL",
				   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     NewExp.setDataPrototype     ( "SgExpression*", "builtin_args", "= NULL",
				   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     NewExp.setDataPrototype     ( "short", "need_global_specifier", "= 0",
				   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (8/21/2006): Added pointer to new operator being used (there could be many implemented)
  // This is NULL if the new operator is compiler generated (not explicitly declared).
     NewExp.setDataPrototype     ( "SgFunctionDeclaration*", "newOperatorDeclaration", "= NULL",
				   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     DeleteExp.setFunctionPrototype ( "HEADER_DELETE_OPERATOR_EXPRESSION", "../Grammar/Expression.code" );
     DeleteExp.setDataPrototype     ( "SgExpression*", "variable", "= NULL",
				      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     DeleteExp.setDataPrototype     ( "short", "is_array", "= 0",
				      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     DeleteExp.setDataPrototype     ( "short", "need_global_specifier", "= 0",
				      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (8/21/2006): Added pointer to delete operator being used (there could be many implemented)
  // At the moment this is always NULL, since I can't find the information in EDG!!!
     DeleteExp.setDataPrototype     ( "SgFunctionDeclaration*", "deleteOperatorDeclaration", "= NULL",
				   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     ThisExp.setFunctionPrototype ( "HEADER_THIS_EXPRESSION", "../Grammar/Expression.code" );
     ThisExp.setDataPrototype     ( "SgClassSymbol*", "class_symbol", "= NULL",
				    CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (1/14/2006): This is a CC++ specific data member, but it is part of the 
  // constructor argument list so we will remove it later.
     ThisExp.setDataPrototype     ( "int", "pobj_this", "= 0",
               CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     ScopeOp.setFunctionPrototype ( "HEADER_SCOPE_OPERATOR", "../Grammar/Expression.code" );
     AssignOp.setFunctionPrototype ( "HEADER_ASSIGNMENT_OPERATOR", "../Grammar/Expression.code" );
     PlusAssignOp.setFunctionPrototype ( "HEADER_PLUS_ASSIGNMENT_OPERATOR", "../Grammar/Expression.code" );
     MinusAssignOp.setFunctionPrototype ( "HEADER_MINUS_ASSIGNMENT_OPERATOR", "../Grammar/Expression.code" );
     AndAssignOp.setFunctionPrototype ( "HEADER_AND_ASSIGNMENT_OPERATOR", "../Grammar/Expression.code" );
     IorAssignOp.setFunctionPrototype ( "HEADER_IOR_ASSIGNMENT_OPERATOR", "../Grammar/Expression.code" );
     MultAssignOp.setFunctionPrototype ( "HEADER_MULTIPLY_ASSIGNMENT_OPERATOR", "../Grammar/Expression.code" );
     DivAssignOp.setFunctionPrototype ( "HEADER_DIVIDE_ASSIGNMENT_OPERATOR", "../Grammar/Expression.code" );
     ModAssignOp.setFunctionPrototype ( "HEADER_MOD_ASSIGNMENT_OPERATOR", "../Grammar/Expression.code" );
     XorAssignOp.setFunctionPrototype ( "HEADER_XOR_ASSIGNMENT_OPERATOR", "../Grammar/Expression.code" );
     LshiftAssignOp.setFunctionPrototype ( "HEADER_LEFT_SHIFT_ASSIGNMENT_OPERATOR", "../Grammar/Expression.code" );
     RshiftAssignOp.setFunctionPrototype ( "HEADER_RIGHT_SHIFT_ASSIGNEMENT_OPERATOR", "../Grammar/Expression.code" );
     PointerAssignOp.setFunctionPrototype ( "HEADER_POINTER_ASSIGNMENT_OPERATOR", "../Grammar/Expression.code" );

     RefExp.setFunctionPrototype ( "HEADER_REFERENCE_EXPRESSION", "../Grammar/Expression.code" );
     RefExp.setDataPrototype     ( "SgType*", "type_name", "= NULL",
				   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);

     ThrowOp.setFunctionPrototype ( "HEADER_THROW_OPERATOR", "../Grammar/Expression.code" );
  // DQ (9/19/2004): Added support for throw taking a list of type (what the C++ grammar calls a "exception-specification")
  // JJW (8/6/2008): This should never be used -- SgThrowOp is not used for exception specifications
#if 0
     ThrowOp.setDataPrototype     ( "SgTypePtrListPtr", "typeList", "= NULL",
				    CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif
  // DQ (9/19/2004): Added support for rethrow (allows unary operator operand to be NULL without being interpreted as an error)
     ThrowOp.setDataPrototype     ( "SgThrowOp::e_throw_kind", "throwKind", "= SgThrowOp::unknown_throw",
				    CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     ConcatenationOp.setFunctionPrototype ( "HEADER_CONCATENATION_OPERATOR", "../Grammar/Expression.code" );

     Initializer.setFunctionPrototype ( "HEADER_INITIALIZER_EXPRESSION", "../Grammar/Expression.code" );
     Initializer.setDataPrototype     ( "bool"               , "is_explicit_cast", "= true",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // SgAggregateInitializer.setFunctionPrototype ( "HEADER_REPLACE_EXPRESSION", "../Grammar/Expression.code" );
     AggregateInitializer.setFunctionPrototype ( "HEADER_AGGREGATE_INITIALIZER_EXPRESSION", "../Grammar/Expression.code" );

  // The the list declaration from the SgStatement.code file
     AggregateInitializer.editSubstitute       ( "HEADER_LIST_DECLARATIONS", "HEADER_LIST_FUNCTIONS", "../Grammar/Expression.code" );
     AggregateInitializer.editSubstitute       ( "LIST_NAME", "initializer" );
     AggregateInitializer.setDataPrototype     ( "SgExprListExp*", "initializers", "= NULL",
						 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AggregateInitializer.setDataPrototype     ( "SgType*", "expression_type", "= NULL",
						 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // Liao, 6/16/2009, fixing bug 355, for multidimensional array's designated initializer , 
     // aggregateInitializer should not have braces 
     AggregateInitializer.setDataPrototype     ( "bool", "need_explicit_braces", "= true",
						 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


#if 0
     // AggregateInitializer.editSubstitute       ( "HEADER_EXTRA_LIST_FUNCTIONS", "HEADER_LIST_EXPRESSION_LIST_EXPRESSION", "../Grammar/Expression.code" );
     // AggregateInitializer.editSubstitute       ( "HEADER_EXTRA_LIST_FUNCTIONS", " " );
     // AggregateInitializer.editSubstitute       ( "LIST_DATA_TYPE", "SgExpression" );
     // AggregateInitializer.editSubstitute       ( "LIST_FUNCTION_RETURN_TYPE", "void" );
     // AggregateInitializer.editSubstitute       ( "LIST_FUNCTION_NAME", "initializer" );
     // AggregateInitializer.editSubstitute       ( "LIST_ELEMENT_DATA_TYPE", "const SgExpression &" );
#endif

     ConstructorInitializer.setFunctionPrototype ( "HEADER_CONSTRUCTOR_INITIALIZER_EXPRESSION", "../Grammar/Expression.code" );
     ConstructorInitializer.setDataPrototype     ( "SgMemberFunctionDeclaration*", "declaration", "= NULL",
						   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     ConstructorInitializer.setDataPrototype     ( "SgExprListExp*"     , "args"          , "= NULL",
						   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  // DQ (8/1/2006): Store the type explicitly (from it we can still get the SgClassDeclaration, but this permits more general use of ConstructorInitializer).
  // ConstructorInitializer.setDataPrototype     ( "SgClassDeclaration*", "class_decl"    , "= NULL",
  //                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     ConstructorInitializer.setDataPrototype     ( "SgType*", "expression_type", "= NULL",
						   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (5/20/2004): removed need_paren from this class and added it to the base class so that 
  // all expression could allow it to be set (so that we can use the value as set in EDG)!
  // ConstructorInitializer.setDataPrototype     ( "int", "need_paren"    , "= false",
  //                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     ConstructorInitializer.setDataPrototype     ( "bool", "need_name"     , "= false",
						   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     ConstructorInitializer.setDataPrototype     ( "bool", "need_qualifier", "= false",
						   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (8/5/2005): Added this flag so it could be used for a different purpose (or perhaps the 
  // orginal purpose as "need_paren" if I didn't understand it intitially).  This flag controls the use of "()"
  // after a class name in a variable declaration (i.e. "class X x();" vs. "class X x;" if the
  // default constructor is explicit in the class "X" then eitheer will work, but if not the 
  // only "class X x;" will work).  This was previously controled by the get_args() function 
  // returning a NULL pointer (poorly done, since we want to avoid NULL pointers in general). 
  // Also changed types to :"bool" from "int".
     ConstructorInitializer.setDataPrototype     ( "bool", "need_parenthesis_after_name", "= false",
						   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (8/5/2005): Mark explicitly the cases where the associated class is unknown (happens in 
  // case dik_call_returning_class_via_cctor).  When associated_class_unknown == true class_decl 
  // is NULL, else class_decl should be a valid pointer!
     ConstructorInitializer.setDataPrototype     ( "bool", "associated_class_unknown", "= false",
						   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AssignInitializer.setFunctionPrototype ( "HEADER_ASSIGNMENT_INITIALIZER_EXPRESSION", "../Grammar/Expression.code" );
     AssignInitializer.setDataPrototype     ( "SgExpression*", "operand_i"      , "= NULL",
					      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  // DQ (1/14/2006): We should not store the type of unary operators but instead obtain it from the operand directly.
  // AssignInitializer.setDataPrototype     ( "SgType*"      , "expression_type", "= NULL",
  //        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);
     AssignInitializer.setDataPrototype     ( "SgType*"      , "expression_type", "= NULL",
            CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);
#if 0
  // DQ (5/20/2004): removed need_paren from this class and added it to the base class so that 
  // all expression could allow it to be set (so that we can use the value as set in EDG)!
     AssignInitializer.setDataPrototype     ( "bool"    , "need_paren"     , "= true",
					      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

     NullExpression.setFunctionPrototype    ( "HEADER_NULL_EXPRESSION", "../Grammar/Expression.code" );
  // NullExpression.setDataPrototype        ( "SgType*", "expression_type", "= NULL",
  //                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);
     VariantExpression.setFunctionPrototype ( "HEADER_VARIANT_EXPRESSION", "../Grammar/Expression.code" );
  // VariantExpression.setDataPrototype        ( "SgType*", "expression_type", "= NULL",
  //                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);

     StatementExpression.setFunctionPrototype ( "HEADER_STATEMENT_EXPRESSION", "../Grammar/Expression.code" );
     StatementExpression.setDataPrototype     ( "SgStatement*", "statement", "= NULL",
				      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmOp.setFunctionPrototype ( "HEADER_ASM_OP", "../Grammar/Expression.code" );
  // DQ (7/22/2006): This maybe should be a list of constraints (but the only examples I have have a single constraint)
#if 1
     AsmOp.setDataPrototype     ( "SgAsmOp::asm_operand_constraint_enum", "constraint", "= SgAsmOp::e_invalid",
				      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif
     AsmOp.setDataPrototype     ( "SgAsmOp::asm_operand_modifier_enum", "modifiers", "= SgAsmOp::e_unknown",
				      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmOp.setDataPrototype     ( "SgExpression*", "expression", "= NULL",
				      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, DEF_DELETE);

  // DQ (1/8/2009): Added support for asm operand handling with EDG RECORD_RAW_ASM_OPERAND_DESCRIPTIONS == TRUE
  // This allows us to handle "asm" statements that reference non-x86 specific details (registers and instructions).
     AsmOp.setDataPrototype     ( "bool", "recordRawAsmOperandDescriptions", "= false",
				      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmOp.setDataPrototype     ( "bool", "isOutputOperand", "= false",
				      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmOp.setDataPrototype     ( "std::string", "constraintString", "= \"\"",
				      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmOp.setDataPrototype     ( "std::string", "name", "= \"\"",
				      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


#if USE_FORTRAN_IR_NODES
  // DQ (11/24/2007): Removed this IR node
  // DQ (3/20/2007): Support for Fortran IR nodes.
  // IntrinsicFn.setFunctionPrototype ( "HEADER_INTRINSICFN_EXPRESSION", "../Grammar/Expression.code" );
  // IntrinsicFn.editSubstitute       ( "HEADER_LIST_DECLARATIONS", "HEADER_LIST_FUNCTIONS", "../Grammar/Expression.code" );
  // IntrinsicFn.editSubstitute       ( "LIST_NAME", "arg" );
  // IntrinsicFn.setDataPrototype ( "SgName", "name", "= \"\"",
  //         CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // IntrinsicFn.setDataPrototype ( "SgExprListExp*", "args", "= NULL",
  //         CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  // IntrinsicFn.setDataPrototype ( "SgType*"      , "return_type", "= NULL",
  //         CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL || DEF2TYPE_TRAVERSAL, NO_DELETE);

     SubscriptExpression.setFunctionPrototype ( "HEADER_SUBSCRIPT_EXPR_EXPRESSION", "../Grammar/Expression.code" );
     SubscriptExpression.setDataPrototype ( "SgExpression*", "lowerBound", "= NULL",
				          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     SubscriptExpression.setDataPrototype ( "SgExpression*", "upperBound", "= NULL",
				          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     SubscriptExpression.setDataPrototype ( "SgExpression*", "stride", "= NULL",
				          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (11/24/2007): Removed this IR node (equivalent to default setting of SgSubscriptExpression)
  // SubscriptColon.setFunctionPrototype ( "HEADER_SUBSCRIPT_COLON_EXPRESSION", "../Grammar/Expression.code" );

  // DQ (11/18/2007): These data members are not used!
  // SubscriptColon.setDataPrototype ( "SgExpression*", "lowerBound", "= NULL",
  //                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  // DQ (11/18/2007): These data members are not used!
  // SubscriptColon.setDataPrototype ( "SgExpression*", "stride", "= NULL",
  //                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (11/18/2007): This IR node is used in declarations!
  // Colon.setFunctionPrototype ( "HEADER_COLON_EXPRESSION", "../Grammar/Expression.code" );
     ColonShapeExp.setFunctionPrototype ( "HEADER_COLON_SHAPE_EXPRESSION", "../Grammar/Expression.code" );
  // Colon.setDataPrototype ( "SgExpression*", "lowerBound", "= NULL",
  //                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  // Colon.setDataPrototype ( "SgExpression*", "stride", "= NULL",
  //                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // SubscriptAsterisk.setFunctionPrototype ( "HEADER_SUBSCRIPT_ASTERISK_EXPRESSION", "../Grammar/Expression.code" );
     AsteriskShapeExp.setFunctionPrototype  ( "HEADER_ASTERISK_SHAPE_EXPRESSION", "../Grammar/Expression.code" );
  // DQ (11/18/2007): These data members are not used!
  // SubscriptAsterisk.setDataPrototype ( "SgExpression*", "lowerBound", "= NULL",
  //                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  // DQ (11/18/2007): These data members are not used!
  // SubscriptAsterisk.setDataPrototype ( "SgExpression*", "stride", "= NULL",
  //                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

#if 0
  // DQ (10/4/2008): I no longer agree that these are expressions, they are just parts of the SgUseStatment.
     UseOnlyExpression.setFunctionPrototype ( "HEADER_USE_ONLY_EXPR", "../Grammar/Expression.code" );
     UseOnlyExpression.setDataPrototype ( "SgExprListExp*", "access_list", "= NULL",
                                         CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     UseRenameExpression.setFunctionPrototype ( "HEADER_USE_RENAME_EXPR", "../Grammar/Expression.code" );
     UseRenameExpression.setDataPrototype ( "SgExpression*", "oldname", "= NULL",
                                         CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     UseRenameExpression.setDataPrototype ( "SgExpression*", "newname", "= NULL",
                                         CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

     IOItemExpression.setFunctionPrototype ( "HEADER_IO_ITEM_EXPR", "../Grammar/Expression.code" );
     IOItemExpression.setDataPrototype ( "SgName", "name", "= \"\"",
                                         CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     IOItemExpression.setDataPrototype ( "SgExpression*", "io_item", "= NULL",
                                         CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     ImpliedDo.setFunctionPrototype ( "HEADER_IMPLIED_DO", "../Grammar/Expression.code" );

  // DQ (9/22/2010): This is only an simple varRef in trivial cases, this is more generally 
  // where an expression in terms of an index is put (e.g. a function of an index as in 
  // "(product(localCount(:j)), j = 1, numDims - 1)" in test2010_49.f90).
  // ImpliedDo.setDataPrototype     ( "SgVarRefExp*", "do_var", "= NULL",
  //                                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  // ImpliedDo.setDataPrototype     ( "SgExpression*", "do_var_exp", "= NULL",
  //                                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     ImpliedDo.setDataPrototype     ( "SgExpression*", "do_var_initialization", "= NULL",
                                         CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (10/9/2010): This data member is now replaced by the do_var_exp_initialization.
  // DQ (10/2/2010): This should be a SgVariableDeclaration instead of an expression.  
  // This was suggested at the Sept Portlan Fortran Adventure meeting.
  // ImpliedDo.setDataPrototype     ( "SgExpression*", "first_val", "= NULL",
  //              CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  // ImpliedDo.setDataPrototype     ( "SgExpression*", "first_val", "= NULL",
  //              CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     ImpliedDo.setDataPrototype     ( "SgExpression*", "last_val", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     ImpliedDo.setDataPrototype     ( "SgExpression*", "increment", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  // For "((A(i),B(i,j),i=0,10,2),j=0,20,3)" A(i) and B(i,j) are the objects in the object_list 
  // for the inner most implided do loop.  The inner most implied do loop is in the object list 
  // for the outer implied do loop.
     ImpliedDo.setDataPrototype     ( "SgExprListExp*", "object_list", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#if 1
  // DQ (10/2/2010): Added scope to hold the SgVariableDeclaration and support nested scopes of 
  // multi-dimensional implied do loops. However this scope should not be traversed since its
  // purpose is to hold a symbol table, but we don't want just have a symbol table directly.
     ImpliedDo.setDataPrototype     ( "SgScopeStatement*", "implied_do_scope", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif
  // DQ (11/24/2007): Added new IR node to handle unknown array reference vs. function call problem in Fortran.
  // These are translated to either array references or function calls within post-processing steps.
     UnknownArrayOrFunctionReference.setFunctionPrototype ( "HEADER_UNKNOWN_ARRAY_OR_FUNCTION_REFERENCE", "../Grammar/Expression.code" );

  // This is the name of the array variable being referenced or the name of the function (to be decided in post-processing).
     UnknownArrayOrFunctionReference.setDataPrototype ( "std::string", "name", "= \"\"",
           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // This is the variable reference that was constucted already (will be ignored if we convert this to a function call in post-processing).
     UnknownArrayOrFunctionReference.setDataPrototype ( "SgExpression*", "named_reference", "= NULL",
           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // This is either a subscript list or a function argument list (to be decided in post-processing).
     UnknownArrayOrFunctionReference.setDataPrototype ( "SgExprListExp*", "expression_list", "= NULL",
           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     PseudoDestructorRefExp.setFunctionPrototype ( "HEADER_PSEUDO_DESTRUCTOR_REF", "../Grammar/Expression.code" );
     PseudoDestructorRefExp.setDataPrototype ( "SgType*", "object_type", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     PseudoDestructorRefExp.setDataPrototype ( "SgType*", "expression_type", "= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (12/31/2007): Support for named actual arguments to functions (fortran specific).
     ActualArgumentExpression.setFunctionPrototype ( "HEADER_ACTUAL_ARGUMENT_EXPRESSION", "../Grammar/Expression.code" );
     ActualArgumentExpression.setDataPrototype     ( "SgName", "argument_name", "= \"\"",
				      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     ActualArgumentExpression.setDataPrototype     ( "SgExpression*", "expression", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     UserDefinedUnaryOp.setFunctionPrototype ( "HEADER_USER_DEFINED_UNARY_EXPRESSION", "../Grammar/Expression.code" );
     UserDefinedUnaryOp.setDataPrototype     ( "SgName", "operator_name", "= \"\"",
				      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UserDefinedUnaryOp.setDataPrototype ( "SgFunctionSymbol*", "symbol"     , "= NULL",
				       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     UserDefinedBinaryOp.setFunctionPrototype ( "HEADER_USER_DEFINED_BINARY_EXPRESSION", "../Grammar/Expression.code" );
     UserDefinedBinaryOp.setDataPrototype     ( "SgName", "operator_name", "= \"\"",
				      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UserDefinedBinaryOp.setDataPrototype ( "SgFunctionSymbol*", "symbol"     , "= NULL",
				       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


     //FMZ (2/5/2009): Added for CAFCoExpression => change teamId from "SgName" to "SgVarRefExp*"
     CAFCoExpression.setFunctionPrototype ( "HEADER_CO_EXPRESSION", "../Grammar/Expression.code" );

     //CAFCoExpression.setDataPrototype ( "SgName", "teamId",  "= \"\"", 
     //				          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,NO_TRAVERSAL,NO_DELETE);
     CAFCoExpression.setDataPrototype ( "SgVarRefExp*", "teamId",  "= NULL", 
                                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     CAFCoExpression.setDataPrototype ( "SgExpression*", "teamRank", "= NULL",
				          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     CAFCoExpression.setDataPrototype ( "SgExpression*", "referData", "= NULL",
				          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,DEF_TRAVERSAL, NO_DELETE);
     CAFCoExpression.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", 
                                  "../Grammar/Expression.code" );


#endif

     DesignatedInitializer.setFunctionPrototype ( "HEADER_DESIGNATED_INITIALIZER", "../Grammar/Expression.code" );
  // Each of these fields is either a SgValueExp for an array index or an SgVarRefExp for a struct field name -- they are chained to form the actual designator
     DesignatedInitializer.setDataPrototype("SgExprListExp*", "designatorList", "= NULL", CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     DesignatedInitializer.setDataPrototype("SgInitializer*", "memberInit", "= NULL", CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);


 // TV (04/22/2010): CUDA support
 
     CudaKernelExecConfig.setFunctionPrototype ( "HEADER_CUDA_KERNEL_EXEC_CONFIG", "../Grammar/Expression.code" );
     
     CudaKernelExecConfig.setDataPrototype ( "SgExpression*", "grid",   "= NULL", CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     CudaKernelExecConfig.setDataPrototype ( "SgExpression*", "blocks", "= NULL", CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     CudaKernelExecConfig.setDataPrototype ( "SgExpression*", "shared", "= NULL", CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     CudaKernelExecConfig.setDataPrototype ( "SgExpression*", "stream", "= NULL", CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     CudaKernelCallExp.setFunctionPrototype ( "HEADER_CUDA_KERNEL_CALL_EXPRESSION", "../Grammar/Expression.code" );
     
     CudaKernelCallExp.editSubstitute       ( "HEADER_LIST_DECLARATIONS", "HEADER_LIST_FUNCTIONS", "../Grammar/Expression.code" );
     CudaKernelCallExp.editSubstitute       ( "LIST_NAME", "arg" );
  
     CudaKernelCallExp.setDataPrototype ( "SgExpression*", "function", "= NULL", CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     CudaKernelCallExp.setDataPrototype ( "SgExprListExp*", "args", "= NULL", CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     CudaKernelCallExp.setDataPrototype ( "SgCudaKernelExecConfig*", "exec_config", "= NULL", CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     // ***********************************************************************
     // ***********************************************************************
     //                       Source Code Declaration
     // ***********************************************************************
     // ***********************************************************************

     // ############################
     // Functions assigned by object
     // ############################

     Expression.setFunctionSource ( "SOURCE_BASECLASS_EXPRESSION", "../Grammar/Expression.code" );

     UnaryOp.setFunctionSource  ( "SOURCE_UNARY_EXPRESSION", "../Grammar/Expression.code" );
     BinaryOp.setFunctionSource ( "SOURCE_BINARY_EXPRESSION", "../Grammar/Expression.code" );

     ExpressionRoot.setFunctionSource         ( "SOURCE_EXPRESSION_ROOT","../Grammar/Expression.code" );

#ifdef HL_GRAMMARS
     X_ExpressionUnknown.setFunctionSource   ( "SOURCE_X_EXPRESSION_UNKNOWN", "../Grammar/Expression.code" );
     X_Expression.setFunctionSource          ( "SOURCE_X_EXPRESSION",         "../Grammar/Expression.code" );
     non_X_Expression.setFunctionSource      ( "SOURCE_NON_X_EXPRESSION",     "../Grammar/Expression.code" );
#endif

     ExprListExp.setFunctionSource ( "SOURCE_EXPRESSION_LIST_EXPRESSION","../Grammar/Expression.code" );
     VarRefExp.setFunctionSource ( "SOURCE_VARIABLE_REFERENCE_EXPRESSION","../Grammar/Expression.code" );
     LabelRefExp.setFunctionSource ( "SOURCE_LABEL_REFERENCE_EXPRESSION","../Grammar/Expression.code" );
     ClassNameRefExp.setFunctionSource ( "SOURCE_CLASS_NAME_REFERENCE_EXPRESSION","../Grammar/Expression.code" );
     FunctionRefExp.setFunctionSource ( "SOURCE_FUNCTION_REFERENCE_EXPRESSION","../Grammar/Expression.code" );
     MemberFunctionRefExp.setFunctionSource ( "SOURCE_MEMBER_FUNCTION_REFERENCE_EXPRESSION","../Grammar/Expression.code" );
     ValueExp.setFunctionSource ( "SOURCE_VALUE_EXPRESSION","../Grammar/Expression.code" );
     BoolValExp.setFunctionSource ( "SOURCE_BOOLEAN_VALUE_EXPRESSION","../Grammar/Expression.code" );
     StringVal.setFunctionSource ( "SOURCE_STRING_VALUE_EXPRESSION","../Grammar/Expression.code" );
     ShortVal.setFunctionSource ( "SOURCE_SHORT_VALUE_EXPRESSION","../Grammar/Expression.code" );
     CharVal.setFunctionSource ( "SOURCE_CHAR_VALUE_EXPRESSION","../Grammar/Expression.code" );
     UnsignedCharVal.setFunctionSource ( "SOURCE_UNSIGNED_CHAR_VALUE_EXPRESSION","../Grammar/Expression.code" );
     WcharVal.setFunctionSource ( "SOURCE_WCHAR_VALUE_EXPRESSION","../Grammar/Expression.code" );
     UnsignedShortVal.setFunctionSource ( "SOURCE_UNSIGNED_SHORT_VALUE_EXPRESSION","../Grammar/Expression.code" );
     IntVal.setFunctionSource ( "SOURCE_INTEGER_VALUE_EXPRESSION","../Grammar/Expression.code" );
     EnumVal.setFunctionSource ( "SOURCE_ENUM_VALUE_EXPRESSION","../Grammar/Expression.code" );
     UnsignedIntVal.setFunctionSource ( "SOURCE_UNSIGNED_INTEGER_VALUE_EXPRESSION","../Grammar/Expression.code" );
     LongIntVal.setFunctionSource ( "SOURCE_LONG_INTEGER_VALUE_EXPRESSION","../Grammar/Expression.code" );
     LongLongIntVal.setFunctionSource ( "SOURCE_LONG_LONG_INTEGER_VALUE_EXPRESSION","../Grammar/Expression.code" );
     UnsignedLongLongIntVal.setFunctionSource ( "SOURCE_UNSIGNED_LONG_LONG_INTEGER_VALUE_EXPRESSION","../Grammar/Expression.code" );
     UnsignedLongVal.setFunctionSource ( "SOURCE_UNSIGNED_LONG_VALUE_EXPRESSION","../Grammar/Expression.code" );
     FloatVal.setFunctionSource ( "SOURCE_FLOAT_VALUE_EXPRESSION","../Grammar/Expression.code" );
     DoubleVal.setFunctionSource ( "SOURCE_DOUBLE_VALUE_EXPRESSION","../Grammar/Expression.code" );
     LongDoubleVal.setFunctionSource ( "SOURCE_LONG_DOUBLE_VALUE_EXPRESSION","../Grammar/Expression.code" );
     ComplexVal.setFunctionSource ( "SOURCE_COMPLEX_VALUE_EXPRESSION","../Grammar/Expression.code" );
     FunctionCallExp.setFunctionSource ( "SOURCE_FUNCTION_CALL_EXPRESSION","../Grammar/Expression.code" );
     ArrowExp.setFunctionSource ( "SOURCE_ARROW_EXPRESSION","../Grammar/Expression.code" );
     DotExp.setFunctionSource ( "SOURCE_DOT_EXPRESSION","../Grammar/Expression.code" );
     DotStarOp.setFunctionSource ( "SOURCE_DOT_STAR_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     ArrowStarOp.setFunctionSource ( "SOURCE_ARROW_STAR_OPERATOR_EXPRESSION","../Grammar/Expression.code" );

     UpcThreads.setFunctionSource ( "SOURCE_UPC_THREADS_EXPRESSION","../Grammar/Expression.code" );
     UpcMythread.setFunctionSource ( "SOURCE_UPC_MYTHREAD_EXPRESSION","../Grammar/Expression.code" );

     EqualityOp.setFunctionSource       ( "SOURCE_EQUALITY_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     LessThanOp.setFunctionSource       ( "SOURCE_LESS_THAN_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     GreaterThanOp.setFunctionSource    ( "SOURCE_GREATER_THAN_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     NotEqualOp.setFunctionSource       ( "SOURCE_NOT_EQUAL_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     LessOrEqualOp.setFunctionSource    ( "SOURCE_LESS_OR_EQUAL_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     GreaterOrEqualOp.setFunctionSource ( "SOURCE_GREATER_OR_EQUAL_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     NotOp.setFunctionSource            ( "SOURCE_NOT_OPERATOR_EXPRESSION","../Grammar/Expression.code" );

  // DQ (6/20/2006): Relational Operators must return bool type
     EqualityOp.editSubstitute       ( "SOURCE_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "SOURCE_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );
     LessThanOp.editSubstitute       ( "SOURCE_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "SOURCE_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );
     GreaterThanOp.editSubstitute    ( "SOURCE_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "SOURCE_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );
     NotEqualOp.editSubstitute       ( "SOURCE_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "SOURCE_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );
     LessOrEqualOp.editSubstitute    ( "SOURCE_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "SOURCE_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );
     GreaterOrEqualOp.editSubstitute ( "SOURCE_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "SOURCE_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );
     NotOp.editSubstitute            ( "SOURCE_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "SOURCE_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );

     AddOp.setFunctionSource ( "SOURCE_ADD_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     SubtractOp.setFunctionSource ( "SOURCE_SUBTRACT_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     MultiplyOp.setFunctionSource ( "SOURCE_MULTIPLY_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     DivideOp.setFunctionSource ( "SOURCE_DIVIDE_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     IntegerDivideOp.setFunctionSource ( "SOURCE_INTEGER_DIVIDE_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     ModOp.setFunctionSource ( "SOURCE_MOD_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     AndOp.setFunctionSource ( "SOURCE_AND_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     OrOp.setFunctionSource ( "SOURCE_OR_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     BitXorOp.setFunctionSource ( "SOURCE_BIT_XOR_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     BitAndOp.setFunctionSource ( "SOURCE_BIT_AND_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     BitOrOp.setFunctionSource ( "SOURCE_BIT_OR_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     CommaOpExp.setFunctionSource ( "SOURCE_COMMA_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     LshiftOp.setFunctionSource ( "SOURCE_LEFT_SHIFT_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     RshiftOp.setFunctionSource ( "SOURCE_RIGHT_SHIFT_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     MinusOp.setFunctionSource ( "SOURCE_MINUS_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     UnaryAddOp.setFunctionSource ( "SOURCE_UNARY_ADD_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     SizeOfOp.setFunctionSource ( "SOURCE_SIZE_OF_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     TypeIdOp.setFunctionSource ( "SOURCE_TYPE_ID_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     PointerDerefExp.setFunctionSource ( "SOURCE_POINTER_DEREFERENCE_EXPRESSION","../Grammar/Expression.code" );
     AddressOfOp.setFunctionSource     ( "SOURCE_ADDRESS_OF_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     MinusMinusOp.setFunctionSource ( "SOURCE_MINUS_MINUS_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     PlusPlusOp.setFunctionSource ( "SOURCE_PLUS_PLUS_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     BitComplementOp.setFunctionSource ( "SOURCE_BIT_COMPLEMENT_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     RealPartOp.setFunctionSource ( "SOURCE_REAL_PART_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     ImagPartOp.setFunctionSource ( "SOURCE_IMAG_PART_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     ConjugateOp.setFunctionSource ( "SOURCE_CONJUGATE_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     ConditionalExp.setFunctionSource ( "SOURCE_CONDITIONAL_EXPRESSION","../Grammar/Expression.code" );
     CastExp.setFunctionSource ( "SOURCE_CAST_EXPRESSION","../Grammar/Expression.code" );
     PntrArrRefExp.setFunctionSource ( "SOURCE_POINTER_ARRAY_REFERENCE_EXPRESSION","../Grammar/Expression.code" );
     NewExp.setFunctionSource ( "SOURCE_NEW_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     DeleteExp.setFunctionSource ( "SOURCE_DELETE_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     ThisExp.setFunctionSource ( "SOURCE_THIS_EXPRESSION","../Grammar/Expression.code" );
     ScopeOp.setFunctionSource ( "SOURCE_SCOPE_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     AssignOp.setFunctionSource  ( "SOURCE_ASSIGN_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
  // This must be a bug since it is different from the MinusAssignOp!
     PlusAssignOp.setFunctionSource  ( "SOURCE_PLUS_ASSIGN_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     MinusAssignOp.setFunctionSource ( "SOURCE_MINUS_ASSIGN_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     AndAssignOp.setFunctionSource ( "SOURCE_AND_ASSIGN_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     IorAssignOp.setFunctionSource ( "SOURCE_IOR_ASSIGN_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     MultAssignOp.setFunctionSource ( "SOURCE_MULTIPLY_ASSIGN_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     DivAssignOp.setFunctionSource ( "SOURCE_DIVIDE_ASSIGN_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     ModAssignOp.setFunctionSource ( "SOURCE_MOD_ASSIGN_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     XorAssignOp.setFunctionSource ( "SOURCE_XOR_ASSIGN_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     LshiftAssignOp.setFunctionSource ( "SOURCE_LEFT_SHIFT_ASSIGN_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     RshiftAssignOp.setFunctionSource ( "SOURCE_RIGHT_SHIFT_ASSIGN_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     PointerAssignOp.setFunctionSource  ( "SOURCE_POINTER_ASSIGN_OPERATOR_EXPRESSION","../Grammar/Expression.code" );

     RefExp.setFunctionSource ( "SOURCE_REFERENCE_EXPRESSION","../Grammar/Expression.code" );
     ThrowOp.setFunctionSource ( "SOURCE_THROW_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     Initializer.setFunctionSource ( "SOURCE_INITIALIZER_EXPRESSION","../Grammar/Expression.code" );
     AggregateInitializer.setFunctionSource ( "SOURCE_AGGREGATE_INITIALIZER_EXPRESSION","../Grammar/Expression.code" );
     ConstructorInitializer.setFunctionSource ( "SOURCE_CONSTRUCTOR_INITIALIZER_EXPRESSION","../Grammar/Expression.code" );
     AssignInitializer.setFunctionSource ( "SOURCE_ASSIGNMENT_INITIALIZER_EXPRESSION","../Grammar/Expression.code" );

     ConcatenationOp.setFunctionSource  ( "SOURCE_CONCATENATION_OPERATOR_EXPRESSION","../Grammar/Expression.code" );


     // ###################################
     // Functions assigned by function name
     // ###################################

     ArrowStarOp.setFunctionSource       ( "SOURCE_ARROW_STAR_OPERATOR_EXPRESSION","../Grammar/Expression.code" );

     AddOp.setFunctionSource           ( "SOURCE_ARITHMETIC_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     SubtractOp.setFunctionSource      ( "SOURCE_ARITHMETIC_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     MultiplyOp.setFunctionSource      ( "SOURCE_ARITHMETIC_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     DivideOp.setFunctionSource        ( "SOURCE_ARITHMETIC_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     IntegerDivideOp.setFunctionSource ( "SOURCE_ARITHMETIC_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     ModOp.setFunctionSource           ( "SOURCE_ARITHMETIC_OPERATOR_EXPRESSION","../Grammar/Expression.code" );

     AndOp.setFunctionSource ( "SOURCE_INTEGER_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     OrOp.setFunctionSource  ( "SOURCE_INTEGER_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     NotOp.setFunctionSource ( "SOURCE_INTEGER_OPERATOR_EXPRESSION","../Grammar/Expression.code" );

     BitXorOp.setFunctionSource ( "SOURCE_BIT_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     BitAndOp.setFunctionSource ( "SOURCE_BIT_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
     BitOrOp.setFunctionSource  ( "SOURCE_BIT_OPERATOR_EXPRESSION","../Grammar/Expression.code" );

     CommaOpExp.setFunctionSource ( "SOURCE_COMMA_OPERATOR_EXPRESSION","../Grammar/Expression.code" );

     ExponentiationOp.setFunctionSource    ( "SOURCE_EXPONENTIATION_EXPRESSION", "../Grammar/Expression.code" );

  // IntrinsicFn.setFunctionSource ( "SOURCE_INTRINSICFN_EXPRESSION", "../Grammar/Expression.code" );
  // IntrinsicFn.setFunctionSource ( "SOURCE_EMPTY_SET_TYPE_FUNCTION", "../Grammar/Expression.code" );

     SubscriptExpression.setFunctionSource ( "SOURCE_SUBSCRIPT_EXPR_EXPRESSION", "../Grammar/Expression.code" );
  // SubscriptExpression.setFunctionSource ( "SOURCE_EMPTY_SET_TYPE_FUNCTION", "../Grammar/Expression.code" );

  // SubscriptColon.setFunctionSource ( "SOURCE_SUBSCRIPT_COLON_EXPRESSION", "../Grammar/Expression.code" );
  // SubscriptColon.setFunctionSource ( "SOURCE_EMPTY_SET_TYPE_FUNCTION", "../Grammar/Expression.code" );

  // Colon.setFunctionSource ( "SOURCE_COLON_EXPRESSION", "../Grammar/Expression.code" );
     ColonShapeExp.setFunctionSource ( "SOURCE_COLON_SHAPE_EXPRESSION", "../Grammar/Expression.code" );
  // Colon.setFunctionSource ( "SOURCE_EMPTY_SET_TYPE_FUNCTION", "../Grammar/Expression.code" );

  // SubscriptAsterisk.setFunctionSource ( "SOURCE_SUBSCRIPT_ASTERISK_EXPRESSION", "../Grammar/Expression.code" );
     AsteriskShapeExp.setFunctionSource ( "SOURCE_ASTERISK_SHAPE_EXPRESSION", "../Grammar/Expression.code" );
  // SubscriptAsterisk.setFunctionSource ( "SOURCE_EMPTY_SET_TYPE_FUNCTION", "../Grammar/Expression.code" );

#if 0
  // DQ (10/4/2008): I no longer agree that these are expressions, they are just parts of the SgUseStatment.
     UseOnlyExpression.setFunctionSource ( "SOURCE_USE_ONLY_EXPR", "../Grammar/Expression.code" );
  // UseOnlyExpression.setFunctionSource ( "SOURCE_EMPTY_SET_TYPE_FUNCTION", "../Grammar/Expression.code" );

     UseRenameExpression.setFunctionSource ( "SOURCE_USE_RENAME_EXPR", "../Grammar/Expression.code" );
  // UseRenameExpression.setFunctionSource ( "SOURCE_EMPTY_SET_TYPE_FUNCTION", "../Grammar/Expression.code" );
#endif

     IOItemExpression.setFunctionSource ( "SOURCE_IO_ITEM_EXPR", "../Grammar/Expression.code" );
  // IOItemExpression.setFunctionSource ( "SOURCE_EMPTY_SET_TYPE_FUNCTION", "../Grammar/Expression.code" );

     ImpliedDo.setFunctionSource ( "SOURCE_IMPLIED_DO", "../Grammar/Expression.code" );
  // IOImpliedDo.setFunctionSource ( "SOURCE_EMPTY_SET_TYPE_FUNCTION", "../Grammar/Expression.code" );

     UnknownArrayOrFunctionReference.setFunctionSource ( "SOURCE_UNKNOWN_ARRAY_OR_FUNCTION_REFERENCE", "../Grammar/Expression.code" );

     ActualArgumentExpression.setFunctionSource ( "SOURCE_ACTUAL_ARGUMENT_EXPRESSION", "../Grammar/Expression.code" );
     DesignatedInitializer.setFunctionSource ( "SOURCE_DESIGNATED_INITIALIZER", "../Grammar/Expression.code" );

     //FMZ (2/6/2009): Added for CoArray Reference
     CAFCoExpression.setFunctionSource ( "SOURCE_CO_EXPRESSION", "../Grammar/Expression.code" );

#if USE_UPC_IR_NODES
  // DQ and Liao (6/10/2008): Added new IR nodes specific to UPC.
     UpcLocalsizeofExpression.setFunctionSource ( "SOURCE_UPC_LOCAL_SIZEOF_EXPRESSION", "../Grammar/Expression.code" );
     UpcBlocksizeofExpression.setFunctionSource ( "SOURCE_UPC_BLOCK_SIZEOF_EXPRESSION", "../Grammar/Expression.code" );
     UpcElemsizeofExpression.setFunctionSource  ( "SOURCE_UPC_ELEM_SIZEOF_EXPRESSION",  "../Grammar/Expression.code" );
#endif

     UserDefinedUnaryOp.setFunctionSource  ( "SOURCE_USER_DEFINED_UNARY_EXPRESSION",  "../Grammar/Expression.code" );
     UserDefinedBinaryOp.setFunctionSource ( "SOURCE_USER_DEFINED_BINARY_EXPRESSION", "../Grammar/Expression.code" );

  // ***************************************
  //      get_type() member function
  // ***************************************

     ExprListExp.setFunctionSource            ( "SOURCE_DEFAULT_GET_TYPE","../Grammar/Expression.code" );
     VarRefExp.setFunctionSource              ( "SOURCE_GET_TYPE_FROM_SYMBOL","../Grammar/Expression.code" );
     LabelRefExp.setFunctionSource            ( "SOURCE_GET_TYPE_FROM_SYMBOL","../Grammar/Expression.code" );
     FunctionRefExp.setFunctionSource         ( "SOURCE_GET_TYPE_FROM_SYMBOL","../Grammar/Expression.code" );
     MemberFunctionRefExp.setFunctionSource   ( "SOURCE_GET_TYPE_FROM_SYMBOL","../Grammar/Expression.code" );

     BoolValExp.setFunctionSource             ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
     ShortVal.setFunctionSource               ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );

  // DQ (8/17/2010): types for strings need to be handled using a lenght parameter to the SgTypeString::createType function.
  // For fortran the lenght can be specified as an expression, but for a literal it has to be a known value of an integer.
  // StringVal.setFunctionSource              ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
     StringVal.setFunctionSource              ( "SOURCE_GET_TYPE_STRING","../Grammar/Expression.code" );

     CharVal.setFunctionSource                ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
     UnsignedCharVal.setFunctionSource        ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
     WcharVal.setFunctionSource               ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
     UnsignedShortVal.setFunctionSource       ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
     IntVal.setFunctionSource                 ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
     EnumVal.setFunctionSource                ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
     UnsignedIntVal.setFunctionSource         ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
     LongIntVal.setFunctionSource             ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
     LongLongIntVal.setFunctionSource         ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
     UnsignedLongLongIntVal.setFunctionSource ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
     UnsignedLongVal.setFunctionSource        ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
     FloatVal.setFunctionSource               ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
     DoubleVal.setFunctionSource              ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
     LongDoubleVal.setFunctionSource          ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
     ComplexVal.setFunctionSource             ( "SOURCE_GET_TYPE_COMPLEX","../Grammar/Expression.code" );

     UpcThreads.setFunctionSource             ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
     UpcMythread.setFunctionSource            ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );

     BoolValExp.editSubstitute     ( "GENERIC_TYPE", "SgTypeBool" );
     StringVal.editSubstitute      ( "GENERIC_TYPE", "SgTypeString" );
     ShortVal.editSubstitute       ( "GENERIC_TYPE", "SgTypeShort" );

     CharVal.editSubstitute        ( "GENERIC_TYPE", "SgTypeChar" );

     UnsignedCharVal.editSubstitute        ( "GENERIC_TYPE", "SgTypeUnsignedChar" );
     WcharVal.editSubstitute               ( "GENERIC_TYPE", "SgTypeWchar" );
     UnsignedShortVal.editSubstitute       ( "GENERIC_TYPE", "SgTypeUnsignedShort" );
     IntVal.editSubstitute                 ( "GENERIC_TYPE", "SgTypeInt" );

  // Shouldn't this be using the TypeEnum?
     EnumVal.editSubstitute                ( "GENERIC_TYPE", "SgTypeInt" );
     UnsignedIntVal.editSubstitute         ( "GENERIC_TYPE", "SgTypeUnsignedInt" );
     LongIntVal.editSubstitute             ( "GENERIC_TYPE", "SgTypeLong" );

  // LongLongIntVal should return a value of type TypeLongLong (I think!)
     LongLongIntVal.editSubstitute         ( "GENERIC_TYPE", "SgTypeLong" );

     UnsignedLongLongIntVal.editSubstitute ( "GENERIC_TYPE", "SgTypeUnsignedLongLong" );
     UnsignedLongVal.editSubstitute        ( "GENERIC_TYPE", "SgTypeUnsignedLong" );
     FloatVal.editSubstitute               ( "GENERIC_TYPE", "SgTypeFloat" );
     DoubleVal.editSubstitute              ( "GENERIC_TYPE", "SgTypeDouble" );
     LongDoubleVal.editSubstitute          ( "GENERIC_TYPE", "SgTypeLongDouble" );
     ComplexVal.editSubstitute             ( "GENERIC_TYPE", "SgTypeComplex" );

     UpcThreads.editSubstitute             ( "GENERIC_TYPE", "SgTypeInt" );
     UpcMythread.editSubstitute            ( "GENERIC_TYPE", "SgTypeInt" );

  // DQ (1/16/2006): This is not IR node specific code since we don't store the type explicitly
  // FunctionCallExp.setFunctionSource     ( "SOURCE_GET_TYPE_CALLING_GET_EXPRESSION_TYPE_EXPRESSION",
  //                                              "../Grammar/Expression.code" );

  // SizeOfOp.setFunctionSource            ( "SOURCE_GET_TYPE_CALLING_GET_EXPRESSION_TYPE_EXPRESSION",
     VarArgStartOp.setFunctionSource            ( "SOURCE_VARARG_START_OPERATOR","../Grammar/Expression.code" );
     VarArgEndOp.setFunctionSource            ( "SOURCE_VARARG_END_OPERATOR","../Grammar/Expression.code" );
  //                                              "../Grammar/Expression.code" );
  // TypeIdOp.setFunctionSource            ( "SOURCE_GET_TYPE_CALLING_GET_EXPRESSION_TYPE_EXPRESSION",
  //                                              "../Grammar/Expression.code" );

  // DQ (2/5/2004): Adding vararg support to SAGE 
     VarArgStartOp.setFunctionSource       ( "SOURCE_DEFAULT_GET_TYPE","../Grammar/Expression.code" );
     VarArgStartOneOperandOp.setFunctionSource ( "SOURCE_DEFAULT_GET_TYPE","../Grammar/Expression.code" );
  // VarArgOp.setFunctionSource            ( "SOURCE_DEFAULT_GET_TYPE",
  //                                              "../Grammar/Expression.code" );
     VarArgOp.setFunctionSource            ( "SOURCE_VARARG_OPERATOR","../Grammar/Expression.code" );
     VarArgStartOp.setFunctionSource            ( "SOURCE_VARARG_START_OPERATOR","../Grammar/Expression.code" );
     VarArgEndOp.setFunctionSource            ( "SOURCE_VARARG_END_OPERATOR","../Grammar/Expression.code" );
     VarArgEndOp.setFunctionSource         ( "SOURCE_DEFAULT_GET_TYPE","../Grammar/Expression.code" );
     VarArgCopyOp.setFunctionSource        ( "SOURCE_DEFAULT_GET_TYPE","../Grammar/Expression.code" );

  // ConditionalExp.setFunctionSource      ( "SOURCE_GET_TYPE_CALLING_GET_EXPRESSION_TYPE_EXPRESSION",
  //                                              "../Grammar/Expression.code" );
  // NewExp.setFunctionSource              ( "SOURCE_GET_TYPE_CALLING_GET_EXPRESSION_TYPE_EXPRESSION",
  //                                              "../Grammar/Expression.code" );

  // DQ (1/16/2006): I think this should be void instead of generic
  // DeleteExp.setFunctionSource           ( "SOURCE_GET_TYPE_GENERIC",
  //                                              "../Grammar/Expression.code" );
  // DeleteExp.editSubstitute              ( "GENERIC_TYPE", "SgTypeVoid" );

     ThisExp.setFunctionSource             ( "SOURCE_GET_TYPE_THIS_EXPRESSION","../Grammar/Expression.code" );

  // AssignInitializer.setFunctionSource   ( "SOURCE_GET_TYPE_CALLING_GET_EXPRESSION_TYPE_EXPRESSION",
  //                                              "../Grammar/Expression.code" );

  // DQ (7/19/2006): Suggested change by Jeremiah Willcock
  // ConstructorInitializer.setFunctionSource ( "SOURCE_GET_TYPE_GENERIC", "../Grammar/Expression.code" );
  // ConstructorInitializer.editSubstitute    ( "GENERIC_TYPE", "SgTypeDefault" );
     ConstructorInitializer.setFunctionSource ( "SOURCE_GET_TYPE_CLASS_DECL", "../Grammar/Expression.code" );

     NullExpression.setFunctionSource    ( "SOURCE_NULL_EXPRESSION", "../Grammar/Expression.code" );
  // NullExpression.setFunctionSource    ( "SOURCE_DEFAULT_GET_TYPE", "../Grammar/Expression.code" );
  // NullExpression.setFunctionSource    ( "SOURCE_EMPTY_SET_TYPE_FUNCTION", "../Grammar/Expression.code" );
     NullExpression.setFunctionSource    ( "SOURCE_GET_TYPE_GENERIC", "../Grammar/Expression.code" );
     NullExpression.editSubstitute       ( "GENERIC_TYPE", "SgTypeDefault" );

     VariantExpression.setFunctionSource ( "SOURCE_VARIANT_EXPRESSION", "../Grammar/Expression.code" );
  // VariantExpression.setFunctionSource ( "SOURCE_DEFAULT_GET_TYPE", "../Grammar/Expression.code" );
  // VariantExpression.setFunctionSource ( "SOURCE_EMPTY_SET_TYPE_FUNCTION", "../Grammar/Expression.code" );
  // VariantExpression.editSubstitute    ( "GENERIC_TYPE", "SgTypeDefault" );
     VariantExpression.setFunctionSource ( "SOURCE_GET_TYPE_GENERIC", "../Grammar/Expression.code" );
     VariantExpression.editSubstitute    ( "GENERIC_TYPE", "SgTypeDefault" );

     StatementExpression.setFunctionSource ( "SOURCE_STATEMENT_EXPRESSION", "../Grammar/Expression.code" );

     AsmOp.setFunctionSource             ( "SOURCE_ASM_OP", "../Grammar/Expression.code" );

  // DQ (10/8/2008): Unclear if this is how we should hancle this!
  // UserDefinedUnaryOp.setFunctionSource  ( "SOURCE_GET_TYPE_FROM_SYMBOL","../Grammar/Expression.code" );
  // UserDefinedBinaryOp.setFunctionSource ( "SOURCE_GET_TYPE_FROM_SYMBOL","../Grammar/Expression.code" );

     PseudoDestructorRefExp.setFunctionSource ( "SOURCE_GET_TYPE_CLASS_DECL", "../Grammar/Expression.code" );
     PseudoDestructorRefExp.setFunctionSource ( "SOURCE_PSEUDO_DESTRUCTOR_REF", "../Grammar/Expression.code" );

  // TV (04/22/2010): CUDA support

     CudaKernelExecConfig.setFunctionSource ( "SOURCE_CUDA_KERNEL_EXEC_CONFIG","../Grammar/Expression.code" );
     CudaKernelExecConfig.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     CudaKernelExecConfig.setFunctionSource ( "SOURCE_DEFAULT_GET_TYPE","../Grammar/Expression.code" );
     CudaKernelCallExp.setFunctionSource ( "SOURCE_CUDA_KERNEL_CALL_EXPRESSION","../Grammar/Expression.code" );

   }
