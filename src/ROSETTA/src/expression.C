#include "grammar.h"
#include "ROSETTA_macros.h"
#include "AstNodeClass.h"

// DQ (3/22/2018): Revert back to the previous implementation and pursue changing the name qualification support code.
// The new version below with the names NOT using the "_for_type" version was a problem for the unparser support
// (which reuses the same code for SgInitializedName, SgTemplateArgument, and SgAggregateInitializer).  So it might
// be better to just fixup the name qualification support and reuse the unparsing support then to reuse the name
// qualification support and implement new unparsing support.  It appears that we can not quite do both, unless
// I figure that out next.

// DQ (3/22/2018): The names of the data members have been renamed to support the name qualification support
// using the same support as for the SgConstructor initializer.  The name qualification that is supported for
// an aggregate initializer is just that for the C++11 specific type specifier that is sometime required
// (for an example of this see Cxx11_tests/test2018_47.C).  Since it is the type name that is qualified
// it does make sens to use the original names (e.g. name_qualification_length_for_type), but it would be
// inconsistant with the constructor initializer support, and eliminate the opportunity to reuse that
// supporting name qualification code.
#define USE_NAME_QUALIFICATION_THROUGH_TYPE 1

// What should be the behavior of the default constructor for Grammar

void
Grammar::setUpExpressions () {
    // In preparation for moving to Rosebud where each node type is defined in a C++-like DSL that looks like a class definition, we
    // are reorganizing this `setUpExpressions` function so that all parts of each particular class are together instead of
    // scattered throughout the thousands of lines in this function. For ease of finding them, they are also alphabetical to the
    // extent possible (ROSETTA requires derived classes to be declared before their base class, opposite the C++ and Rosebud
    // ordering).

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Terminal classes in alphabetical order
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (AbsOp,                  "AbsOp",                  "ABS_OP" );
    AbsOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                              "../Grammar/Expression.code" );
    AbsOp.editSubstitute           ( "PRECEDENCE_VALUE", "15" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef USE_FORTRAN_IR_NODES
    // DQ (12/31/2007): This expression represents constructions such as "DIM=N" which binds
    // a function parameter name with a function argument as in: "sum(array,DIM=1)".
    // DQ (12/31/2007): Support for named actual arguments to functions (fortran specific).
    NEW_TERMINAL_MACRO(ActualArgumentExpression, "ActualArgumentExpression", "ACTUAL_ARGUMENT_EXPRESSION");
    ActualArgumentExpression.editSubstitute("PRECEDENCE_VALUE", " 2");
    ActualArgumentExpression.setFunctionPrototype("HEADER_ACTUAL_ARGUMENT_EXPRESSION", "../Grammar/Expression.code");
    ActualArgumentExpression.setDataPrototype     ( "SgName", "argument_name", "= \"\"",
                                                    CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    ActualArgumentExpression.setDataPrototype     ( "SgExpression*", "expression", "= NULL",
                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (AdaAncestorInitializer, "AdaAncestorInitializer", "ADA_ANCESTOR_INIT" );
    AdaAncestorInitializer.setFunctionPrototype ( "HEADER_ADA_ANCESTOR_INITIALIZER_EXPRESSION", "../Grammar/Expression.code" );
    AdaAncestorInitializer.setDataPrototype ( "SgExpression*", "ancestor", "= nullptr",
                                              CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (AdaAttributeExp,        "AdaAttributeExp",        "ADA_ATTRIBUTE_EXP" );
    AdaAttributeExp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                        "../Grammar/Expression.code" );
    AdaAttributeExp.setDataPrototype ( "SgName", "attribute", "",
                                       CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AdaAttributeExp.setDataPrototype ( "SgExpression*", "object", "= nullptr",
                                       CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AdaAttributeExp.setDataPrototype ( "SgExprListExp*", "args", "= nullptr",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AdaAttributeExp.setDataPrototype ( "SgType*", "attrType", "= nullptr",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (AdaFloatVal,            "AdaFloatVal",            "ADA_FLOAT_VAL" );
    AdaFloatVal.setFunctionSource      ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    AdaFloatVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AdaFloatVal.setFunctionSource            ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
    // \todo set type according to Asis frontend
    AdaFloatVal.editSubstitute            ( "GENERIC_TYPE", "SgTypeFloat" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // PP (04/24/21): Adding explicit support for Ada others expressions
    NEW_TERMINAL_MACRO (AdaOthersExp,           "AdaOthersExp",               "ADA_OTHERS_EXPR" );
    AdaOthersExp.setFunctionSource   ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    AdaOthersExp.editSubstitute      ( "PRECEDENCE_VALUE", "16" );
    AdaOthersExp.setFunctionSource      ( "SOURCE_GET_TYPE_GENERIC", "../Grammar/Expression.code" );
    AdaOthersExp.editSubstitute         ( "GENERIC_TYPE", "SgTypeDefault" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (AdaProtectedRefExp,     "AdaProtectedRefExp",     "ADA_PROTECTED_REF" );

    AdaProtectedRefExp.setDataPrototype ( "SgAdaProtectedSymbol*", "symbol", "= nullptr",
                                          CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AdaProtectedRefExp.setFunctionSource    ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    AdaProtectedRefExp.editSubstitute       ( "DECLARATION_TYPE", "SgDeclarationStatement" );
    AdaProtectedRefExp.setFunctionPrototype ( "HEADER_DECLARATION_FROM_SYMBOL", "../Grammar/Expression.code" );
    AdaProtectedRefExp.setFunctionSource    ( "SOURCE_DECLARATION_FROM_SYMBOL", "../Grammar/Expression.code" );
    AdaProtectedRefExp.setFunctionSource    ( "SOURCE_GET_TYPE_FROM_SYMBOL", "../Grammar/Expression.code" );
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (AdaRenamingRefExp,      "AdaRenamingRefExp",      "ADA_RENAMING_REF" );
    AdaRenamingRefExp.setDataPrototype ( "SgAdaRenamingSymbol*", "symbol", "= nullptr",
                                         CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AdaRenamingRefExp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                          "../Grammar/Expression.code" );
    AdaRenamingRefExp.editSubstitute       ( "DECLARATION_TYPE", "SgAdaRenamingDecl" );
    AdaRenamingRefExp.setFunctionPrototype ( "HEADER_DECLARATION_FROM_SYMBOL", "../Grammar/Expression.code" );
    AdaRenamingRefExp.setFunctionSource    ( "SOURCE_DECLARATION_FROM_SYMBOL", "../Grammar/Expression.code" );
    AdaRenamingRefExp.setFunctionSource    ( "SOURCE_GET_TYPE_FROM_SYMBOL", "../Grammar/Expression.code" );


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (AdaTaskRefExp,          "AdaTaskRefExp",          "ADA_TASK_REF" );
    AdaTaskRefExp.setDataPrototype ( "SgAdaTaskSymbol*", "symbol", "= nullptr",
                                     CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AdaTaskRefExp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                      "../Grammar/Expression.code" );
    AdaTaskRefExp.editSubstitute       ( "DECLARATION_TYPE", "SgDeclarationStatement" );
    AdaTaskRefExp.setFunctionPrototype ( "HEADER_DECLARATION_FROM_SYMBOL", "../Grammar/Expression.code" );
    AdaTaskRefExp.setFunctionSource    ( "SOURCE_DECLARATION_FROM_SYMBOL", "../Grammar/Expression.code" );
    AdaTaskRefExp.setFunctionSource    ( "SOURCE_GET_TYPE_FROM_SYMBOL", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (AdaUnitRefExp,          "AdaUnitRefExp",          "ADA_UNIT_REF" );
    AdaUnitRefExp.setDataPrototype ( "SgSymbol*", "symbol", "= nullptr",
                                     CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AdaUnitRefExp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                      "../Grammar/Expression.code" );
    AdaUnitRefExp.editSubstitute       ( "DECLARATION_TYPE", "SgDeclarationStatement" );
    AdaUnitRefExp.setFunctionPrototype ( "HEADER_DECLARATION_FROM_SYMBOL", "../Grammar/Expression.code" );
    AdaUnitRefExp.setFunctionSource    ( "SOURCE_DECLARATION_FROM_SYMBOL_UNIT_REF", "../Grammar/Expression.code" );

    //~ AdaUnitRefExp.editSubstitute       ( "DECLARATION_TYPE", "SgDeclarationStatement" );
    //~ AdaUnitRefExp.setFunctionPrototype ( "HEADER_DECLARATION_FROM_SYMBOL", "../Grammar/Expression.code" );
    //~ AdaUnitRefExp.setFunctionSource    ( "SOURCE_DECLARATION_FROM_SYMBOL", "../Grammar/Expression.code" );

    // PP (06/30/21): For now AdaUnitRefExp can refer to any declaration.
    //                Consider narrowing that to packages via SgAdaPackageSymbol/SgAdaPackageSpecDecl
    //                once we have generic packages.

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (AddOp,                  "AddOp",                  "ADD_OP" );
    AddOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                              "../Grammar/Expression.code" );
    AddOp.editSubstitute           ( "PRECEDENCE_VALUE", "12" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (AddressOfOp,            "AddressOfOp",            "ADDRESS_OP" );
    AddressOfOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                    "../Grammar/Expression.code" );
    // DQ (1/12/2020): Adding support for the originalExpressionTree.
    AddressOfOp.setDataPrototype ( "SgExpression*", "originalExpressionTree", "= NULL",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AddressOfOp.editSubstitute     ( "PRECEDENCE_VALUE", "15" );
    AddressOfOp.setFunctionPrototype ( "HEADER_ADDRESS_OF_OPERATOR", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (AggregateInitializer,   "AggregateInitializer",   "AGGREGATE_INIT" );
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

    // DQ (7/26/2013): This is required for initializers using compound literals (it triggers the output of syntax that
    // makes the aggregate initializer look like a cast, but a SgCastExp should not be used to wrap the SgAggregateInitializer).
    // This fix is important for test2013_27.c and it related to the support for designated initializers.
    AggregateInitializer.setDataPrototype     ( "bool", "uses_compound_literal", "= false",
                                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (3/22/2018): This should be the type_elaboration_required data member (and to be consistant with the ConstructorInitializer.
    // In general, the use of this name qualification is only for type names that are sometime required for C++11 support (see Cxx11_tests/test2018_47.C).
    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (9/4/2013): Added support for name qualification on the type referenced by the AggregateInitializer (part of support for compound literals).
    // AggregateInitializer.setDataPrototype("bool", "requiresGlobalNameQualificationOnType", "= false",
    //             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#if USE_NAME_QUALIFICATION_THROUGH_TYPE
    AggregateInitializer.setDataPrototype("bool", "requiresGlobalNameQualificationOnType", "= false",
                                          NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

    // DQ (3/22/2018): This should be the type_elaboration_required data member (and to be consistant with the ConstructorInitializer.
    // In general, the use of this name qualification is only for type names that are sometime required for C++11 support (see Cxx11_tests/test2018_47.C).
    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (9/4/2013): Added support for name qualification on the type referenced by the AggregateInitializer (part of support for compound literals).
    // AggregateInitializer.setDataPrototype ( "int", "name_qualification_length_for_type", "= 0",
    //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#if USE_NAME_QUALIFICATION_THROUGH_TYPE
    AggregateInitializer.setDataPrototype ( "int", "name_qualification_length_for_type", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#else
    AggregateInitializer.setDataPrototype ( "int", "name_qualification_length", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif
    // DQ (3/22/2018): This should be the type_elaboration_required data member (and to be consistant with the ConstructorInitializer.
    // In general, the use of this name qualification is only for type names that are sometime required for C++11 support (see Cxx11_tests/test2018_47.C).
    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (9/4/2013): Added support for name qualification on the type referenced by the AggregateInitializer (part of support for compound literals).
    // AggregateInitializer.setDataPrototype("bool","type_elaboration_required_for_type","= false",
    //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#if USE_NAME_QUALIFICATION_THROUGH_TYPE
    AggregateInitializer.setDataPrototype("bool","type_elaboration_required_for_type","= false",
                                          NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#else
    AggregateInitializer.setDataPrototype("bool","type_elaboration_required","= false",
                                          NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

    // DQ (3/22/2018): This should be the type_elaboration_required data member (and to be consistant with the ConstructorInitializer.
    // In general, the use of this name qualification is only for type names that are sometime required for C++11 support (see Cxx11_tests/test2018_47.C).
    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (9/4/2013): Added support for name qualification on the type referenced by the AggregateInitializer (part of support for compound literals).
    // AggregateInitializer.setDataPrototype("bool","global_qualification_required_for_type","= false",
    //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#if USE_NAME_QUALIFICATION_THROUGH_TYPE
    AggregateInitializer.setDataPrototype("bool","global_qualification_required_for_type","= false",
                                          NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#else
    AggregateInitializer.setDataPrototype("bool","global_qualification_required","= false",
                                          NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

    // DQ (8/1/2014): Added to support C++11 constexpr constructors that can generate an originalExpressionTree in ROSE.
    AggregateInitializer.setDataPrototype ( "SgExpression*", "originalExpressionTree", "= NULL",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DQ (6/20/2013): Added alignOf operator.
    NEW_TERMINAL_MACRO (AlignOfOp,                    "AlignOfOp",                    "ALIGNOF_OP" );
    AlignOfOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                  "../Grammar/Expression.code" );
    AlignOfOp.editSubstitute       ( "PRECEDENCE_VALUE", "16" );
    // DQ (6/20/2013): Added alignOf operator.
    AlignOfOp.setFunctionPrototype ( "HEADER_ALIGNOF_OPERATOR", "../Grammar/Expression.code" );
    AlignOfOp.setDataPrototype ( "SgExpression*", "operand_expr", "= NULL",
                                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AlignOfOp.setDataPrototype ( "SgType*", "operand_type", "= NULL",
                                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // DQ (3/7/2013): We should not store the type of operators but instead obtain it from the operand directly.
    // I think that we are not using this data member.
    AlignOfOp.setDataPrototype ( "SgType*", "expression_type", "= NULL",
                                 CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (6/2/2011): Added support for name qualification.
    // AlignOfOp.setDataPrototype ( "int", "name_qualification_length", "= 0",
    //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AlignOfOp.setDataPrototype ( "int", "name_qualification_length", "= 0",
                                 NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (6/2/2011): Added information required for new name qualification support.
    // AlignOfOp.setDataPrototype("bool","type_elaboration_required","= false",
    //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AlignOfOp.setDataPrototype("bool","type_elaboration_required","= false",
                               NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (6/2/2011): Added information required for new name qualification support.
    // AlignOfOp.setDataPrototype("bool","global_qualification_required","= false",
    //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AlignOfOp.setDataPrototype("bool","global_qualification_required","= false",
                               NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (10/17/2012): Added information to trigger output of the defining declaration of the type (see test2012_57.c).
    // We need to control the output of the defining declaration in some interesting places where it can be specified.
    AlignOfOp.setDataPrototype("bool","alignOfContainsBaseTypeDefiningDeclaration","= false",
                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (AndAssignOp,            "AndAssignOp",            "AND_ASSIGN_OP" );
    AndAssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                    "../Grammar/Expression.code" );
    AndAssignOp.editSubstitute     ( "PRECEDENCE_VALUE", " 2" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (AndOp,                  "AndOp",                  "AND_OP" );
    AndOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                              "../Grammar/Expression.code" );
    AndOp.editSubstitute           ( "PRECEDENCE_VALUE", " 5" );
    AndOp.setFunctionPrototype ( "HEADER_AND_OPERATOR", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (ArrowExp,               "ArrowExp",               "POINTST_OP" );
    ArrowExp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                 "../Grammar/Expression.code" );
    ArrowExp.editSubstitute        ( "PRECEDENCE_VALUE", "16" );
    ArrowExp.setFunctionPrototype ( "HEADER_ARROW_EXPRESSION", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (ArrowStarOp,            "ArrowStarOp",            "ARROWSTAR_OP" );
    ArrowStarOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                    "../Grammar/Expression.code" );
    ArrowStarOp.editSubstitute     ( "PRECEDENCE_VALUE", "14" );
    ArrowStarOp.setFunctionPrototype ( "HEADER_ARROW_STAR_OPERATOR", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DQ (7/22/2006): Support for ASM operands that are contained in an SgAsmStmt
    NEW_TERMINAL_MACRO (AsmOp,                  "AsmOp",                      "ASM_OP" );
    AsmOp.setFunctionSource               ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    AsmOp.editSubstitute ( "PRECEDENCE_VALUE", "16" );
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (AssignInitializer,      "AssignInitializer",      "ASSIGN_INIT" );
    AssignInitializer.setFunctionPrototype ( "HEADER_ASSIGNMENT_INITIALIZER_EXPRESSION", "../Grammar/Expression.code" );
    AssignInitializer.setDataPrototype     ( "SgExpression*", "operand_i"      , "= NULL",
                                             CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    // DQ (1/14/2006): We should not store the type of unary operators but instead obtain it from the operand directly.
    // AssignInitializer.setDataPrototype     ( "SgType*"      , "expression_type", "= NULL",
    //        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AssignInitializer.setDataPrototype     ( "SgType*"      , "expression_type", "= NULL",
                                             CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (AssignOp,               "AssignOp",               "ASSIGN_OP" );
    AssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                 "../Grammar/Expression.code" );
    AssignOp.editSubstitute        ( "PRECEDENCE_VALUE", " 2" );
    AssignOp.setFunctionPrototype ( "HEADER_ASSIGNMENT_OPERATOR", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Rasmussen (3/6/2024): Used for array shape (Fortran,Jovial) and StarSize kind (Fortran) expressions
    NEW_TERMINAL_MACRO (AsteriskShapeExp,       "AsteriskShapeExp",       "TEMP_AsteriskShapeExp" );
    AsteriskShapeExp.editSubstitute    ( "PRECEDENCE_VALUE", " 2" );
    AsteriskShapeExp.setFunctionPrototype  ( "HEADER_ASTERISK_SHAPE_EXPRESSION", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if USE_X10_IR_NODES
    NEW_TERMINAL_MACRO (AtExp,      "AtExp",      "ATEXP_NODE" );
    AtExp.setDataPrototype              ( "SgExpression*", "expression", "= NULL",
                                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AtExp.setDataPrototype              ( "SgStatement*", "body", "= NULL",
                                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CR (9/23/2020): Jovial binary "@" operator is used to dereference pointers "member @ object".
    NEW_TERMINAL_MACRO (AtOp,                   "AtOp",                   "AT_OP" );
    AtOp.setFunctionSource  ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                              "../Grammar/Expression.code" );
    AtOp.editSubstitute            ( "PRECEDENCE_VALUE", "15" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (AwaitExpression,           "AwaitExpression",           "AWAIT_EXPR" );
    AwaitExpression.setFunctionSource         ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    // DQ (7/25/2020): Adding C++17 language features (required for C++20 support).
    AwaitExpression.setDataPrototype            ( "SgExpression*", "value", "= NULL",
                                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AwaitExpression.setFunctionSource     ( "SOURCE_DEFAULT_GET_TYPE","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (BitAndOp,               "BitAndOp",               "BITAND_OP" );
    BitAndOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                 "../Grammar/Expression.code" );
    BitAndOp.editSubstitute        ( "PRECEDENCE_VALUE", " 8" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (BitComplementOp,        "BitComplementOp",        "BIT_COMPLEMENT_OP" );
    // DQ (1/20/2019): This should be a prefix operator and so it can't use the default
    // automatically generated version of the post_construction_initialization function.
    // BitComplementOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    BitComplementOp.editSubstitute ( "PRECEDENCE_VALUE", "15" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CR (4/28/2020): Added this node to support the Jovial bitwise EQV operator.
    NEW_TERMINAL_MACRO (BitEqvOp,               "BitEqvOp",               "BITEQV_OP" );
    BitEqvOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                 "../Grammar/Expression.code" );
    BitEqvOp.editSubstitute        ( "PRECEDENCE_VALUE", " 6" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (BitOrOp,                "BitOrOp",                "BITOR_OP"  );
    BitOrOp.setFunctionSource  ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                 "../Grammar/Expression.code" );
    BitOrOp.editSubstitute         ( "PRECEDENCE_VALUE", " 6" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (BitXorOp,               "BitXorOp",               "BITXOR_OP" );
    BitXorOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                 "../Grammar/Expression.code" );
    BitXorOp.editSubstitute        ( "PRECEDENCE_VALUE", " 7" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (BoolValExp,             "BoolValExp",             "BOOL_VAL" );
    BoolValExp.setFunctionSource       ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    BoolValExp.setDataPrototype ( "int", "value", "= 0",
                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    BoolValExp.setFunctionSource             ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
    BoolValExp.editSubstitute     ( "GENERIC_TYPE", "SgTypeBool" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DQ (11/15/2016): Adding support for new SgBracedInitializer, required to template support (see Cxx11_tests/test2016_82.C).
    NEW_TERMINAL_MACRO (BracedInitializer,      "BracedInitializer",      "BRACED_INIT" );
    // DQ (11/15/2016): Adding support for new SgBracedInitializer, required to template support (see Cxx11_tests/test2016_82.C).
    BracedInitializer.setFunctionPrototype ( "HEADER_BRACED_INITIALIZER_EXPRESSION", "../Grammar/Expression.code" );
    // BracedInitializer.setDataPrototype     ( "SgExpression*", "operand_i"      , "= NULL",
    //                                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    BracedInitializer.editSubstitute       ( "HEADER_LIST_DECLARATIONS", "HEADER_LIST_FUNCTIONS", "../Grammar/Expression.code" );
    BracedInitializer.editSubstitute       ( "LIST_NAME", "initializer" );
    BracedInitializer.setDataPrototype     ( "SgExprListExp*", "initializers", "= NULL",
                                             CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    BracedInitializer.setDataPrototype     ( "SgType*"      , "expression_type", "= NULL",
                                             CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // Not certain that I want this data member since it is redundant with the list of initializers being lenght zero.
    // This would make a better member function that tested the list size than a new data member.
    // BracedInitializer.setDataPrototype     ( "bool", "is_empty_braced_initializer", "= false",
    //                                               CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if USE_FORTRAN_IR_NODES
    // FMZ (2/6/2009): Added CoArray Reference Expression
    NEW_TERMINAL_MACRO (CAFCoExpression,    "CAFCoExpression",    "COARRAY_REF_EXPR" );
    // FMZ (2/6/2009): Added for SgCAFCoExpression--following SgPntrArrRefExp
    CAFCoExpression.editSubstitute ( "PRECEDENCE_VALUE", " 16" );
    //FMZ (2/5/2009): Added for CAFCoExpression => change teamId from "SgName" to "SgVarRefExp*"

    //CAFCoExpression.setDataPrototype ( "SgName", "teamId",  "= \"\"",
    //                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,NO_TRAVERSAL,NO_DELETE);
    CAFCoExpression.setDataPrototype ( "SgVarRefExp*", "teamId",  "= NULL",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    CAFCoExpression.setDataPrototype ( "SgExpression*", "teamRank", "= NULL",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    CAFCoExpression.setDataPrototype ( "SgExpression*", "referData", "= NULL",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,DEF_TRAVERSAL, NO_DELETE);
    CAFCoExpression.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                        "../Grammar/Expression.code" );
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (CastExp,                "CastExp",                "CAST_OP" );
    CastExp.editSubstitute         ( "PRECEDENCE_VALUE", "15" );
    CastExp.setFunctionPrototype ( "HEADER_CAST_EXPRESSION", "../Grammar/Expression.code" );

    CastExp.setDataPrototype     ( "SgCastExp::cast_type_enum", "cast_type" , "= SgCastExp::e_C_style_cast",
                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (9/23/2011): Modified this to not be traversed.  The traversal leads to an inconsistant AST (incrementally applying fixes).
    // DQ (9/22/2011): Double checked and this was marked as DEF_TRAVERSAL in master (so we don't have to change this case).
    // DQ (9/17/2011): Put back the traversal over the originalExpressionTree (because it will be set to NULL in post processing).
    // DQ (9/16/2011): Modified this to not be traversed.  The traversal leads to an inconsistant AST
    // We now want to select two modes of consistant AST (using or not use the originalExpressionTree).
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
                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (6/2/2011): Added support for name qualification.
    // CastExp.setDataPrototype ( "int", "name_qualification_length", "= 0",
    //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    CastExp.setDataPrototype ( "int", "name_qualification_length", "= 0",
                               NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (6/2/2011): Added information required for new name qualification support.
    // CastExp.setDataPrototype("bool","type_elaboration_required","= false",
    //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    CastExp.setDataPrototype("bool","type_elaboration_required","= false",
                             NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (6/2/2011): Added information required for new name qualification support.
    // CastExp.setDataPrototype("bool","global_qualification_required","= false",
    //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    CastExp.setDataPrototype("bool","global_qualification_required","= false",
                             NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (10/17/2012): Added information to trigger output of the defining declaration of the type (see test2012_46.c).
    // We need to control the output of the defining declaration in some interesting places where it can be specified.
    CastExp.setDataPrototype("bool","castContainsBaseTypeDefiningDeclaration","= false",
                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (4/15/2019): Let's demonstrate this is possible in the language before we add support for it (see Cxx11_test/test2019_381.C).
    // DQ (4/15/2019): This is needed to support pointers to member type specified to the cast operator.
    CastExp.setDataPrototype ( "int", "name_qualification_for_pointer_to_member_class_length", "= 0",
                               NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (4/15/2019): This is needed to support pointers to member type specified to the cast operator.
    CastExp.setDataPrototype("bool","type_elaboration_for_pointer_to_member_class_required","= false",
                             NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (4/15/2019): This is needed to support pointers to member type specified to the cast operator.
    CastExp.setDataPrototype("bool","global_qualification_for_pointer_to_member_class_required","= false",
                             NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DQ (2/16/2018): Adding support for char16_t and char32_t (C99 and C++11 specific types).
    NEW_TERMINAL_MACRO (Char16Val,              "Char16Val",              "CHAR16_VAL" );
    // DQ (2/16/2018): Adding support for char16_t and char32_t (C99 and C++11 specific types).
    Char16Val.setFunctionPrototype ( "HEADER_CHAR16_VALUE_EXPRESSION", "../Grammar/Expression.code" );
    Char16Val.setDataPrototype ( "unsigned short", "valueUL", "= 0",
                                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    Char16Val.setDataPrototype ( "std::string", "valueString", "= \"\"",
                                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // DQ (2/16/2018): Adding support for char16_t and char32_t (C99 and C++11 specific types).
    Char16Val.setFunctionSource              ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
    // DQ (2/16/2018): Adding support for char16_t and char32_t (C99 and C++11 specific types).
    Char16Val.editSubstitute              ( "GENERIC_TYPE", "SgTypeChar16" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (Char32Val,              "Char32Val",              "CHAR32_VAL" );
    // DQ (2/16/2018): Adding support for char16_t and char32_t (C99 and C++11 specific types).
    Char32Val.setFunctionPrototype ( "HEADER_CHAR32_VALUE_EXPRESSION", "../Grammar/Expression.code" );
    Char32Val.setDataPrototype ( "unsigned int", "valueUL", "= 0",
                                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    Char32Val.setDataPrototype ( "std::string", "valueString", "= \"\"",
                                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    Char32Val.setFunctionSource              ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
    Char32Val.editSubstitute              ( "GENERIC_TYPE", "SgTypeChar32" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (CharVal,                "CharVal",                "CHAR_VAL" );
    CharVal.setFunctionSource          ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    CharVal.setDataPrototype ( "char", "value", "= 0",
                               CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    CharVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
                               CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    CharVal.setFunctionSource                ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
    CharVal.editSubstitute        ( "GENERIC_TYPE", "SgTypeChar" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (ChooseExpression,          "ChooseExpression",          "CHOOSE_EXPR" );
    ChooseExpression.setFunctionSource        ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    // DQ (7/25/2020): Adding C++17 language features (required for C++20 support).
    ChooseExpression.setDataPrototype            ( "SgExpression*", "value", "= NULL",
                                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    ChooseExpression.setFunctionSource    ( "SOURCE_DEFAULT_GET_TYPE","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (ClassExp,               "ClassExp",               "CLASS_NODE" );
    ClassExp.setFunctionSource         ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    ClassExp.setFunctionPrototype ( "HEADER_CLASS_EXPRESSION", "../Grammar/Expression.code" );
    ClassExp.setDataPrototype     ( "SgClassSymbol*", "class_symbol", "= NULL",
                                    CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    ClassExp.setDataPrototype     ( "int", "pobj_class", "= 0",
                                    CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (ClassNameRefExp,              "ClassNameRefExp",              "CLASSNAME_REF" );
    ClassNameRefExp.excludeFunctionPrototype ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );
    ClassNameRefExp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                        "../Grammar/Expression.code" );
    ClassNameRefExp.setDataPrototype ( "SgClassSymbol*", "symbol", "= NULL",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if USE_FORTRAN_IR_NODES
    NEW_TERMINAL_MACRO (ColonShapeExp,          "ColonShapeExp",          "TEMP_ColonShapeExp" );
    // Note that SgColonExp and SgAsteriskExp are used in array shape specification, but not indexing,
    // so they are not part of subscript expressions.
    // DQ (11/18/2007): I think this should have lowest precedence (unclear if this really
    // made any difference, in the end I need to handle this as a special case to about over use of "()"
    // Colon.editSubstitute               ( "PRECEDENCE_VALUE", " 2" );
    ColonShapeExp.editSubstitute       ( "PRECEDENCE_VALUE", " 17" );
    // DQ (11/18/2007): This IR node is used in declarations!
    // Colon.setFunctionPrototype ( "HEADER_COLON_EXPRESSION", "../Grammar/Expression.code" );
    // Colon.setDataPrototype ( "SgExpression*", "lowerBound", "= NULL",
    //                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    // Colon.setDataPrototype ( "SgExpression*", "stride", "= NULL",
    //                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (CommaOpExp,             "CommaOpExp",             "COMMA_OP" );
    CommaOpExp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                   "../Grammar/Expression.code" );
    // CR (4/28/2020): Added this node to support the Jovial bitwise operator.
    // Note that precedence of Jovial bitwise operators must be specified by parens, '(' ')'
    // The PRECEDENCE_VALUE of "6" was chosen so as not to have to change other precedence values.
    CommaOpExp.editSubstitute      ( "PRECEDENCE_VALUE", " 1" ); // lowest precedence
    CommaOpExp.setFunctionPrototype ( "HEADER_COMMA_OPERATOR_EXPRESSION", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DQ (8/27/2006): Added support for complex values (We will use a ComplexVal to stand for a imaginary number as well).
    NEW_TERMINAL_MACRO (ComplexVal,             "ComplexVal",             "COMPLEX_VAL" );
    ComplexVal.setFunctionSource       ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    // DQ (8/27/2006): Added support for Complex values (save the values as long doubles internally within the AST)
    // JJW (11/22/2008): Changed members to SgValueExp*; real_value can be NULL for imaginary numbers
    // DQ (10/7/2014): Added missing default values (caught by ROSETTA generated aterm support).
    ComplexVal.setDataPrototype ( "SgValueExp*", "real_value", "= NULL",
                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    // DQ (10/7/2014): Added missing default values (caught by ROSETTA generated aterm support).
    ComplexVal.setDataPrototype ( "SgValueExp*", "imaginary_value", "= NULL",
                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    ComplexVal.setDataPrototype ( "SgType*", "precisionType", "= NULL",
                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // DQ (11/9/2005): Added string to hold source code constant precisely (part of work with Andreas)
    ComplexVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    ComplexVal.editSubstitute             ( "GENERIC_TYPE", "SgTypeComplex" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (CompoundInitializer,    "CompoundInitializer",    "COMPOUND_INIT" );
    // DQ (9/4/2013): This should be replaced by the use of SgCompoundLiteral since it is the concept trying to be expressed here
    // but SgCompoundLiteral is derived from SgExpression, and there is no such thing as a CompoundInitializer.  This is
    // a confusing topic and this IR nod represent partial support for where compound literals are used in initializers, but
    // it is better to support a proper SgCompoundLiteral IR node (just being added today) since it can be used outside of
    // the concept of initialization.  This SgCompoundInitializer is not used in the new EDG/ROSE connection, and the use of
    // SgCompoundLiteral is being added currently.
    // TV (03/04/2012) Compound initializer: for OpenCL (Vector type initializer): float4 a = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
    CompoundInitializer.setFunctionPrototype ( "HEADER_COMPOUND_INITIALIZER_EXPRESSION", "../Grammar/Expression.code" );
    CompoundInitializer.editSubstitute       ( "HEADER_LIST_DECLARATIONS", "HEADER_LIST_FUNCTIONS", "../Grammar/Expression.code" );
    CompoundInitializer.editSubstitute       ( "LIST_NAME", "initializer" );
    CompoundInitializer.setDataPrototype     ( "SgExprListExp*", "initializers", "= NULL",
                                               CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    CompoundInitializer.setDataPrototype     ( "SgType*", "expression_type", "= NULL",
                                               CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DQ (9/4/2013): Adding support for compound literals.  These are not the same as initializers and define
    // a memory location that is un-named (much like an un-named variable).  When they are const they cannot
    // be written to and can be unified where they are the same value.  Any expression can be placed into the
    // list. C90 and C99 define writting to the compound literal as undefined behavior.
    NEW_TERMINAL_MACRO (CompoundLiteralExp,        "CompoundLiteralExp",        "COMPOUND_LITERAL" );
    CompoundLiteralExp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                           "../Grammar/Expression.code" );
    // DQ (9/4/2013): Adding support for compound literals.  These are not the same as initializers and define
    // a memory location that is un-named (much like an un-named variable).  When they are const they cannot
    // be written to and can be unified where they are the same value.  Any expression can be placed into the
    // list. C90 and C99 define writting to the compound literal as undefined behavior.
    CompoundLiteralExp.setFunctionPrototype ( "HEADER_COMPOUND_LITERAL_EXPRESSION", "../Grammar/Expression.code" );

    // DQ (9/4/2013): This better matches the concept of unnamed variable and follows the design of a variable reference.
    // Note that a SgVariableSymbol is used which means that the declaration will be a SgInitializedName which will contain
    // a internally generated name used as a key to add the SgVariableSymbol to the symbol table.
    CompoundLiteralExp.setDataPrototype ( "SgVariableSymbol*", "symbol", "= NULL",
                                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (9/4/2013): This follows the design of CompoundLiteralExp to be similar to a variable reference.
    CompoundLiteralExp.setFunctionSource     ( "SOURCE_GET_TYPE_FROM_SYMBOL","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (Comprehension,             "Comprehension",                "COMPREHENSION");
    Comprehension.setFunctionSource           ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    Comprehension.setDataPrototype ("SgExpression*", "target", "= NULL",
                                    CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    Comprehension.setDataPrototype ("SgExpression*", "iter", "= NULL",
                                    CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    Comprehension.setDataPrototype ("SgExprListExp*", "filters", "= NULL",
                                    CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    Comprehension.setFunctionSource     ( "SOURCE_DEFAULT_GET_TYPE","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DQ (12/13/2007): Added support for Fortran string concatenation operator
    NEW_TERMINAL_MACRO (ConcatenationOp,        "ConcatenationOp",        "CONCATENATION_OP" );
    ConcatenationOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                        "../Grammar/Expression.code" );
    ConcatenationOp.editSubstitute ( "PRECEDENCE_VALUE", " 3" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (ConjugateOp,            "ConjugateOp",            "CONJUGATE_OP" );
    ConjugateOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                    "../Grammar/Expression.code" );
    ConjugateOp.editSubstitute      ( "PRECEDENCE_VALUE", "15" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (ConditionalExp,         "ConditionalExp",         "EXPR_CONDITIONAL" );
    ConditionalExp.setFunctionPrototype ( "HEADER_GET_NEXT_EXPRESSION", "../Grammar/Expression.code" );
    ConditionalExp.setFunctionPrototype ( "HEADER_CONDITIONAL_EXPRESSION", "../Grammar/Expression.code" );
    ConditionalExp.setDataPrototype ( "SgExpression*", "conditional_exp", "= NULL",
                                      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    ConditionalExp.setDataPrototype ( "SgExpression*", "true_exp"       , "= NULL",
                                      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    ConditionalExp.setDataPrototype ( "SgExpression*", "false_exp"      , "= NULL",
                                      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    // DQ (1/14/2006): We should not store the type of unary operators but instead obtain it from the operand directly.
    // ConditionalExp.setDataPrototype ( "SgType*"      , "expression_type", "= NULL",
    //        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    ConditionalExp.setDataPrototype ( "SgType*"      , "expression_type", "= NULL",
                                      CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (ConstructorInitializer, "ConstructorInitializer", "CONSTRUCTOR_INIT" );
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

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (6/1/2011): Added support for name qualification.
    // ConstructorInitializer.setDataPrototype ( "int", "name_qualification_length", "= 0",
    //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    ConstructorInitializer.setDataPrototype ( "int", "name_qualification_length", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (5/12/2011): Added information required for new name qualification support.
    // ConstructorInitializer.setDataPrototype("bool","type_elaboration_required","= false",
    //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    ConstructorInitializer.setDataPrototype("bool","type_elaboration_required","= false",
                                            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (5/12/2011): Added information required for new name qualification support.
    // ConstructorInitializer.setDataPrototype("bool","global_qualification_required","= false",
    //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    ConstructorInitializer.setDataPrototype("bool","global_qualification_required","= false",
                                            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (1/15/2019): Adding support for initializers in for loop tests (conditionals), see Cxx_tests/test2019_02.C).
    ConstructorInitializer.setDataPrototype     ( "bool", "is_used_in_conditional", "= false",
                                                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // DQ (7/19/2006): Suggested change by Jeremiah Willcock
    // ConstructorInitializer.setFunctionSource ( "SOURCE_GET_TYPE_GENERIC", "../Grammar/Expression.code" );
    // ConstructorInitializer.editSubstitute    ( "GENERIC_TYPE", "SgTypeDefault" );
    ConstructorInitializer.setFunctionSource ( "SOURCE_GET_TYPE_CLASS_DECL", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // sgCudaKernelCallExp is a node for CUDA support, it catch kernel's call.
    NEW_TERMINAL_MACRO (CudaKernelCallExp,        "CudaKernelCallExp",        "KERN_CALL" );
    CudaKernelCallExp.editSubstitute ( "PRECEDENCE_VALUE", " 0" );
    CudaKernelCallExp.editSubstitute       ( "HEADER_LIST_DECLARATIONS", "HEADER_LIST_FUNCTIONS", "../Grammar/Expression.code" );
    CudaKernelCallExp.setDataPrototype ( "SgCudaKernelExecConfig*", "exec_config", "= NULL", CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // TV (04/22/2010): CUDA support
    // sgCudaKernelExecConfig is the '<<< grid, block, shared_size, stream >>>' part of a kernel call
    NEW_TERMINAL_MACRO (CudaKernelExecConfig,     "CudaKernelExecConfig",     "EXEC_CONF" );
    CudaKernelExecConfig.editSubstitute ( "PRECEDENCE_VALUE", " 0" );
    // TV (04/22/2010): CUDA support
    CudaKernelExecConfig.setDataPrototype ( "SgExpression*", "grid",   "= NULL", CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    CudaKernelExecConfig.setDataPrototype ( "SgExpression*", "blocks", "= NULL", CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    CudaKernelExecConfig.setDataPrototype ( "SgExpression*", "shared", "= NULL", CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    CudaKernelExecConfig.setDataPrototype ( "SgExpression*", "stream", "= NULL", CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    // TV (04/22/2010): CUDA support
    CudaKernelExecConfig.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    CudaKernelExecConfig.setFunctionSource ( "SOURCE_DEFAULT_GET_TYPE","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (DeleteExp,              "DeleteExp",              "DELETE_OP" );
    DeleteExp.setFunctionPrototype ( "HEADER_GET_NEXT_EXPRESSION", "../Grammar/Expression.code" );
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

    // DQ (1/16/2006): I think this should be void instead of generic
    // DeleteExp.setFunctionSource           ( "SOURCE_GET_TYPE_GENERIC",
    //                                              "../Grammar/Expression.code" );
    // DeleteExp.editSubstitute              ( "GENERIC_TYPE", "SgTypeVoid" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // An expression with a designator, used for designated initialization in
    // SgAggregateInitializer
    NEW_TERMINAL_MACRO (DesignatedInitializer, "DesignatedInitializer", "DESIGNATED_INITIALIZER" );
    DesignatedInitializer.setFunctionPrototype ( "HEADER_DESIGNATED_INITIALIZER", "../Grammar/Expression.code" );

    // DQ (7/21/2013): I may have been too quick to not consider multi-deminsional array references (so move back to support SgExprListExp*).
    // DQ (7/21/2013): I think that the use of a SgExprListExp* for the data member of this SgDesignatedInitializer is incorrect, unless we wanted it
    // to be a DesignatedInitializerList in which case we would at least need a list of SgInitializer pointers.  So we should have the first data
    // member be a SgVarRefExp or SgValueExp (so make it a SgExpression) and the second data member can be the SgInitializer* (as it is).
    // Note that the SgAggregateInitializer holds the list and should reference the associated type.
    // Each of these fields is either a SgValueExp for an array index or an SgVarRefExp for a struct field name -- they are chained to form the actual designator
    DesignatedInitializer.setDataPrototype("SgExprListExp*", "designatorList", "= NULL", CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    // DesignatedInitializer.setDataPrototype("SgExpression*" , "designator", "= NULL", CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    DesignatedInitializer.setDataPrototype("SgInitializer*", "memberInit", "= NULL", CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (DictionaryComprehension,   "DictionaryComprehension",      "DICTIONARY_COMPREHENSION" );
    DictionaryComprehension.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    DictionaryComprehension.setDataPrototype ("SgKeyDatumPair*", "element", "= NULL",
                                              CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    DictionaryComprehension.setDataPrototype ("SgExprListExp*", "generators", "= NULL",
                                              CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    DictionaryComprehension.setFunctionSource     ( "SOURCE_DEFAULT_GET_TYPE","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (DictionaryExp,             "DictionaryExp",                "DICT_EXP" );
    DictionaryExp.setFunctionSource    ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    DictionaryExp.setFunctionPrototype ( "HEADER_DICTIONARY_EXP", "../Grammar/Expression.code" );
    DictionaryExp.setDataPrototype("SgKeyDatumPairPtrList", "key_datum_pairs", "",
                                   NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    DictionaryExp.editSubstitute       ( "HEADER_LIST_DECLARATIONS", "HEADER_LIST_FUNCTIONS", "../Grammar/Expression.code" );
    DictionaryExp.editSubstitute       ( "LIST_NAME", "key_datum_pair" );
    DictionaryExp.setFunctionSource    ( "SOURCE_DEFAULT_GET_TYPE","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (DivAssignOp,            "DivAssignOp",            "DIV_ASSIGN_OP" );
    DivAssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                    "../Grammar/Expression.code" );
    DivAssignOp.editSubstitute     ( "PRECEDENCE_VALUE", " 2" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (DivideOp,               "DivideOp",               "DIV_OP" );
    DivideOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                 "../Grammar/Expression.code" );
    DivideOp.editSubstitute        ( "PRECEDENCE_VALUE", "13" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if USE_X10_IR_NODES
    NEW_TERMINAL_MACRO (DotDotExp,  "DotDotExp",  "DOTDOT_NODE" );
    DotDotExp.editSubstitute            ( "PRECEDENCE_VALUE", "16" );
    DotDotExp.setFunctionSource         ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    DotDotExp.setFunctionPrototype      ( "HEADER_DOTDOT_EXPRESSION", "../Grammar/Expression.code");
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (DotExp,                 "DotExp",                 "RECORD_REF" );
    DotExp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                               "../Grammar/Expression.code" );
    DotExp.editSubstitute          ( "PRECEDENCE_VALUE", "16" );
    DotExp.setFunctionPrototype ( "HEADER_DOT_EXPRESSION", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (DotStarOp,              "DotStarOp",              "DOTSTAR_OP" );
    // DQ (2/27/2005): We need to have the type of the DotStarOp set (so uncomment this)
    // Bugfix (2/27/2001) we want to use the empty version of this function since then we don't
    // have to build an empty version of the set_type member function (this just makes more sense).
    DotStarOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                  "../Grammar/Expression.code" );
    // DQ (2/27/2005): We want to post_construction_initialization to call set_type so we don't want
    // and empty function here plus I have added a set_type function for DotStarOp.
    // Bugfix (2/27/2001) Generate this empty function instead of one with a call to an empty setType() function
    // DotStarOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    DotStarOp.editSubstitute       ( "PRECEDENCE_VALUE", "14" );
    DotStarOp.setFunctionPrototype ( "HEADER_DOT_STAR_OPERATOR", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (DoubleVal,              "DoubleVal",              "DOUBLE_VAL" );
    DoubleVal.setFunctionSource        ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    DoubleVal.setDataPrototype ( "double", "value", "= 0.0",
                                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // DQ (11/9/2005): Added string to hold source code constant precisely (part of work with Andreas)
    DoubleVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
                                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    DoubleVal.setFunctionSource              ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
    DoubleVal.editSubstitute              ( "GENERIC_TYPE", "SgTypeDouble" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (ElementwiseAddOp, "ElementwiseAddOp", "ELEMENT_ADD_OP");
    ElementwiseAddOp.setFunctionSource          ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    ElementwiseAddOp.editSubstitute ( "PRECEDENCE_VALUE", "12" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (ElementwiseDivideOp, "ElementwiseDivideOp", "ELEMENT_DIVIDE_OP");
    ElementwiseDivideOp.setFunctionSource          ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    ElementwiseDivideOp.editSubstitute ( "PRECEDENCE_VALUE", "13" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (ElementwiseLeftDivideOp, "ElementwiseLeftDivideOp", "ELEMENT_LEFT_DIVIDE_OP");
    ElementwiseLeftDivideOp.setFunctionSource          ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    ElementwiseLeftDivideOp.editSubstitute ( "PRECEDENCE_VALUE", "13" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //SK (06/25/2015) Elementwise operators in Matlab
    NEW_TERMINAL_MACRO (ElementwiseMultiplyOp, "ElementwiseMultiplyOp", "ELEMENT_MULT_OP");
    ElementwiseMultiplyOp.setFunctionSource          ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    ElementwiseMultiplyOp.editSubstitute ( "PRECEDENCE_VALUE", "13" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (ElementwisePowerOp, "ElementwisePowerOp", "ELEMENT_POWER_OP");
    ElementwisePowerOp.setFunctionSource          ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    ElementwisePowerOp.editSubstitute ( "PRECEDENCE_VALUE", "14" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (ElementwiseSubtractOp, "ElementwiseSubtractOp", "ELEMENT_SUBTRACT_OP");
    ElementwiseSubtractOp.setFunctionSource          ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    ElementwiseSubtractOp.editSubstitute ( "PRECEDENCE_VALUE", "12" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (EnumVal,                "EnumVal",                "ENUM_VAL" );
    EnumVal.setFunctionSource          ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    EnumVal.setFunctionPrototype ( "HEADER_ENUM_VALUE_EXPRESSION", "../Grammar/Expression.code" );

    // PP (01/31/22) changed type from int to long long int to handle large enum values in Ada.
    EnumVal.setDataPrototype ( "long long int", "value", "= 0",
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

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (5/11/2011): Added support for name qualification.
    // EnumVal.setDataPrototype ( "int", "name_qualification_length", "= 0",
    //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    EnumVal.setDataPrototype ( "int", "name_qualification_length", "= 0",
                               NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (5/11/2011): Added information required for new name qualification support.
    // EnumVal.setDataPrototype("bool","type_elaboration_required","= false",
    //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    EnumVal.setDataPrototype("bool","type_elaboration_required","= false",
                             NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (5/11/2011): Added information required for new name qualification support.
    // EnumVal.setDataPrototype("bool","global_qualification_required","= false",
    //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    EnumVal.setDataPrototype("bool","global_qualification_required","= false",
                             NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // DQ (2/5/2020): I think this should be a TypeEnum, so fixing this now.
    // I think we require a seperate get_type() function so that we can pass in the required SgEnumDeclaration.
    // EnumVal.setFunctionSource                ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
    // DQ (2/5/2020): I think this should be a TypeEnum, so fixing this now.
    // I think we require a seperate get_type() function so that we can pass in the required SgEnumDeclaration.
    // Shouldn't this be using the TypeEnum?
    // EnumVal.editSubstitute                ( "GENERIC_TYPE", "SgTypeInt" );
    // EnumVal.editSubstitute                ( "GENERIC_TYPE", "SgEnumType" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (EqualityOp,             "EqualityOp",             "EQ_OP" );
    EqualityOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                   "../Grammar/Expression.code" );
    // DQ (8/8/2006): A review of operator precedence with Jeremiah pointed out a number of errors!
    // EqualityOp.editSubstitute      ( "PRECEDENCE_VALUE", " 2" );
    EqualityOp.editSubstitute      ( "PRECEDENCE_VALUE", " 9" );
    EqualityOp.setFunctionPrototype ( "HEADER_EQUALITY_OPERATOR", "../Grammar/Expression.code" );
    // DQ (6/20/2006): Relational Operators must return bool type
    EqualityOp.editSubstitute       ( "HEADER_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "HEADER_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );
    EqualityOp.setFunctionSource       ( "SOURCE_EQUALITY_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
    // DQ (6/20/2006): Relational Operators must return bool type
    EqualityOp.editSubstitute       ( "SOURCE_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "SOURCE_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (ExponentiationAssignOp, "ExponentiationAssignOp", "EXP_ASSIGN_OP" );
    ExponentiationAssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                               "../Grammar/Expression.code" );
    ExponentiationAssignOp.editSubstitute ( "PRECEDENCE_VALUE", " 2" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if USE_FORTRAN_IR_NODES
    // Binary operator
    NEW_TERMINAL_MACRO (ExponentiationOp,       "ExponentiationOp",       "EXPONENTIATION_OP" );
    ExponentiationOp.editSubstitute    ( "PRECEDENCE_VALUE", " 2" );
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (ExpressionRoot,         "ExpressionRoot",         "EXPRESSION_ROOT" );
    // *** Expression objects using empty (non-existant) post_construction_initialization() member functions
    ExpressionRoot.setFunctionSource   ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    ExpressionRoot.excludeFunctionPrototype ( "HEADER_PRECEDENCE", "../Grammar/Expression.code" );
    // QY:9/30/04: remove statement pointer. use parent pointer instead
    // ExpressionRoot.setDataPrototype ( "SgStatement*", "statement", "= NULL",
    //      CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // ExpressionRoot.setAutomaticGenerationOfDataAccessFunctions(false);
    // QY: 9/30/2004: removed stmt pointer inside exprRoot. Use parent pointer instead.
    // void initialize_root (SgExpression* exp, SgStatement* stmt);

    // DQ (12/17/2006): Removed to clean up code (not used, I think).
    // void set_statement( SgStatement* stmt );
    // SgStatement* get_statement() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if USE_X10_IR_NODES
    NEW_TERMINAL_MACRO (FinishExp,  "FinishExp",  "FINISHEXP_NODE" );
    FinishExp.setDataPrototype          ( "SgExpression*", "expression", "= NULL",
                                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    FinishExp.setDataPrototype          ( "SgStatement*", "body", "= NULL",
                                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (Float128Val,            "Float128Val",            "FLOAT_128_VAL" );
    Float128Val.setFunctionSource      ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    Float128Val.setDataPrototype ( "long double", "value", "= 0.0",
                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // DQ (11/9/2005): Added string to hold source code constant precisely (part of work with Andreas)
    Float128Val.setDataPrototype ( "std::string", "valueString", "= \"\"",
                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    Float128Val.setFunctionSource            ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
    Float128Val.editSubstitute            ( "GENERIC_TYPE", "SgTypeFloat128" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (Float80Val,             "Float80Val",             "FLOAT_80_VAL" );
    Float80Val.setFunctionSource       ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    Float80Val.setDataPrototype ( "long double", "value", "= 0.0",
                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // DQ (11/9/2005): Added string to hold source code constant precisely (part of work with Andreas)
    Float80Val.setDataPrototype ( "std::string", "valueString", "= \"\"",
                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    Float80Val.setFunctionSource             ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
    Float80Val.editSubstitute             ( "GENERIC_TYPE", "SgTypeFloat80" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (FloatVal,               "FloatVal",               "FLOAT_VAL" );
    FloatVal.setFunctionSource         ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    FloatVal.setDataPrototype ( "float", "value", "= 0.0",
                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // DQ (11/9/2005): Added string to hold source code constant precisely (part of work with Andreas)
    FloatVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    FloatVal.setFunctionSource               ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
    FloatVal.editSubstitute               ( "GENERIC_TYPE", "SgTypeFloat" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DQ (7/25/2020): Adding C++17 fold operator support.
    NEW_TERMINAL_MACRO (FoldExpression,            "FoldExpression",            "FOLD_EXPR" );
    // DQ (7/25/2020): Adding support for C++17 and C++20.
    FoldExpression.setFunctionSource          ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    // DQ (7/25/2020): Adding C++17 language features (required for C++20 support).
    FoldExpression.setDataPrototype            ( "SgExpression*", "operands", "= NULL",
                                                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    FoldExpression.setDataPrototype            ( "std::string", "operator_token", "= \"\"",
                                                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    FoldExpression.setDataPrototype            ("bool","is_left_associative","= false",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // DQ (7/25/2020): Adding support for C++20.
    FoldExpression.setFunctionSource      ( "SOURCE_DEFAULT_GET_TYPE","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DQ (8/8/2014): Added support for C++11 decltype which references a function parameter.
    NEW_TERMINAL_MACRO (FunctionParameterRefExp, "FunctionParameterRefExp", "FUNCTION_PARAMETER_REF_EXP" );
    // DQ (8/8/2014): Added support for function parameter reference used in C++11 decltype type declarations.
    FunctionParameterRefExp.setFunctionSource( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    // DQ (8/11/2014): Added support for C++11 decltype used in new function return syntax.
    FunctionParameterRefExp.editSubstitute ( "PRECEDENCE_VALUE", "16" );
    // DQ (8/8/2014): Added support for function parameter reference used in C++11 decltype type declarations.
    FunctionParameterRefExp.setDataPrototype ("int", "parameter_number", "= -1",
                                              CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    FunctionParameterRefExp.setDataPrototype ("int", "parameter_levels_up", "= -1",
                                              CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // DQ (8/8/2014): This is where we store the reference to the function parameter (likely a SgVarRefExp).
    // This value is computed as part of the post-processing of the ROSE AST (using the parameter_number and
    // parameter_levels_up values.
    FunctionParameterRefExp.setDataPrototype ("SgExpression*", "parameter_expression", "= NULL",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    // DQ (2/14/2015): This can't be a part of the AST (so can't be defined in a traversal), since types
    // are never traversed.
    // DQ (11/10/2014): We need to store an explicit type pointer in this IR node so that we can support
    // the get_type() function called from any expression that might have this kind of IR node in its subtree.
    // FunctionParameterRefExp.setDataPrototype ("SgType*", "parameter_type", "= NULL",
    //                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    FunctionParameterRefExp.setDataPrototype ("SgType*", "parameter_type", "= NULL",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // DQ (8/8/2014): Added support for function parameter reference used in C++11 decltype type declarations.
    // I think we need a custom get_type() function.
    // FunctionParameterRefExp.setFunctionSource ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (FunctionRefExp,               "FunctionRefExp",               "FUNCTION_REF" );
    FunctionRefExp.setFunctionPrototype ( "HEADER_FUNCTION_REF_EXPRESSION", "../Grammar/Expression.code" );
    FunctionRefExp.setDataPrototype ( "SgFunctionSymbol*", "symbol_i"     , "= NULL",
                                      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (1/14/2006): The function type should be computed from the function declaration (instead of being stored)
    // Leave the type in the constructor for storage internally and build a special version of get_type() to access
    // this value or later compute it directly.
    // FunctionRefExp.setDataPrototype ( "SgFunctionType*"  , "function_type", "= NULL",
    //        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    FunctionRefExp.setDataPrototype ( "SgFunctionType*"  , "function_type", "= NULL",
                                      CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // DQ (9/22/2011): Double checked and this was marked as NO_TRAVERSAL in master (so mark it as such so
    // that I can pass the existing tests).  I am trying to pass the existing tests and implement the
    // new changes to support originalExpressionTree handling gradually so that I can identify what
    // changes are breaking a few of the existing tests.
    // DQ (9/17/2011): Put back the traversal over the originalExpressionTree (because it will be set to NULL in post processing).
    // DQ (9/16/2011): Modified this to specify NO_TRAVERSAL.
    // DQ (2/6/2011): Added reference to expression tree for unfolded constant expressions (see comment above).
    FunctionRefExp.setDataPrototype ( "SgExpression*", "originalExpressionTree", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (5/12/2011): Added support for name qualification.
    // FunctionRefExp.setDataPrototype ( "int", "name_qualification_length", "= 0",
    //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    FunctionRefExp.setDataPrototype ( "int", "name_qualification_length", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (5/12/2011): Added information required for new name qualification support.
    // FunctionRefExp.setDataPrototype("bool","type_elaboration_required","= false",
    //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    FunctionRefExp.setDataPrototype("bool","type_elaboration_required","= false",
                                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (5/12/2011): Added information required for new name qualification support.
    // FunctionRefExp.setDataPrototype("bool","global_qualification_required","= false",
    //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    FunctionRefExp.setDataPrototype("bool","global_qualification_required","= false",
                                    NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    FunctionRefExp.setFunctionSource         ( "SOURCE_GET_TYPE_FROM_SYMBOL","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (GreaterOrEqualOp,       "GreaterOrEqualOp",       "GE_OP" );
    GreaterOrEqualOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                         "../Grammar/Expression.code" );
    GreaterOrEqualOp.editSubstitute( "PRECEDENCE_VALUE", "10" );
    GreaterOrEqualOp.setFunctionPrototype ( "HEADER_GREATER_OR_EQUAL_OPERATOR", "../Grammar/Expression.code" );
    GreaterOrEqualOp.editSubstitute ( "HEADER_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "HEADER_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );
    GreaterOrEqualOp.setFunctionSource ( "SOURCE_GREATER_OR_EQUAL_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
    GreaterOrEqualOp.editSubstitute ( "SOURCE_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "SOURCE_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (GreaterThanOp,          "GreaterThanOp",          "GT_OP" );
    GreaterThanOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                      "../Grammar/Expression.code" );
    GreaterThanOp.editSubstitute   ( "PRECEDENCE_VALUE", "10" );
    GreaterThanOp.setFunctionPrototype ( "HEADER_GREATER_THAN_OPERATOR", "../Grammar/Expression.code" );
    GreaterThanOp.editSubstitute    ( "HEADER_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "HEADER_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );
    GreaterThanOp.setFunctionSource    ( "SOURCE_GREATER_THAN_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
    GreaterThanOp.editSubstitute    ( "SOURCE_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "SOURCE_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if USE_X10_IR_NODES
    NEW_TERMINAL_MACRO (HereExp,    "HereExp",    "HERE_NODE"   );
    HereExp.setDataPrototype            ( "SgType*", "expression_type", "= NULL",
                                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    HereExp.setDataPrototype            ( "SgExpression*", "expression", "= NULL",
                                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (ImagPartOp,             "ImagPartOp",             "IMAG_PART_OP" );
    ImagPartOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                   "../Grammar/Expression.code" );
    ImagPartOp.editSubstitute      ( "PRECEDENCE_VALUE", "15" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if USE_FORTRAN_IR_NODES
    NEW_TERMINAL_MACRO (ImpliedDo,              "ImpliedDo",              "IMPLIED_DO" );
    ImpliedDo.editSubstitute           ( "PRECEDENCE_VALUE", " 2" );
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
    // DQ (10/2/2010): Added scope to hold the SgVariableDeclaration and support nested scopes of
    // multi-dimensional implied do loops. However this scope should not be traversed since its
    // purpose is to hold a symbol table, but we don't want just have a symbol table directly.
    ImpliedDo.setDataPrototype     ( "SgScopeStatement*", "implied_do_scope", "= NULL",
                                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (IntegerDivideAssignOp,  "IntegerDivideAssignOp",  "IDIV_ASSIGN_OP" );
    IntegerDivideAssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                              "../Grammar/Expression.code" );
    IntegerDivideAssignOp.editSubstitute  ( "PRECEDENCE_VALUE", " 2" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (IntegerDivideOp,        "IntegerDivideOp",        "INTEGER_DIV_OP" );
    IntegerDivideOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                        "../Grammar/Expression.code" );
    IntegerDivideOp.editSubstitute ( "PRECEDENCE_VALUE", "13" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (IntVal,                 "IntVal",                 "INT_VAL" );
    IntVal.setFunctionSource           ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    IntVal.setDataPrototype ( "int", "value", "= 0",
                              CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    IntVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
                              CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    IntVal.setFunctionSource                 ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
    IntVal.editSubstitute                 ( "GENERIC_TYPE", "SgTypeInt" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if USE_FORTRAN_IR_NODES
    NEW_TERMINAL_MACRO (IOItemExpression,       "IOItemExpression",       "IO_ITEM_EXPR" );
    IOItemExpression.editSubstitute    ( "PRECEDENCE_VALUE", " 2" );
    IOItemExpression.setDataPrototype ( "SgName", "name", "= \"\"",
                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    IOItemExpression.setDataPrototype ( "SgExpression*", "io_item", "= NULL",
                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    // IOItemExpression.setFunctionSource ( "SOURCE_EMPTY_SET_TYPE_FUNCTION", "../Grammar/Expression.code" );
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (IorAssignOp,            "IorAssignOp",            "IOR_ASSIGN_OP" );
    IorAssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                    "../Grammar/Expression.code" );
    IorAssignOp.editSubstitute     ( "PRECEDENCE_VALUE", " 2" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (IsNotOp,                   "IsNotOp",                      "IS_NOT_OP" );
    IsNotOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                "../Grammar/Expression.code" );
    IsNotOp.editSubstitute         ( "PRECEDENCE_VALUE", " 9" );
    IsNotOp.setFunctionPrototype ( "HEADER_IS_NOT_OP", "../Grammar/Expression.code" );
    IsNotOp.editSubstitute          ( "HEADER_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "HEADER_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );
    IsNotOp.setFunctionSource          ( "SOURCE_IS_NOT_OP","../Grammar/Expression.code" );
    IsNotOp.editSubstitute       ( "SOURCE_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "SOURCE_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (IsOp,                      "IsOp",                         "IS_OP" );
    IsOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                             "../Grammar/Expression.code" );
    IsOp.editSubstitute            ( "PRECEDENCE_VALUE", " 9" );
    IsOp.setFunctionPrototype ( "HEADER_IS_OP", "../Grammar/Expression.code" );
    IsOp.editSubstitute             ( "HEADER_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "HEADER_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );
    IsOp.setFunctionSource             ( "SOURCE_IS_OP","../Grammar/Expression.code" );
    IsOp.editSubstitute          ( "SOURCE_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "SOURCE_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (JavaInstanceOfOp,             "JavaInstanceOfOp",             "JAVA_INSTANCEOF_OP" );
    JavaInstanceOfOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                         "../Grammar/Expression.code" );
    // DQ (7/18/2011): What is the precedence of this operator?
    JavaInstanceOfOp.editSubstitute        ( "PRECEDENCE_VALUE", "16" );
    // DQ (7/18/2011): This is structurally similar to the SizeOfOp in that it takes a type operand
    // and we have to save the expression type explicitly (I think).
    JavaInstanceOfOp.setFunctionPrototype ( "HEADER_JAVA_INSTANCEOF_OPERATOR", "../Grammar/Expression.code" );
    JavaInstanceOfOp.setDataPrototype ( "SgExpression*", "operand_expr", "= NULL",
                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    JavaInstanceOfOp.setDataPrototype ( "SgType*", "operand_type", "= NULL",
                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    JavaInstanceOfOp.setDataPrototype ( "SgType*", "expression_type", "= NULL",
                                        CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DQ (1/13/2014): Added Java support for Java annotations.
    NEW_TERMINAL_MACRO (JavaMarkerAnnotation,         "JavaMarkerAnnotation",         "JAVA_MARKER_ANNOTATION" );
    JavaMarkerAnnotation.setFunctionSource       ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    JavaMarkerAnnotation.editSubstitute       ( "PRECEDENCE_VALUE", "16" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (JavaNormalAnnotation,         "JavaNormalAnnotation",         "JAVA_NORMAL_ANNOTATION" );
    JavaNormalAnnotation.setFunctionSource       ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    JavaNormalAnnotation.editSubstitute       ( "PRECEDENCE_VALUE", "16" );
    // DQ (1/13/2014): Added Java support for Java annotations.
    JavaNormalAnnotation.setFunctionPrototype ( "HEADER_JAVA_NORMAL_ANNOTATION", "../Grammar/Expression.code" );
    JavaNormalAnnotation.setDataPrototype ( "SgJavaMemberValuePairPtrList", "value_pair_list", "",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    JavaNormalAnnotation.editSubstitute   ( "HEADER_LIST_DECLARATIONS", "HEADER_LIST_FUNCTIONS", "../Grammar/Expression.code" );
    JavaNormalAnnotation.editSubstitute   ( "LIST_NAME", "value_pair" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (JavaSingleMemberAnnotation,   "JavaSingleMemberAnnotation",   "JAVA_SINGLE_MEMBER_ANNOTATION" );
    JavaSingleMemberAnnotation.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    JavaSingleMemberAnnotation.editSubstitute ( "PRECEDENCE_VALUE", "16" );
    // DQ (1/13/2014): Added Java support for Java annotations.
    JavaSingleMemberAnnotation.setDataPrototype ( "SgExpression*", "value", "= NULL",
                                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (JavaTypeExpression, "JavaTypeExpression", "JAVA_TYPE_EXPRESSION");
    // DQ (3/7/2014): We want to use the automatically generated access function instead (so I think I need to include this).
    JavaTypeExpression.excludeFunctionPrototype        ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );
    JavaTypeExpression.excludeSubTreeFunctionPrototype ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );
    JavaTypeExpression.setFunctionSource         ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    JavaTypeExpression.editSubstitute         ( "PRECEDENCE_VALUE", "16" );
    JavaTypeExpression.setFunctionPrototype ( "HEADER_JAVA_TYPE_EXPRESSION", "../Grammar/Expression.code" );
    // DQ (3/23/2017): We need to change the name to simplify the support for the virtual get_type()
    // function elsewhere in ROSE (and to support the "override" keyword).
    // DQ (3/7/2014): Added support to build access functions for type to be reset in snippet support.
    JavaTypeExpression.setDataPrototype ( "SgType*", "type", "= NULL",
                                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // JavaTypeExpression.setDataPrototype ( "SgType*", "internal_type", "= NULL",
    //        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DQ (7/17/2011): Added this function to support new Java ">>>" operator.
    NEW_TERMINAL_MACRO (JavaUnsignedRshiftAssignOp, "JavaUnsignedRshiftAssignOp", "JAVA_UNSIGNED_RSHIFT_ASSIGN_OP" );
    JavaUnsignedRshiftAssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                                   "../Grammar/Expression.code" );
    JavaUnsignedRshiftAssignOp.editSubstitute  ( "PRECEDENCE_VALUE", " 2" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DQ (7/17/2011): Added this function to support new Java ">>>" operator.
    NEW_TERMINAL_MACRO (JavaUnsignedRshiftOp,   "JavaUnsignedRshiftOp",   "JAVA_UNSIGNED_RSHIFT_OP" );
    JavaUnsignedRshiftOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                             "../Grammar/Expression.code" );
    JavaUnsignedRshiftOp.editSubstitute        ( "PRECEDENCE_VALUE", "11" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Rasmussen (04/09/2021) Used for specifying a value at a position/location in a Jovial table
    NEW_TERMINAL_MACRO (JovialPresetPositionExp, "JovialPresetPositionExp", "JOVIAL_PRESET_POSITION_EXP" );
    // Rasmussen (4/9/2021): Used in Jovial table initialization.
    JovialPresetPositionExp.setFunctionPrototype ("HEADER_JOVIAL_PRESET_POSITION_EXP", "../Grammar/Expression.code");
    JovialPresetPositionExp.setDataPrototype("SgExprListExp*", "indices", "= NULL", CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    JovialPresetPositionExp.setDataPrototype("SgExpression*", "value", "= NULL", CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Rasmussen (06/24/2020) Initializer for Jovial tables (wraps SgExprListExp, useful for unparsing/analysis)
    NEW_TERMINAL_MACRO (JovialTablePresetExp, "JovialTablePresetExp", "JOVIAL_TABLE_PRESET_EXP" );
    // Rasmussen (6/24/2020): An initializer for Jovial tables
    JovialTablePresetExp.setDataPrototype("SgExprListExp*", "preset_list", "= NULL",   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (KeyDatumPair,              "KeyDatumPair",                 "KEY_DATUM_PAIR" );
    KeyDatumPair.setFunctionSource     ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    KeyDatumPair.setDataPrototype ("SgExpression*", "key", "= NULL",
                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    KeyDatumPair.setDataPrototype ("SgExpression*", "datum", "= NULL",
                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    KeyDatumPair.setFunctionSource     ( "SOURCE_DEFAULT_GET_TYPE","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DQ (12/30/2007): New IR node to support references to labels (much like references to variables
    // for the VarRefExp, but with a internal pointer ot a SgLabelSymbol) This IR nodes will eventually
    // be used in the C/C++ goto, but is introduced to hnadle references to labels in Fortran (e.g. format
    // expression in "read" and "write" statements, and a cleanr implementation of references to labels in
    // Fortran loop constructs etc.).
    NEW_TERMINAL_MACRO (LabelRefExp,            "LabelRefExp",            "LABEL_REF" );
    LabelRefExp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                    "../Grammar/Expression.code" );
    LabelRefExp.setFunctionPrototype ( "HEADER_LABEL_REF_EXPRESSION", "../Grammar/Expression.code" );
    LabelRefExp.setDataPrototype ( "SgLabelSymbol*", "symbol", "= NULL",
                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    LabelRefExp.setFunctionSource            ( "SOURCE_GET_TYPE_FROM_SYMBOL","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DQ (9/2/2014): Adding support for C++11 Lambda expressions.
    NEW_TERMINAL_MACRO (LambdaExp,      "LambdaExp",  "LAMBDA_EXP" );
    // DQ (9/2/2014): Adding support for C++11 lambda functions.
    LambdaExp.setFunctionSource( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    // DQ (9/2/2014): Adding support for C++11 lambda expresions.
    LambdaExp.editSubstitute ( "PRECEDENCE_VALUE", "16" );
    // DQ (9/2/2014): Adding support for C++11 lambda expresions.
    LambdaExp.setDataPrototype ("SgLambdaCaptureList*", "lambda_capture_list", "= NULL",
                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

    // DQ (2/15/2015): This will call cycles in the AST if it is allowed to be defined in the AST traversal.
    // LambdaExp.setDataPrototype ("SgClassDeclaration*", "lambda_closure_class", "= NULL",
    //             CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    // DQ (4/27/2017): I think it might be better to traverse the class directly and mark the operator()
    // member function to not be traversed via the lambda function (becasue it is in the lambda closure class).
    LambdaExp.setDataPrototype ("SgClassDeclaration*", "lambda_closure_class", "= NULL",
                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    // DQ (4/27/2017): This points to the non defining declaration, so it is OK to traverse it.
    LambdaExp.setDataPrototype ("SgFunctionDeclaration*", "lambda_function", "= NULL",
                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    LambdaExp.setDataPrototype ( "bool", "is_mutable", "= false",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    LambdaExp.setDataPrototype ( "bool", "capture_default", "= false",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    LambdaExp.setDataPrototype ( "bool", "default_is_by_reference", "= false",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    LambdaExp.setDataPrototype ( "bool", "explicit_return_type", "= false",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    LambdaExp.setDataPrototype ( "bool", "has_parameter_decl", "= false",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (1/23/2016): Modified to support specification of __device__ or __atttribute((device))
    // This is included to support a concept that Jeff Keasler would like to have (experimental).
    // This implementation still needs to be reviewed.
    LambdaExp.setDataPrototype ( "bool", "is_device", "= false",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // driscoll6 (6/27/11) Support for Python
    NEW_TERMINAL_MACRO (LambdaRefExp,              "LambdaRefExp",                 "LAMBDA_REF_EXP" );
    LambdaRefExp.editSubstitute ( "PRECEDENCE_VALUE", " 0" );
    // driscoll6 (6/27/11): Python support
    LambdaRefExp.setFunctionPrototype ( "HEADER_LAMBDA_REF_EXP", "../Grammar/Expression.code" );
    LambdaRefExp.setDataPrototype ("SgFunctionDeclaration*", "functionDeclaration", "= NULL",
                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (LeftDivideOp, "LeftDivideOp", "LEFT_DIVIDE_OP");
    LeftDivideOp.setFunctionSource          ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    LeftDivideOp.editSubstitute ( "PRECEDENCE_VALUE", "13" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (LessOrEqualOp,          "LessOrEqualOp",          "LE_OP" );
    LessOrEqualOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                      "../Grammar/Expression.code" );
    LessOrEqualOp.editSubstitute   ( "PRECEDENCE_VALUE", "10" );
    LessOrEqualOp.setFunctionPrototype ( "HEADER_LESS_OR_EQUAL_OPERATOR", "../Grammar/Expression.code" );
    LessOrEqualOp.editSubstitute    ( "HEADER_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "HEADER_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );
    LessOrEqualOp.setFunctionSource    ( "SOURCE_LESS_OR_EQUAL_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
    LessOrEqualOp.editSubstitute    ( "SOURCE_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "SOURCE_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (LessThanOp,             "LessThanOp",             "LT_OP" );
    LessThanOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                   "../Grammar/Expression.code" );
    LessThanOp.editSubstitute      ( "PRECEDENCE_VALUE", "10" );
    LessThanOp.setFunctionPrototype ( "HEADER_LESS_THAN_OPERATOR", "../Grammar/Expression.code" );
    LessThanOp.editSubstitute       ( "HEADER_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "HEADER_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );
    LessThanOp.setFunctionSource       ( "SOURCE_LESS_THAN_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
    LessThanOp.editSubstitute       ( "SOURCE_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "SOURCE_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (ListComprehension,         "ListComprehension",            "LIST_COMPREHENSION" );
    ListComprehension.setFunctionSource       ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    ListComprehension.setDataPrototype ("SgExpression*", "element", "= NULL",
                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    ListComprehension.setDataPrototype ("SgExprListExp*", "generators", "= NULL",
                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    ListComprehension.setFunctionSource     ( "SOURCE_DEFAULT_GET_TYPE","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (ListExp,                   "ListExp",                      "LIST_EXP" );
    ListExp.setFunctionSource          ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    ListExp.setFunctionSource      ( "SOURCE_DEFAULT_GET_TYPE","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (LongDoubleVal,          "LongDoubleVal",          "LONG_DOUBLE_VAL" );
    LongDoubleVal.setFunctionSource    ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    LongDoubleVal.setDataPrototype ( "long double", "value", "= 0.0",
                                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // DQ (11/9/2005): Added string to hold source code constant precisely (part of work with Andreas)
    LongDoubleVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
                                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    LongDoubleVal.setFunctionSource          ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
    LongDoubleVal.editSubstitute          ( "GENERIC_TYPE", "SgTypeLongDouble" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (LongIntVal,             "LongIntVal",             "LONG_INT_VAL" );
    LongIntVal.setFunctionSource       ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    LongIntVal.setDataPrototype ( "long int", "value", "= 0",
                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    LongIntVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    LongIntVal.setFunctionSource             ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
    LongIntVal.editSubstitute             ( "GENERIC_TYPE", "SgTypeLong" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (LongLongIntVal,         "LongLongIntVal",         "LONG_LONG_INT_VAL" );
    LongLongIntVal.setFunctionSource   ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    LongLongIntVal.setDataPrototype ( "long long int", "value", "= 0",
                                      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    LongLongIntVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
                                      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    LongLongIntVal.setFunctionSource         ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
    // LongLongIntVal should return a value of type TypeLongLong (I think!)
    LongLongIntVal.editSubstitute         ( "GENERIC_TYPE", "SgTypeLong" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (LshiftAssignOp,         "LshiftAssignOp",         "LSHIFT_ASSIGN_OP" );
    LshiftAssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                       "../Grammar/Expression.code" );
    LshiftAssignOp.editSubstitute  ( "PRECEDENCE_VALUE", " 2" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (LshiftOp,               "LshiftOp",               "LSHIFT_OP" );
    LshiftOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                 "../Grammar/Expression.code" );
    // DQ (1/26/2013): I think this is wrong, "<<" and ">>" have value 7 (lower than "==") (see test2013_42.C).
    // I think this value of 7 is incorrect since it is from a table that lists values in reverse order from how
    // we list then here.  Here we are following the apendix of the C++ language book.
    // LshiftOp.editSubstitute        ( "PRECEDENCE_VALUE", "11" );
    // RshiftOp.editSubstitute        ( "PRECEDENCE_VALUE", "11" );
    LshiftOp.editSubstitute        ( "PRECEDENCE_VALUE", "11" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Sk (07/16/2015) This Expression represents a : in Matlab also called magic colon
    NEW_TERMINAL_MACRO (MagicColonExp, "MagicColonExp", "MAGIC_COLON_EXP");
    MagicColonExp.setFunctionSource          ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    MagicColonExp.setFunctionSource ( "SOURCE_DEFAULT_GET_TYPE","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //SK (06/23/2015) SgMatrixExp for Matlab Matrix
    NEW_TERMINAL_MACRO (MatrixExp, "MatrixExp", "MATRIX_EXP");
    MatrixExp.setFunctionSource          ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (MatrixTransposeOp, "MatrixTransposeOp", "MATRIX_TRANSPOSE_OP");
    MatrixTransposeOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                          "../Grammar/Expression.code" );
    MatrixTransposeOp.editSubstitute ( "PRECEDENCE_VALUE", "15" );
    MatrixTransposeOp.setDataPrototype("bool", "is_conjugate", "= false",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (MemberFunctionRefExp,         "MemberFunctionRefExp",         "MEMBER_FUNCTION_REF" );
    MemberFunctionRefExp.setFunctionPrototype ( "HEADER_MEMBER_FUNCTION_REF_EXPRESSION", "../Grammar/Expression.code" );
    MemberFunctionRefExp.setDataPrototype ( "SgMemberFunctionSymbol*", "symbol_i", "= NULL",
                                            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    MemberFunctionRefExp.setDataPrototype ( "int", "virtual_call", "= 0",
                                            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // DQ (1/14/2006): The function type should be computed from the function declaration (instead of being stored)
    // Leave the type in the constructor for storage internally and build a special version of get_type() to access
    // this value or later compute it directly.
    // MemberFunctionRefExp.setDataPrototype ( "SgFunctionType*", "function_type", "= NULL",
    //        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    MemberFunctionRefExp.setDataPrototype ( "SgFunctionType*", "function_type", "= NULL",
                                            CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (4/13/2004): Changed false to true in default setting (and removed resetting of value in
    //                 post_constructor_initialization(), as suggested by Qing).
    MemberFunctionRefExp.setDataPrototype ( "int", "need_qualifier", "= true",
                                            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (5/12/2011): Added support for name qualification.
    // MemberFunctionRefExp.setDataPrototype ( "int", "name_qualification_length", "= 0",
    //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    MemberFunctionRefExp.setDataPrototype ( "int", "name_qualification_length", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (5/12/2011): Added information required for new name qualification support.
    // MemberFunctionRefExp.setDataPrototype("bool","type_elaboration_required","= false",
    //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    MemberFunctionRefExp.setDataPrototype("bool","type_elaboration_required","= false",
                                          NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (5/12/2011): Added information required for new name qualification support.
    // MemberFunctionRefExp.setDataPrototype("bool","global_qualification_required","= false",
    //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    MemberFunctionRefExp.setDataPrototype("bool","global_qualification_required","= false",
                                          NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // DQ (1/14/2006): The function type should be computed from the function declaration (instead of being stored)
    // Leave the type in the constructor for storage internally and build a special version of get_type() to access
    // this value or later compute it directly.
    // MemberFunctionRefExp.setDataPrototype ( "SgFunctionType*", "function_type", "= NULL",
    //        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    MemberFunctionRefExp.setFunctionSource   ( "SOURCE_GET_TYPE_FROM_SYMBOL","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (MembershipOp,              "MembershipOp",                 "MEMBERSHIP_OP" );
    MembershipOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                     "../Grammar/Expression.code" );
    MembershipOp.editSubstitute    ( "PRECEDENCE_VALUE", " 9" );
    MembershipOp.setFunctionPrototype ( "HEADER_MEMBERSHIP_OP", "../Grammar/Expression.code" );
    MembershipOp.editSubstitute     ( "HEADER_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "HEADER_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );
    MembershipOp.setFunctionSource     ( "SOURCE_MEMBERSHIP_OP","../Grammar/Expression.code" );
    MembershipOp.editSubstitute  ( "SOURCE_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "SOURCE_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (MinusAssignOp,          "MinusAssignOp",          "MINUS_ASSIGN_OP" );
    MinusAssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                      "../Grammar/Expression.code" );
    MinusAssignOp.editSubstitute   ( "PRECEDENCE_VALUE", " 2" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (MinusMinusOp,           "MinusMinusOp",           "MINUSMINUS_OP" );
    MinusMinusOp.editSubstitute    ( "PRECEDENCE_VALUE", "15" );
    MinusMinusOp.setFunctionPrototype ( "HEADER_MINUS_MINUS_OPERATOR", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (MinusOp,                "MinusOp",                "UNARY_MINUS_OP" );
    MinusOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                "../Grammar/Expression.code" );
    MinusOp.editSubstitute         ( "PRECEDENCE_VALUE", "15" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (ModAssignOp,            "ModAssignOp",            "MOD_ASSIGN_OP" );
    ModAssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                    "../Grammar/Expression.code" );
    ModAssignOp.editSubstitute     ( "PRECEDENCE_VALUE", " 2" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (ModOp,                  "ModOp",                  "MOD_OP" );
    ModOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                              "../Grammar/Expression.code" );
    ModOp.editSubstitute           ( "PRECEDENCE_VALUE", "13" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (MultAssignOp,           "MultAssignOp",           "MULT_ASSIGN_OP" );
    MultAssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                     "../Grammar/Expression.code" );
    MultAssignOp.editSubstitute    ( "PRECEDENCE_VALUE", " 2" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (MultiplyOp,             "MultiplyOp",             "MULT_OP" );
    MultiplyOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                   "../Grammar/Expression.code" );
    MultiplyOp.editSubstitute      ( "PRECEDENCE_VALUE", "13" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (NaryBooleanOp,          "NaryBooleanOp",          "NARY_BOOLEAN_OP");
    NaryBooleanOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                      "../Grammar/Expression.code" );
    NaryBooleanOp.editSubstitute    ( "PRECEDENCE_VALUE", "13" );
    NaryBooleanOp.setFunctionPrototype ( "HEADER_NARY_BOOLEAN_OP", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // driscoll6 (7/20/11): Support for n-ary operators in python
    NEW_TERMINAL_MACRO (NaryComparisonOp,       "NaryComparisonOp",       "NARY_COMPARISON_OP");
    NaryComparisonOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                         "../Grammar/Expression.code" );
    NaryComparisonOp.editSubstitute ( "PRECEDENCE_VALUE", "13" );
    NaryComparisonOp.setFunctionPrototype ( "HEADER_NARY_COMPARISON_OP", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (NewExp,                 "NewExp",                 "NEW_OP" );
    NewExp.setFunctionPrototype ( "HEADER_GET_NEXT_EXPRESSION", "../Grammar/Expression.code" );
    NewExp.setFunctionPrototype ( "HEADER_NEW_OPERATOR_EXPRESSION", "../Grammar/Expression.code" );
    // DQ (1/14/2006): We should not store the type of unary operators but instead obtain it from the operand directly.
    // NewExp.setDataPrototype     ( "SgType*", "expression_type", "= NULL",
    //        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // NewExp.setDataPrototype     ( "SgType*", "expression_type", "= NULL",
    //        CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    NewExp.setDataPrototype     ( "SgType*", "specified_type", "= NULL",
                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
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

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (6/2/2011): Added support for name qualification.
    // NewExp.setDataPrototype ( "int", "name_qualification_length", "= 0",
    //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    NewExp.setDataPrototype ( "int", "name_qualification_length", "= 0",
                              NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (6/2/2011): Added information required for new name qualification support.
    // NewExp.setDataPrototype("bool","type_elaboration_required","= false",
    //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    NewExp.setDataPrototype("bool","type_elaboration_required","= false",
                            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (6/2/2011): Added information required for new name qualification support.
    // NewExp.setDataPrototype("bool","global_qualification_required","= false",
    //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    NewExp.setDataPrototype("bool","global_qualification_required","= false",
                            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (4/15/2019): This is needed to support pointers to member type specified to the new operator.
    NewExp.setDataPrototype ( "int", "name_qualification_for_pointer_to_member_class_length", "= 0",
                              NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (4/15/2019): This is needed to support pointers to member type specified to the new operator.
    NewExp.setDataPrototype("bool","type_elaboration_for_pointer_to_member_class_required","= false",
                            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (4/15/2019): This is needed to support pointers to member type specified to the new operator.
    NewExp.setDataPrototype("bool","global_qualification_for_pointer_to_member_class_required","= false",
                            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DQ (2/4/2015): Added C++11 noexcept operator.
    NEW_TERMINAL_MACRO (NoexceptOp,                   "NoexceptOp",                   "NOEXCEPT_OP" );
    NoexceptOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                   "../Grammar/Expression.code" );
    NoexceptOp.editSubstitute      ( "PRECEDENCE_VALUE", "16" );
    // DQ (2/4/2015): Added noexecpt operator.
    NoexceptOp.setFunctionPrototype ( "HEADER_NOEXCEPT_OPERATOR", "../Grammar/Expression.code" );
    NoexceptOp.setDataPrototype ( "SgExpression*", "operand_expr", "= NULL",
                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (NonMembershipOp,           "NonMembershipOp",              "NON_MEMBERSHIP_OP" );
    NonMembershipOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                        "../Grammar/Expression.code" );
    NonMembershipOp.editSubstitute ( "PRECEDENCE_VALUE", " 9" );
    NonMembershipOp.setFunctionPrototype ( "HEADER_NON_MEMBERSHIP_OP", "../Grammar/Expression.code" );
    NonMembershipOp.editSubstitute  ( "HEADER_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "HEADER_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );
    NonMembershipOp.setFunctionSource  ( "SOURCE_NON_MEMBERSHIP_OP","../Grammar/Expression.code" );
    NonMembershipOp.editSubstitute  ( "SOURCE_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "SOURCE_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (NonrealRefExp,          "NonrealRefExp",          "NONREAL_REF" );
    NonrealRefExp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                      "../Grammar/Expression.code" );
    NonrealRefExp.setFunctionPrototype ( "HEADER_NONREAL_REF_EXPRESSION", "../Grammar/Expression.code" );
    NonrealRefExp.setDataPrototype ( "SgNonrealSymbol*", "symbol", "= NULL",
                                     CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    NonrealRefExp.setDataPrototype ( "int", "name_qualification_length", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    NonrealRefExp.setDataPrototype ("bool","type_elaboration_required","= false",
                                    NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    NonrealRefExp.setDataPrototype ("bool","global_qualification_required","= false",
                                    NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    NonrealRefExp.setFunctionSource          ( "SOURCE_GET_TYPE_FROM_SYMBOL","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (NotOp,                  "NotOp",                  "NOT_OP" );
    NotOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                              "../Grammar/Expression.code" );
    NotOp.editSubstitute           ( "PRECEDENCE_VALUE", "15" );
    NotOp.setFunctionPrototype    ( "HEADER_NOT_OPERATOR", "../Grammar/Expression.code" );
    NotOp.setFunctionSource            ( "SOURCE_NOT_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
    NotOp.editSubstitute            ( "SOURCE_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "SOURCE_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (NotEqualOp,             "NotEqualOp",             "NE_OP" );
    NotEqualOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                   "../Grammar/Expression.code" );
    NotEqualOp.editSubstitute      ( "PRECEDENCE_VALUE", " 9" );
    NotEqualOp.setFunctionPrototype ( "HEADER_NOT_EQUAL_OPERATOR", "../Grammar/Expression.code" );
    NotEqualOp.editSubstitute       ( "HEADER_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "HEADER_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );
    NotEqualOp.setFunctionSource       ( "SOURCE_NOT_EQUAL_OPERATOR_EXPRESSION","../Grammar/Expression.code" );
    NotEqualOp.editSubstitute       ( "SOURCE_BOOLEAN_GET_TYPE_MEMBER_FUNCTION", "SOURCE_BOOLEAN_GET_TYPE", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DQ (12/13/2005): Added support for empty expression (and empty statement).
    NEW_TERMINAL_MACRO (NullExpression,         "NullExpression",             "NULL_EXPR" );
    NullExpression.setFunctionSource   ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    NullExpression.editSubstitute    ( "PRECEDENCE_VALUE", "16" );
    NullExpression.setFunctionPrototype    ( "HEADER_NULL_EXPRESSION", "../Grammar/Expression.code" );
    // NullExpression.setDataPrototype        ( "SgType*", "expression_type", "= NULL",
    //                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // NullExpression.setFunctionSource    ( "SOURCE_DEFAULT_GET_TYPE", "../Grammar/Expression.code" );
    // NullExpression.setFunctionSource    ( "SOURCE_EMPTY_SET_TYPE_FUNCTION", "../Grammar/Expression.code" );
    NullExpression.setFunctionSource    ( "SOURCE_GET_TYPE_GENERIC", "../Grammar/Expression.code" );
    NullExpression.editSubstitute       ( "GENERIC_TYPE", "SgTypeDefault" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DQ (7/31/2014): Added support for C++11 nullptr constant value expression (using type nullptr_t).
    NEW_TERMINAL_MACRO (NullptrValExp,          "NullptrValExp",          "NULLPTR_VAL" );
    NullptrValExp.setFunctionSource    ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    // DQ (7/31/2014): Adding support for nullptr constant value expression.
    // DQ (7/31/2014): I don't think this need a value.
    // NullptrValExp.setDataPrototype ( "SgNullptrType*", "value", "= 0",
    //                               CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    NullptrValExp.setFunctionSource          ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
    NullptrValExp.editSubstitute  ( "GENERIC_TYPE", "SgTypeNullptr" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (OrOp,                   "OrOp",                   "OR_OP" );
    OrOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                             "../Grammar/Expression.code" );
    OrOp.editSubstitute            ( "PRECEDENCE_VALUE", " 4" );
    OrOp.setFunctionPrototype ( "HEADER_OR_OPERATOR", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (PlusAssignOp,           "PlusAssignOp",           "PLUS_ASSIGN_OP" );
    PlusAssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                     "../Grammar/Expression.code" );
    PlusAssignOp.editSubstitute    ( "PRECEDENCE_VALUE", " 2" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (PlusPlusOp,             "PlusPlusOp",             "PLUSPLUS_OP" );
    PlusPlusOp.editSubstitute      ( "PRECEDENCE_VALUE", "15" );
    PlusPlusOp.setFunctionPrototype ( "HEADER_PLUS_PLUS_OPERATOR", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (PntrArrRefExp,          "PntrArrRefExp",          "ARRAY_OP" );
    PntrArrRefExp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                      "../Grammar/Expression.code" );
    PntrArrRefExp.editSubstitute   ( "PRECEDENCE_VALUE", "16" );
    PntrArrRefExp.setFunctionPrototype ( "HEADER_POINTER_ARRAY_REFERENCE_EXPRESSION", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if USE_FORTRAN_IR_NODES
    // DQ (1/31/2009): Added Fortran pointer assignment operator (to support pointer assignment statement).
    NEW_TERMINAL_MACRO (PointerAssignOp,       "PointerAssignOp",       "POINTER_ASSIGN_OP" );
#endif
    PointerAssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                        "../Grammar/Expression.code" );
    PointerAssignOp.editSubstitute ( "PRECEDENCE_VALUE", " 2" );
    PointerAssignOp.setFunctionPrototype ( "HEADER_POINTER_ASSIGNMENT_OPERATOR", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (PointerDerefExp,        "PointerDerefExp",        "DEREF_OP" );
    PointerDerefExp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                        "../Grammar/Expression.code" );
    PointerDerefExp.editSubstitute ( "PRECEDENCE_VALUE", "15" );
    PointerDerefExp.setFunctionPrototype ( "HEADER_POINTER_DEREF_EXPRESSION", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (PowerOp, "PowerOp", "POWER_OP");
    PowerOp.setFunctionSource          ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    PowerOp.editSubstitute ( "PRECEDENCE_VALUE", "14" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (PseudoDestructorRefExp, "PseudoDestructorRefExp", "PSEUDO_DESTRUCTOR_REF");
#if USE_FORTRAN_IR_NODES
    PseudoDestructorRefExp.editSubstitute ( "PRECEDENCE_VALUE", " 2" );
    PseudoDestructorRefExp.setFunctionPrototype ( "HEADER_PSEUDO_DESTRUCTOR_REF", "../Grammar/Expression.code" );
    PseudoDestructorRefExp.setDataPrototype ( "SgType*", "object_type", "= NULL",
                                              CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    PseudoDestructorRefExp.setDataPrototype ( "SgType*", "expression_type", "= NULL",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (1/18/2020): Adding support for name qualification (see Cxx11_tests/test2020_56.C).
    PseudoDestructorRefExp.setDataPrototype ( "int", "name_qualification_length", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (1/18/2020): Adding support for name qualification (see Cxx11_tests/test2020_56.C).
    PseudoDestructorRefExp.setDataPrototype("bool","type_elaboration_required","= false",
                                            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (1/18/2020): Adding support for name qualification (see Cxx11_tests/test2020_56.C).
    PseudoDestructorRefExp.setDataPrototype("bool","global_qualification_required","= false",
                                            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif
    PseudoDestructorRefExp.setFunctionSource ( "SOURCE_GET_TYPE_CLASS_DECL", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //SK (06/25/2015) SgRangeExp for representing a range like 1:5 or 1:2:5 in Matlab
    NEW_TERMINAL_MACRO (RangeExp, "RangeExp", "RANGE_EXP");
    RangeExp.setFunctionSource          ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    RangeExp.setFunctionPrototype ( "HEADER_RANGE_EXP", "../Grammar/Expression.code" );
    RangeExp.setDataPrototype("SgExpression*", "start", "= NULL",
                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    RangeExp.setDataPrototype("SgExpression*", "end", "= NULL",
                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    RangeExp.setDataPrototype("SgExpression*", "stride", "= NULL",
                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    RangeExp.setFunctionSource     ( "SOURCE_DEFAULT_GET_TYPE","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (RealPartOp,             "RealPartOp",             "REAL_PART_OP" );
    RealPartOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                   "../Grammar/Expression.code" );
    RealPartOp.editSubstitute      ( "PRECEDENCE_VALUE", "15" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (RefExp,                 "RefExp",                 "TYPE_REF" );
    RefExp.excludeFunctionPrototype          ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );
    RefExp.setFunctionSource           ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    RefExp.setDataPrototype     ( "SgType*", "type_name", "= NULL",
                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // PP (06/08/2020): Added support for Ada remainder (different from mod) and abs operators
    NEW_TERMINAL_MACRO (RemOp,                  "RemOp",                  "REM_OP" );
    RemOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                              "../Grammar/Expression.code" );
    RemOp.editSubstitute           ( "PRECEDENCE_VALUE", "13" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CR (07/26/2020): Jovial replication operator for initialization expressions, also seen in Fortran
    NEW_TERMINAL_MACRO (ReplicationOp,          "ReplicationOp",          "REPLICATION_OP" );
    ReplicationOp.setFunctionSource   ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                        "../Grammar/Expression.code" );
    ReplicationOp.editSubstitute   ( "PRECEDENCE_VALUE", "13" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (RshiftAssignOp,         "RshiftAssignOp",         "RSHIFT_ASSIGN_OP" );
    RshiftAssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                       "../Grammar/Expression.code" );
    RshiftAssignOp.editSubstitute  ( "PRECEDENCE_VALUE", " 2" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (RshiftOp,               "RshiftOp",               "RSHIFT_OP" );
    RshiftOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                 "../Grammar/Expression.code" );
    RshiftOp.editSubstitute        ( "PRECEDENCE_VALUE", "11" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (ScopeOp,                "ScopeOp",                "SCOPE_OP" );
    ScopeOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                "../Grammar/Expression.code" );
    ScopeOp.editSubstitute         ( "PRECEDENCE_VALUE", "17" ); // highest precedence
    ScopeOp.setFunctionPrototype ( "HEADER_SCOPE_OPERATOR", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (SetComprehension,          "SetComprehension",             "SET_COMPREHENSION" );
    SetComprehension.setFunctionSource        ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    SetComprehension.setDataPrototype ("SgExpression*", "element", "= NULL",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    SetComprehension.setDataPrototype ("SgExprListExp*", "generators", "= NULL",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    SetComprehension.setFunctionSource     ( "SOURCE_DEFAULT_GET_TYPE","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (ShortVal,               "ShortVal",               "SHORT_VAL" );
    ShortVal.setFunctionSource         ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    // URK (08/22/2006): Added string to hold source code constants of integer and character types precisely.
    ShortVal.setDataPrototype ( "short", "value", "= 0",
                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    ShortVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    ShortVal.setFunctionSource               ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
    ShortVal.editSubstitute       ( "GENERIC_TYPE", "SgTypeShort" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (SizeOfOp,                     "SizeOfOp",                     "SIZEOF_OP" );
    SizeOfOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                 "../Grammar/Expression.code" );
    // Now set the precedence values for each leaf of the grammar
    // (where the precedence member function is defined)
    SizeOfOp.editSubstitute        ( "PRECEDENCE_VALUE", "16" );
    SizeOfOp.setFunctionPrototype ( "HEADER_SIZEOF_OPERATOR", "../Grammar/Expression.code" );
    SizeOfOp.setDataPrototype ( "SgExpression*", "operand_expr", "= NULL",
                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    SizeOfOp.setDataPrototype ( "SgType*", "operand_type", "= NULL",
                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // DQ (1/14/2006): We should not store the type of unary operators but instead obtain it from the operand directly.
    // SizeOfOp.setDataPrototype ( "SgType*", "expression_type", "= NULL",
    //        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    SizeOfOp.setDataPrototype ( "SgType*", "expression_type", "= NULL",
                                CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (6/2/2011): Added support for name qualification.
    // SizeOfOp.setDataPrototype ( "int", "name_qualification_length", "= 0",
    //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    SizeOfOp.setDataPrototype ( "int", "name_qualification_length", "= 0",
                                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (6/2/2011): Added information required for new name qualification support.
    // SizeOfOp.setDataPrototype("bool","type_elaboration_required","= false",
    //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    SizeOfOp.setDataPrototype("bool","type_elaboration_required","= false",
                              NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (6/2/2011): Added information required for new name qualification support.
    // SizeOfOp.setDataPrototype("bool","global_qualification_required","= false",
    //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    SizeOfOp.setDataPrototype("bool","global_qualification_required","= false",
                              NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (10/17/2012): Added information to trigger output of the defining declaration of the type (see test2012_57.c).
    // We need to control the output of the defining declaration in some interesting places where it can be specified.
    SizeOfOp.setDataPrototype("bool","sizeOfContainsBaseTypeDefiningDeclaration","= false",
                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (1/12/2019): Adding support for objectless nonstatic data member references (C++11 feature).
    SizeOfOp.setDataPrototype("bool","is_objectless_nonstatic_data_member_reference","= false",
                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (4/15/2019): Let's demonstrate this is possible in the language before we add support for it (see Cxx11_test/test2019_379.C).
    // DQ (4/15/2019): This is needed to support pointers to member type specified to the sizeof operator.
    SizeOfOp.setDataPrototype ( "int", "name_qualification_for_pointer_to_member_class_length", "= 0",
                                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (4/15/2019): This is needed to support pointers to member type specified to the sizeof operator.
    SizeOfOp.setDataPrototype("bool","type_elaboration_for_pointer_to_member_class_required","= false",
                              NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (4/15/2019): This is needed to support pointers to member type specified to the sizeof operator.
    SizeOfOp.setDataPrototype("bool","global_qualification_for_pointer_to_member_class_required","= false",
                              NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DQ (7/25/2020): Adding C++20 spaceship operator support.
    NEW_TERMINAL_MACRO (SpaceshipOp,               "SpaceshipOp",               "SPACESHIP_OP" );
    // DQ (7/25/2020): Adding C++20 support.
    SpaceshipOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                    "../Grammar/Expression.code" );
    // DQ (7/25/2020): Adding C++20 support (need to lookup the correct operator precedence, made it the same as AddOp for now).
    SpaceshipOp.editSubstitute           ( "PRECEDENCE_VALUE", "12" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DQ (7/21/2006): Added support for GNU Statement Expression extension.
    NEW_TERMINAL_MACRO (StatementExpression,    "StatementExpression",        "STMT_EXPR" );
    StatementExpression.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    // DQ (7/21/2006): Added support for GNU statement expression extension.
    StatementExpression.editSubstitute ( "PRECEDENCE_VALUE", "16" );
    StatementExpression.setFunctionPrototype ( "HEADER_STATEMENT_EXPRESSION", "../Grammar/Expression.code" );
    StatementExpression.setDataPrototype     ( "SgStatement*", "statement", "= NULL",
                                               CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (StringConversion,          "StringConversion",              "STR_CONV" );
    StringConversion.setFunctionSource        ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    StringConversion.setDataPrototype ("SgExpression*", "expression", "= NULL",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    StringConversion.setFunctionSource     ( "SOURCE_DEFAULT_GET_TYPE","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (StringVal,              "StringVal",              "STRING_VAL" );
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

    // PP (4/25/21) replace usesSingleQuotes and usesDoubleQuotes with
    //   stringDelimiter (char)
    //~   StringVal.setDataPrototype ( "bool", "usesSingleQuotes", "= false",
    //~            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    //~ // DQ (12/23/2007): Added support for distinguishing double quotes (permits use of sing, double, or un-quoted strings in the SgFormatItem object).
    //~   StringVal.setDataPrototype ( "bool", "usesDoubleQuotes", "= false",
    //~            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    StringVal.setDataPrototype ( "char", "stringDelimiter", "= 0",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (8/13/2014): Added support for C++11 string types (16bit and 32bit character types for strings).
    StringVal.setDataPrototype ( "bool", "is16bitString", "= false",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    StringVal.setDataPrototype ( "bool", "is32bitString", "= false",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    StringVal.setDataPrototype ( "bool", "isRawString", "= false",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    StringVal.setDataPrototype ( "std::string", "raw_string_value", "= \"\"",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // DQ (8/17/2010): types for strings need to be handled using a lenght parameter to the SgTypeString::createType function.
    // For fortran the lenght can be specified as an expression, but for a literal it has to be a known value of an integer.
    // StringVal.setFunctionSource              ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
    StringVal.editSubstitute      ( "GENERIC_TYPE", "SgTypeString" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if USE_FORTRAN_IR_NODES
    NEW_TERMINAL_MACRO (SubscriptExpression,    "SubscriptExpression",    "SUBSCRIPT_EXPR" );
    SubscriptExpression.editSubstitute ( "PRECEDENCE_VALUE", " 2" );
    SubscriptExpression.setFunctionPrototype ( "HEADER_SUBSCRIPT_EXPR_EXPRESSION", "../Grammar/Expression.code" );
    SubscriptExpression.setDataPrototype ( "SgExpression*", "lowerBound", "= NULL",
                                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    SubscriptExpression.setDataPrototype ( "SgExpression*", "upperBound", "= NULL",
                                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    SubscriptExpression.setDataPrototype ( "SgExpression*", "stride", "= NULL",
                                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    // SubscriptExpression.setFunctionSource ( "SOURCE_EMPTY_SET_TYPE_FUNCTION", "../Grammar/Expression.code" );
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (SubtractOp,             "SubtractOp",             "SUBT_OP" );
    SubtractOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                   "../Grammar/Expression.code" );
    SubtractOp.editSubstitute      ( "PRECEDENCE_VALUE", "12" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (SuperExp,               "SuperExp",               "SUPER_NODE" );
    SuperExp.setFunctionSource         ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    SuperExp.setFunctionPrototype ( "HEADER_SUPER_EXPRESSION", "../Grammar/Expression.code" );
    SuperExp.setDataPrototype     ( "SgClassSymbol*", "class_symbol", "= NULL",
                                    CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    SuperExp.setDataPrototype     ( "int", "pobj_super", "= 0",
                                    CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DQ (12/15/2011): Added support for templates into AST.  It might make sense at some point to have
    // function ref expressions have a hierarchy rather than a flat representation as it is now (similar
    // to SgFunctionDeclaration's hierarchy).
    NEW_TERMINAL_MACRO (TemplateFunctionRefExp,       "TemplateFunctionRefExp",       "TEMPLATE_FUNCTION_REF" );
    TemplateFunctionRefExp.setFunctionPrototype ( "HEADER_TEMPLATE_FUNCTION_REF_EXPRESSION", "../Grammar/Expression.code" );
    // TemplateFunctionRefExp.setDataPrototype ( "SgTemplateSymbol*", "symbol_i", "= NULL",
    //                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    TemplateFunctionRefExp.setDataPrototype ( "SgTemplateFunctionSymbol*", "symbol_i", "= NULL",
                                              CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // DQ (1/14/2006): The function type should be computed from the function declaration (instead of being stored)
    // Leave the type in the constructor for storage internally and build a special version of get_type() to access
    // this value or later compute it directly.
    // FunctionRefExp.setDataPrototype ( "SgFunctionType*"  , "function_type", "= NULL",
    //        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // TemplateFunctionRefExp.setDataPrototype ( "SgFunctionType*"  , "function_type", "= NULL",
    //        CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (5/12/2011): Added support for name qualification.
    // TemplateFunctionRefExp.setDataPrototype ( "int", "name_qualification_length", "= 0",
    //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    TemplateFunctionRefExp.setDataPrototype ( "int", "name_qualification_length", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (5/12/2011): Added information required for new name qualification support.
    // TemplateFunctionRefExp.setDataPrototype("bool","type_elaboration_required","= false",
    //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    TemplateFunctionRefExp.setDataPrototype("bool","type_elaboration_required","= false",
                                            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (5/12/2011): Added information required for new name qualification support.
    // TemplateFunctionRefExp.setDataPrototype("bool","global_qualification_required","= false",
    //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    TemplateFunctionRefExp.setDataPrototype("bool","global_qualification_required","= false",
                                            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // DQ (12/15/2011): Adding template declaration support to the AST.
    TemplateFunctionRefExp.setFunctionSource         ( "SOURCE_GET_TYPE_FROM_SYMBOL","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (TemplateMemberFunctionRefExp, "TemplateMemberFunctionRefExp", "TEMPLATE_MEMBER_FUNCTION_REF" );
    // DQ (12/15/2011) Added template declaration support and so we need support for
    // calling template functions and template member functions as expressions.
    // NOTE: These have a similar interface to the SgFunctionRefExp and
    // SgTemplateMemberFunctionRefExp.
    TemplateMemberFunctionRefExp.setFunctionPrototype ( "HEADER_TEMPLATE_MEMBER_FUNCTION_REF_EXPRESSION", "../Grammar/Expression.code" );
    // TemplateMemberFunctionRefExp.setDataPrototype ( "SgTemplateSymbol*", "symbol_i", "= NULL",
    //        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    TemplateMemberFunctionRefExp.setDataPrototype ( "SgTemplateMemberFunctionSymbol*", "symbol_i", "= NULL",
                                                    CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    TemplateMemberFunctionRefExp.setDataPrototype ( "int", "virtual_call", "= 0",
                                                    CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // TemplateMemberFunctionRefExp.setDataPrototype ( "SgFunctionType*", "function_type", "= NULL",
    //        CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // DQ (4/13/2004): Changed false to true in default setting (and removed resetting of value in
    //                 post_constructor_initialization(), as suggested by Qing).
    TemplateMemberFunctionRefExp.setDataPrototype ( "int", "need_qualifier", "= true",
                                                    CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (5/12/2011): Added support for name qualification.
    // TemplateMemberFunctionRefExp.setDataPrototype ( "int", "name_qualification_length", "= 0",
    //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    TemplateMemberFunctionRefExp.setDataPrototype ( "int", "name_qualification_length", "= 0",
                                                    NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (5/12/2011): Added information required for new name qualification support.
    // TemplateMemberFunctionRefExp.setDataPrototype("bool","type_elaboration_required","= false",
    //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    TemplateMemberFunctionRefExp.setDataPrototype("bool","type_elaboration_required","= false",
                                                  NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (5/12/2011): Added information required for new name qualification support.
    // TemplateMemberFunctionRefExp.setDataPrototype("bool","global_qualification_required","= false",
    //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    TemplateMemberFunctionRefExp.setDataPrototype("bool","global_qualification_required","= false",
                                                  NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    TemplateMemberFunctionRefExp.setFunctionSource   ( "SOURCE_GET_TYPE_FROM_SYMBOL","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DQ (11/28/2011): Adding support for template declarations in the AST.
    NEW_TERMINAL_MACRO (TemplateParameterVal,   "TemplateParameterVal",   "TEMPLATE_PARAMETER_VAL" );
    // DQ (8/6/2013): We need to implement this member function explicitly and cannot use the default implementation.
    TemplateParameterVal.excludeFunctionPrototype        ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );
    // DQ (11/28/2011): Adding template declaration support to the AST.
    TemplateParameterVal.setFunctionSource( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    // DQ (11/28/2011): Adding template declaration support in the AST (see test2011_164.C).
    TemplateParameterVal.setFunctionPrototype ( "HEADER_TEMPLATE_PARAMETER_VALUE_EXPRESSION", "../Grammar/Expression.code" );
    TemplateParameterVal.setDataPrototype ( "int", "template_parameter_position", "= -1",
                                            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    TemplateParameterVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
                                            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (8/6/2013): Added explicit representation for type (required to disambiguate overloaded template functions.
    TemplateParameterVal.setDataPrototype ( "SgType*", "valueType", "= NULL",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // DQ (8/6/2013): We need to store the type explicitly and implement this function explicitly to return
    // the explicitly stored type.  This is important to resolving functions overloaded on template parameters,
    // see test2013_303.C for an example.
    // DQ (11/28/2011): Adding template declaration support to the AST.
    // TemplateParameterVal.setFunctionSource   ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
    // DQ (8/6/2013): We need to store the type explicitly and implement this function explicitly to return
    // the explicitly stored type.  This is important to resolving functions overloaded on template parameters,
    // see test2013_303.C for an example.
    // DQ (11/28/2011): Adding template declaration support to the AST.
    // TemplateParameterVal.editSubstitute   ( "GENERIC_TYPE", "SgTemplateType" );
    // TemplateParameterVal.editSubstitute   ( "GENERIC_TYPE", "SgTypeInt" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (ThisExp,                "ThisExp",                "THIS_NODE" );
    ThisExp.setFunctionSource          ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    ThisExp.setFunctionPrototype ( "HEADER_THIS_EXPRESSION", "../Grammar/Expression.code" );
    ThisExp.setDataPrototype     ( "SgClassSymbol*", "class_symbol", "= NULL",
                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    ThisExp.setDataPrototype     ( "SgNonrealSymbol*", "nonreal_symbol", "= NULL",
                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // DQ (1/14/2006): This is a CC++ specific data member, but it is part of the
    // constructor argument list so we will remove it later.
    ThisExp.setDataPrototype     ( "int", "pobj_this", "= 0",
                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (ThrowOp,                "ThrowOp",                "THROW_OP" );
    ThrowOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                "../Grammar/Expression.code" );
    ThrowOp.editSubstitute         ( "PRECEDENCE_VALUE", "15" );
    ThrowOp.setFunctionPrototype ( "HEADER_THROW_OPERATOR", "../Grammar/Expression.code" );
    // DQ (9/19/2004): Added support for rethrow (allows unary operator operand to be NULL without being interpreted as an error)
    ThrowOp.setDataPrototype     ( "SgThrowOp::e_throw_kind", "throwKind", "= SgThrowOp::unknown_throw",
                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (TupleExp,                  "TupleExp",                     "TUPLE_EXP" );
    TupleExp.setFunctionSource         ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    TupleExp.setFunctionSource     ( "SOURCE_DEFAULT_GET_TYPE","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DQ (7/24/2014): C11 Generic macros requires additional IR support.  This IR node is likely to be
    // similar to the SgJavaTypeExpression node and it might be that that IR node could be eliminated in
    // favor of this newer (more language independent) IR node.
    NEW_TERMINAL_MACRO (TypeExpression, "TypeExpression", "TYPE_EXPRESSION");
    // DQ (7/24/2014): Added more general support for type expressions (required for C11 generic macro support.
    // DQ (7/24/2014): Added more general support for type expressions (required for C11 generic macro support.
    TypeExpression.setFunctionPrototype ( "HEADER_TYPE_EXPRESSION", "../Grammar/Expression.code" );
    // DQ (3/23/2017): We need to change the name to simplify the support for the virtual get_type()
    // function elsewhere in ROSE (and to support the "override" keyword).
    TypeExpression.setDataPrototype ( "SgType*", "type", "= NULL",
                                      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // TypeExpression.setDataPrototype ( "SgType*", "internal_type", "= NULL",
    //        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    TypeExpression.excludeFunctionPrototype        ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );
    TypeExpression.excludeSubTreeFunctionPrototype ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );
    // DQ (7/24/2014): Added more general support for type expressions (required for C11 generic macro support.
    TypeExpression.setFunctionSource         ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    // DQ (7/24/2014): Added more general support for type expressions (required for C11 generic macro support.
    TypeExpression.editSubstitute         ( "PRECEDENCE_VALUE", "16" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (TypeIdOp,               "TypeIdOp",               "TYPEID_OP" );
    TypeIdOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                 "../Grammar/Expression.code" );
    // DQ (1/26/2013): I think that this is an error (see test2013_42.C).
    // TypeIdOp.editSubstitute        ( "PRECEDENCE_VALUE", "16" );
    TypeIdOp.editSubstitute        ( "PRECEDENCE_VALUE", "16" );
    TypeIdOp.setFunctionPrototype ( "HEADER_TYPE_ID_OPERATOR", "../Grammar/Expression.code" );
    TypeIdOp.setDataPrototype ( "SgExpression*", "operand_expr"   , "= NULL",
                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    TypeIdOp.setDataPrototype ( "SgType*"      , "operand_type"   , "= NULL",
                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // DQ (1/14/2006): We should not store the type of unary operators but instead obtain it from the operand directly.
    // TypeIdOp.setDataPrototype ( "SgType*"      , "expression_type", "= NULL",
    //        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // TypeIdOp.setDataPrototype ( "SgType*"      , "expression_type", "= NULL",
    //        CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (6/2/2011): Added support for name qualification.
    // TypeIdOp.setDataPrototype ( "int", "name_qualification_length", "= 0",
    //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    TypeIdOp.setDataPrototype ( "int", "name_qualification_length", "= 0",
                                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (6/2/2011): Added information required for new name qualification support.
    // TypeIdOp.setDataPrototype("bool","type_elaboration_required","= false",
    //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    TypeIdOp.setDataPrototype("bool","type_elaboration_required","= false",
                              NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (6/2/2011): Added information required for new name qualification support.
    // TypeIdOp.setDataPrototype("bool","global_qualification_required","= false",
    //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    TypeIdOp.setDataPrototype("bool","global_qualification_required","= false",
                              NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (4/15/2019): Let's demonstrate this is possible in the language before we add support for it (see Cxx11_test/test2019_380.C).
    // DQ (4/15/2019): This is needed to support pointers to member type specified to the typeid operator.
    TypeIdOp.setDataPrototype ( "int", "name_qualification_for_pointer_to_member_class_length", "= 0",
                                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (4/15/2019): This is needed to support pointers to member type specified to the typeid operator.
    TypeIdOp.setDataPrototype("bool","type_elaboration_for_pointer_to_member_class_required","= false",
                              NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (4/15/2019): This is needed to support pointers to member type specified to the typeid operator.
    TypeIdOp.setDataPrototype("bool","global_qualification_for_pointer_to_member_class_required","= false",
                              NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (TypeTraitBuiltinOperator, "TypeTraitBuiltinOperator", "TYPE_TRAIT_BUILTIN_OPERATOR");
    // DQ (7/12/2013): Added type-trait builtin function support.
    TypeTraitBuiltinOperator.setFunctionPrototype ( "HEADER_TYPE_TRAIT_BUILTIN_OPERATOR", "../Grammar/Expression.code" );
    TypeTraitBuiltinOperator.setDataPrototype ( "SgName", "name", "= \"\"",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    TypeTraitBuiltinOperator.setDataPrototype ("SgNodePtrList", "args", "",
                                               NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (UnaryAddOp,             "UnaryAddOp",             "UNARY_ADD_OP" );
    UnaryAddOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                   "../Grammar/Expression.code" );
    UnaryAddOp.editSubstitute      ( "PRECEDENCE_VALUE", "15" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if USE_FORTRAN_IR_NODES
    // DQ (11/24/2007): Added new IR node for locations in the AST where post-processing is required.
    NEW_TERMINAL_MACRO (UnknownArrayOrFunctionReference, "UnknownArrayOrFunctionReference","TEMP_UnknownArrayOrFunctionReference" );
    // DQ (11/24/2007): Added new IR node for locations in the AST where post-processing is required.
    UnknownArrayOrFunctionReference.editSubstitute ( "PRECEDENCE_VALUE", " 2" );
    // DQ (11/24/2007): Added new IR node to handle unknown array reference vs. function call problem in Fortran.
    // These are translated to either array references or function calls within post-processing steps.

    // This is the name of the array variable being referenced or the name of the function (to be decided in post-processing).
    UnknownArrayOrFunctionReference.setDataPrototype ( "std::string", "name", "= \"\"",
                                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // This is the variable reference that was constucted already (will be ignored if we convert this to a function call in post-processing).
    UnknownArrayOrFunctionReference.setDataPrototype ( "SgExpression*", "named_reference", "= NULL",
                                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

    // This is either a subscript list or a function argument list (to be decided in post-processing).
    UnknownArrayOrFunctionReference.setDataPrototype ( "SgExprListExp*", "expression_list", "= NULL",
                                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (UnsignedCharVal,        "UnsignedCharVal",        "UNSIGNED_CHAR_VAL" );
    UnsignedCharVal.setFunctionSource  ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    UnsignedCharVal.setDataPrototype ( "unsigned char", "value", "= 0",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    UnsignedCharVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    UnsignedCharVal.setFunctionSource        ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
    UnsignedCharVal.editSubstitute        ( "GENERIC_TYPE", "SgTypeUnsignedChar" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (UnsignedIntVal,         "UnsignedIntVal",         "UNSIGNED_INT_VAL" );
    UnsignedIntVal.setFunctionSource   ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    UnsignedIntVal.setDataPrototype ( "unsigned int", "value", "= 0",
                                      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    UnsignedIntVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
                                      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    UnsignedIntVal.setFunctionSource         ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
    UnsignedIntVal.editSubstitute         ( "GENERIC_TYPE", "SgTypeUnsignedInt" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (UnsignedLongLongIntVal, "UnsignedLongLongIntVal", "UNSIGNED_LONG_LONG_INT_VAL" );
    UnsignedLongLongIntVal.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    UnsignedLongLongIntVal.setDataPrototype ( "unsigned long long int", "value", "= 0",
                                              CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    UnsignedLongLongIntVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
                                              CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    UnsignedLongLongIntVal.setFunctionSource ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
    UnsignedLongLongIntVal.editSubstitute ( "GENERIC_TYPE", "SgTypeUnsignedLongLong" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (UnsignedLongVal,        "UnsignedLongVal",        "UNSIGNED_LONG_INT_VAL" );
    UnsignedLongVal.setFunctionSource  ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    UnsignedLongVal.setDataPrototype ( "unsigned long", "value", "= 0",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    UnsignedLongVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    UnsignedLongVal.setFunctionSource        ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
    UnsignedLongVal.editSubstitute        ( "GENERIC_TYPE", "SgTypeUnsignedLong" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (UnsignedShortVal,       "UnsignedShortVal",       "UNSIGNED_SHORT_VAL" );
    UnsignedShortVal.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    UnsignedShortVal.setDataPrototype ( "unsigned short", "value", "= 0",
                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    UnsignedShortVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    UnsignedShortVal.setFunctionSource       ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
    UnsignedShortVal.editSubstitute       ( "GENERIC_TYPE", "SgTypeUnsignedShort" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if USE_UPC_IR_NODES
    NEW_TERMINAL_MACRO (UpcBlocksizeofExpression,    "UpcBlocksizeofExpression",    "UPC_BLOCK_SIZEOF_EXPR" );
    UpcBlocksizeofExpression.setDataPrototype ( "SgExpression*", "expression", "= NULL",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    UpcBlocksizeofExpression.setDataPrototype ( "SgType*", "operand_type", "= NULL",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    UpcBlocksizeofExpression.setDataPrototype ( "SgType*", "expression_type", "= NULL",
                                                CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif
    UpcBlocksizeofExpression.editSubstitute ( "PRECEDENCE_VALUE", "16" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if USE_UPC_IR_NODES
    NEW_TERMINAL_MACRO (UpcElemsizeofExpression,     "UpcElemsizeofExpression",     "UPC_ELEM_SIZEOF_EXPR" );
    UpcElemsizeofExpression.setDataPrototype ( "SgExpression*", "expression", "= NULL",
                                               CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    UpcElemsizeofExpression.setDataPrototype ( "SgType*", "operand_type", "= NULL",
                                               CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    UpcElemsizeofExpression.setDataPrototype ( "SgType*", "expression_type", "= NULL",
                                               CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif
    UpcElemsizeofExpression.editSubstitute  ( "PRECEDENCE_VALUE", "16" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if USE_UPC_IR_NODES
    // DQ and Liao (6/10/2008): Added new IR nodes specific to UPC.
    NEW_TERMINAL_MACRO (UpcLocalsizeofExpression,    "UpcLocalsizeofExpression",    "UPC_LOCAL_SIZEOF_EXPR" );
    // DQ (2/12/2011): Added support for types to UPC specific sizeof operators.
    // DQ and Liao (6/10/2008): Added new IR nodes specific to UPC.
    UpcLocalsizeofExpression.setDataPrototype ( "SgExpression*", "expression", "= NULL",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    UpcLocalsizeofExpression.setDataPrototype ( "SgType*", "operand_type", "= NULL",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    UpcLocalsizeofExpression.setDataPrototype ( "SgType*", "expression_type", "= NULL",
                                                CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif
    // DQ (2/12/2011): Added support for UPC specific sizeof operators.
    UpcLocalsizeofExpression.editSubstitute ( "PRECEDENCE_VALUE", "16" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (UpcMythread,             "UpcMythread",                 "UPC_MYTHREAD" );
    UpcMythread.setFunctionSource         ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    UpcMythread.setDataPrototype ( "int", "value", "= 0",
                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    UpcMythread.setDataPrototype ( "std::string", "valueString", "= \"\"",
                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    UpcMythread.setFunctionSource            ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
    UpcMythread.editSubstitute            ( "GENERIC_TYPE", "SgTypeInt" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Liao 6/18/2008: Support UPC constant THREADS, MYTHREAD
    NEW_TERMINAL_MACRO (UpcThreads,              "UpcThreads",                 "UPC_THREADS" );
    UpcThreads.setFunctionSource          ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    // Liao 6/18/2008, UPC THREADS, MYTHREAD
    UpcThreads.setDataPrototype ( "int", "value", "= 0",
                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    UpcThreads.setDataPrototype ( "std::string", "valueString", "= \"\"",
                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    UpcThreads.setFunctionSource             ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
    UpcThreads.editSubstitute             ( "GENERIC_TYPE", "SgTypeInt" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if USE_FORTRAN_IR_NODES
    // User defined operator for Fortran named operators.
    NEW_TERMINAL_MACRO (UserDefinedBinaryOp,   "UserDefinedBinaryOp",   "USER_DEFINED_BINARY_OP" );
#endif
    UserDefinedBinaryOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
#if USE_FORTRAN_IR_NODES
    UserDefinedBinaryOp.editSubstitute ( "PRECEDENCE_VALUE", " 2" );
    UserDefinedBinaryOp.setFunctionPrototype ( "HEADER_USER_DEFINED_BINARY_EXPRESSION", "../Grammar/Expression.code" );
    UserDefinedBinaryOp.setDataPrototype     ( "SgName", "operator_name", "= \"\"",
                                               CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    UserDefinedBinaryOp.setDataPrototype ( "SgFunctionSymbol*", "symbol"     , "= NULL",
                                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif
    // UserDefinedBinaryOp.setFunctionSource ( "SOURCE_GET_TYPE_FROM_SYMBOL","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // User defined operator for Fortran named operators.
    NEW_TERMINAL_MACRO (UserDefinedUnaryOp,    "UserDefinedUnaryOp",    "USER_DEFINED_UNARY_OP" );
    UserDefinedUnaryOp.setFunctionSource  ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
#if USE_FORTRAN_IR_NODES
    UserDefinedUnaryOp.editSubstitute  ( "PRECEDENCE_VALUE", " 2" );
    UserDefinedUnaryOp.setFunctionPrototype ( "HEADER_USER_DEFINED_UNARY_EXPRESSION", "../Grammar/Expression.code" );
    UserDefinedUnaryOp.setDataPrototype     ( "SgName", "operator_name", "= \"\"",
                                              CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    UserDefinedUnaryOp.setDataPrototype ( "SgFunctionSymbol*", "symbol"     , "= NULL",
                                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif
    // DQ (10/8/2008): Unclear if this is how we should hancle this!
    // UserDefinedUnaryOp.setFunctionSource  ( "SOURCE_GET_TYPE_FROM_SYMBOL","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (VarArgCopyOp,           "VarArgCopyOp",           "VA_COPY_OP" );
    VarArgCopyOp.setFunctionSource  ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    VarArgCopyOp.editSubstitute    ( "PRECEDENCE_VALUE", "16" );
    VarArgCopyOp.setDataPrototype  ( "SgExpression*", "lhs_operand"   , "= NULL",
                                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    VarArgCopyOp.setDataPrototype  ( "SgExpression*", "rhs_operand"   , "= NULL",
                                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    VarArgCopyOp.setDataPrototype ( "SgType*", "expression_type", "= NULL",
                                    CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    VarArgCopyOp.setFunctionSource        ( "SOURCE_DEFAULT_GET_TYPE","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (VarArgEndOp,            "VarArgEndOp",            "VA_END_OP" );
    VarArgEndOp.setFunctionSource   ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    VarArgEndOp.editSubstitute     ( "PRECEDENCE_VALUE", "16" );
    VarArgEndOp.setFunctionPrototype ( "HEADER_VARARG_END_OPERATOR", "../Grammar/Expression.code" );
    VarArgEndOp.setDataPrototype  ( "SgExpression*", "operand_expr"   , "= NULL",
                                    CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    VarArgEndOp.setDataPrototype ( "SgType*", "expression_type", "= NULL",
                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    VarArgEndOp.setFunctionPrototype ( "HEADER_VARARG_END_OPERATOR", "../Grammar/Expression.code" );
    VarArgEndOp.setFunctionSource         ( "SOURCE_DEFAULT_GET_TYPE","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (VarArgOp,               "VarArgOp",               "VA_OP" );
    VarArgOp.setFunctionSource      ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    VarArgOp.editSubstitute        ( "PRECEDENCE_VALUE", "16" );
    VarArgOp.setFunctionPrototype ( "HEADER_VARARG_OPERATOR", "../Grammar/Expression.code" );
    VarArgOp.setDataPrototype  ( "SgExpression*", "operand_expr"   , "= NULL",
                                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    VarArgOp.setDataPrototype ( "SgType*", "expression_type", "= NULL",
                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // DQ (1/17/2008): this was implemented twice (see few lines above)!
    // DQ (1/16/2006): Added support for custom get_type() member function (returns explicitly stored type)
    // VarArgOp.setFunctionPrototype ( "HEADER_VARARG_OPERATOR", "../Grammar/Expression.code" );
    // VarArgOp.setFunctionPrototype ( "HEADER_VARARG_OPERATOR", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (VarArgStartOneOperandOp,"VarArgStartOneOperandOp","VA_START_ONE_OPERAND_OP" );
    VarArgStartOneOperandOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    VarArgStartOneOperandOp.editSubstitute   ( "PRECEDENCE_VALUE", "16" );
    VarArgStartOneOperandOp.setDataPrototype ( "SgExpression*", "operand_expr"   , "= NULL",
                                               CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    VarArgStartOneOperandOp.setDataPrototype ( "SgType*", "expression_type", "= NULL",
                                               CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    VarArgStartOneOperandOp.setFunctionSource ( "SOURCE_DEFAULT_GET_TYPE","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DQ (2/5/2004): EDG 3.3 now separates out vararg functions explicitly in the AST
    // (something I always wanted to see done), so we will do the same in SAGE.
    // This provides for the best possible vararg handling!
    NEW_TERMINAL_MACRO (VarArgStartOp,          "VarArgStartOp",          "VA_START_OP" );
    // DQ (2/5/2004): Adding support for varargs in AST
    VarArgStartOp.setFunctionSource           ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    // DQ (2/5/2004): Adding support for varargs in AST
    VarArgStartOp.editSubstitute   ( "PRECEDENCE_VALUE", "16" );
    // DQ (2/5/2004): Adding vararg support for SAGE AST
    VarArgStartOp.setFunctionPrototype ( "HEADER_VARARG_START_OPERATOR", "../Grammar/Expression.code" );
    VarArgStartOp.setDataPrototype ( "SgExpression*", "lhs_operand"   , "= NULL",
                                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    VarArgStartOp.setDataPrototype ( "SgExpression*", "rhs_operand"   , "= NULL",
                                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    VarArgStartOp.setDataPrototype ( "SgType*", "expression_type", "= NULL",
                                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // DQ (2/5/2004): Adding vararg support to SAGE
    VarArgStartOp.setFunctionSource       ( "SOURCE_DEFAULT_GET_TYPE","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DQ (12/13/2005): Added variant expression to support future patterns
    // specifications (contains RegEx string specifier for SgStatement IR node).
    NEW_TERMINAL_MACRO (VariantExpression,      "VariantExpression",          "VARIANT_EXPR" );
    VariantExpression.setFunctionSource( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    VariantExpression.editSubstitute ( "PRECEDENCE_VALUE", "16" );
    // VariantExpression.setDataPrototype        ( "SgType*", "expression_type", "= NULL",
    //                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // VariantExpression.setFunctionSource ( "SOURCE_DEFAULT_GET_TYPE", "../Grammar/Expression.code" );
    // VariantExpression.setFunctionSource ( "SOURCE_EMPTY_SET_TYPE_FUNCTION", "../Grammar/Expression.code" );
    // VariantExpression.editSubstitute    ( "GENERIC_TYPE", "SgTypeDefault" );
    VariantExpression.setFunctionSource ( "SOURCE_GET_TYPE_GENERIC", "../Grammar/Expression.code" );
    VariantExpression.editSubstitute    ( "GENERIC_TYPE", "SgTypeDefault" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // This function sets up the type system for the grammar.  In this case it implements the
    // C++ grammar, but this will be modified to permit all grammars to contain elements of the
    // C++ grammar.  Modified grammars will add and subtract elements from this default C++ grammar.
    NEW_TERMINAL_MACRO (VarRefExp,              "VarRefExp",              "VAR_REF" );
    // DQ (1/14/2006): We should be using SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION instead of
    // SOURCE_POST_CONSTRUCTION_INITIALIZATION_USING_SET_TYPE since we don't want to have a set_type
    // function on certain types of expressions (because the type should be computed from the operands
    // or the value types directly).
    VarRefExp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                  "../Grammar/Expression.code" );
    VarRefExp.setFunctionPrototype ( "HEADER_VAR_REF_EXPRESSION", "../Grammar/Expression.code" );

    // DQ (11/4/2015): The set_* access function should not mark set the isModified flag.
    // VarRefExp.setDataPrototype ( "SgVariableSymbol*", "symbol", "= NULL",
    //                              CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    VarRefExp.setDataPrototype ( "SgVariableSymbol*", "symbol", "= NULL",
                                 CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (9/22/2011): Double checked and this was marked as NO_TRAVERSAL in master (so mark it as such so
    // that I can pass the existing tests).  I am trying to pass the existing tests and implement the
    // new changes to support originalExpressionTree handling gradually so that I can identify what
    // changes are breaking a few of the existing tests.
    // DQ (9/17/2011): Put back the traversal over the originalExpressionTree (because it will be set to NULL in post processing).
    // DQ (9/16/2011): Modified this to specify NO_TRAVERSAL.
    // DQ (2/6/2011): Added reference to expression tree for unfolded constant expressions (see comment above).
    VarRefExp.setDataPrototype ( "SgExpression*", "originalExpressionTree", "= NULL",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (5/11/2011): Added support for name qualification.
    // VarRefExp.setDataPrototype ( "int", "name_qualification_length", "= 0",
    //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    VarRefExp.setDataPrototype ( "int", "name_qualification_length", "= 0",
                                 NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (5/11/2011): Added information required for new name qualification support.
    // VarRefExp.setDataPrototype("bool","type_elaboration_required","= false",
    //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    VarRefExp.setDataPrototype("bool","type_elaboration_required","= false",
                               NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (6/11/2015): Skip building of access functions (because it sets the isModified flag, not wanted for the name qualification step).
    // DQ (5/11/2011): Added information required for new name qualification support.
    // VarRefExp.setDataPrototype("bool","global_qualification_required","= false",
    //                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    VarRefExp.setDataPrototype("bool","global_qualification_required","= false",
                               NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    VarRefExp.setFunctionSource              ( "SOURCE_GET_TYPE_FROM_SYMBOL","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DQ (2/14/2019): Added support for C++14 void values.
    NEW_TERMINAL_MACRO (VoidVal,          "VoidVal",          "VOID_VAL" );
    VoidVal.setFunctionSource          ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    VoidVal.setFunctionSource                ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
    VoidVal.editSubstitute                ( "GENERIC_TYPE", "SgTypeVoid" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (WcharVal,               "WcharVal",               "WCHAR_VAL" );
    // This stores values such as L'ab' where as a SgCharVal would store 'x'.
    WcharVal.setFunctionPrototype ( "HEADER_WCHAR_VALUE_EXPRESSION", "../Grammar/Expression.code" );
    WcharVal.setDataPrototype ( "unsigned long", "valueUL", "= 0",
                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    WcharVal.setDataPrototype ( "std::string", "valueString", "= \"\"",
                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    WcharVal.setFunctionSource               ( "SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code" );
    WcharVal.editSubstitute               ( "GENERIC_TYPE", "SgTypeWchar" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (XorAssignOp,            "XorAssignOp",            "XOR_ASSIGN_OP" );
    XorAssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                    "../Grammar/Expression.code" );
    XorAssignOp.editSubstitute     ( "PRECEDENCE_VALUE", " 2" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_TERMINAL_MACRO (YieldExpression,           "YieldExpression",               "YIELD_EXP" );
    // NEW_TERMINAL_MACRO (YieldExpression,           "YieldExpression",           "YIELD_EXPR" );
    YieldExpression.setFunctionSource         ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    YieldExpression.setDataPrototype            ( "SgExpression*", "value", "= NULL",
                                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    YieldExpression.setFunctionSource      ( "SOURCE_DEFAULT_GET_TYPE","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Nonterminal classes in reverse hierarchical order
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_NONTERMINAL_MACRO (CompoundAssignOp,
                           PlusAssignOp   | MinusAssignOp    | AndAssignOp  | IorAssignOp    | MultAssignOp     |
                           DivAssignOp    | ModAssignOp      | XorAssignOp  | LshiftAssignOp | RshiftAssignOp   |
                           JavaUnsignedRshiftAssignOp        | IntegerDivideAssignOp | ExponentiationAssignOp,
                           "CompoundAssignOp", "COMPOUND_ASSIGN_OP", false);
    CompoundAssignOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                         "../Grammar/Expression.code" );
    CompoundAssignOp.excludeFunctionPrototype ( "HEADER_PRECEDENCE", "../Grammar/Expression.code" );
    CompoundAssignOp.setFunctionPrototype ( "HEADER_COMPOUND_ASSIGNMENT_OPERATOR", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //SK (08/20/2015) Elementwise operators in Matlab
    NEW_NONTERMINAL_MACRO (ElementwiseOp,
                           ElementwiseMultiplyOp    |  ElementwisePowerOp    | ElementwiseLeftDivideOp |
                           ElementwiseDivideOp      |  ElementwiseAddOp      | ElementwiseSubtractOp ,
                           "ElementwiseOp", "ELEMENT_WISE_OP", false);
    ElementwiseOp.setFunctionSource          ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    ElementwiseOp.excludeFunctionPrototype ( "HEADER_PRECEDENCE", "../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_NONTERMINAL_MACRO (BinaryOp,
                           ArrowExp       | DotExp           | DotStarOp           | ArrowStarOp      | EqualityOp           | LessThanOp     |
                           GreaterThanOp  | NotEqualOp       | LessOrEqualOp       | GreaterOrEqualOp | AddOp                | SubtractOp     |
                           MultiplyOp     | DivideOp         | IntegerDivideOp     | ModOp            | AndOp                | OrOp           |
                           BitXorOp       | BitAndOp         | BitOrOp             | BitEqvOp         | CommaOpExp           | LshiftOp       |
                           RshiftOp       | PntrArrRefExp    | ScopeOp             | AssignOp         | ExponentiationOp     | JavaUnsignedRshiftOp |
                           ConcatenationOp | PointerAssignOp | UserDefinedBinaryOp | CompoundAssignOp | MembershipOp         | SpaceshipOp    |
                           NonMembershipOp | IsOp            | IsNotOp             | DotDotExp        | ElementwiseOp        | PowerOp        |
                           LeftDivideOp    | RemOp           | ReplicationOp       | AtOp,
                           "BinaryOp","BINARY_EXPRESSION", false);
    BinaryOp.setFunctionPrototype            ( "HEADER_EXTRA_FUNCTIONS", "../Grammar/Expression.code" );
    BinaryOp.excludeFunctionPrototype        ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );
    BinaryOp.excludeSubTreeFunctionPrototype ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );
    BinaryOp.setSubTreeFunctionPrototype    ( "HEADER_PRECEDENCE", "../Grammar/Expression.code" );
    BinaryOp.excludeFunctionPrototype       ( "HEADER_PRECEDENCE", "../Grammar/Expression.code" );
    BinaryOp.setFunctionPrototype ( "HEADER_GET_NEXT_EXPRESSION", "../Grammar/Expression.code" );
    BinaryOp.setFunctionPrototype ( "HEADER_BINARY_EXPRESSION", "../Grammar/Expression.code" );
    BinaryOp.setDataPrototype ( "SgExpression*", "lhs_operand_i"  , "= NULL",
                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    BinaryOp.setDataPrototype ( "SgExpression*", "rhs_operand_i"  , "= NULL",
                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    // DQ (1/14/2006): We should not store the type of unary operators but instead obtain it from the operand directly.
    // BinaryOp.setDataPrototype ( "SgType*"      , "expression_type", "= NULL",
    //        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    BinaryOp.setDataPrototype ( "SgType*"      , "expression_type", "= NULL",
                                CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // DQ (9/22/2011): Double checked and this was marked as NO_TRAVERSAL in master (so mark it as such so
    // that I can pass the existing tests).  I am trying to pass the existing tests and implement the
    // new changes to support originalExpressionTree handling gradually so that I can identify what
    // changes are breaking a few of the existing tests.
    // DQ (9/17/2011): Put back the traversal over the originalExpressionTree (because it will be set to NULL in post processing).
    // DQ (9/16/2011): Modified this to specify NO_TRAVERSAL.
    // DQ (2/6/2011): Added reference to expression tree for unfolded constant expressions (see comment above).
    BinaryOp.setDataPrototype ( "SgExpression*", "originalExpressionTree", "= NULL",
                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // TV (06/06/13) : CudaKernelCall are now considered to be a FunctionCall
    NEW_NONTERMINAL_MACRO (FunctionCallExp,
                           CudaKernelCallExp,
                           "FunctionCallExp","FUNC_CALL", true);
    FunctionCallExp.setFunctionPrototype ( "HEADER_FUNCTION_CALL_EXPRESSION", "../Grammar/Expression.code" );
    // FunctionCallExp.editSubstitute       ( "LIST_FUNCTION_RETURN_TYPE", "void" );
    // DQ (4/8/2013): Added support for specification of operator vs. non-operator syntax ("x+y" instead of "operator+(x,y)").
    // This is relevant for generated code in some cases (has different function evaluation rules).  See test2013_100.C.
    // ROSE has historically defaulted to using the operator syntax ("x+y") in generated code, but sometimes this is an error
    // as test2013_100.C demonstrates.
    FunctionCallExp.setDataPrototype ( "bool", "uses_operator_syntax", "= false",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    // PP (7/17/23): To distinguish equivalent Ada calls in the form of
    //               x.init(1,2,3) and init(x, 1, 2, 4)
    //               the flag indicates if the call was parsed/ should be unparsed using the
    //               first argument as prefix notation.
    FunctionCallExp.setDataPrototype ( "bool", "usesObjectCallSyntax", "= false",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DQ (7/12/2013): Added new IR nodes to support new type of function call (builtin functions used for type trait support
    // in later versions of GNU and other compilers).  For more details see: http://gcc.gnu.org/onlinedocs/gcc/Type-Traits.html
    // These are required to be supported as part of bug fix for proper handling of Boost (but also some STL that was never a
    // noticed problem).  These builtin functions take types as parameters and sometimes return types as well.  They will
    // require an implementation in ROSE to support analysis.
    NEW_NONTERMINAL_MACRO (CallExpression,FunctionCallExp,"CallExpression","CALL_EXPRESSION", true);
    // NEW_NONTERMINAL_MACRO (CallExpression,FunctionCallExp | TypeTraitBuiltinOperator,"CallExpression","CALL_EXPRESSION", true);
    CallExpression.setFunctionPrototype ( "HEADER_GET_NEXT_EXPRESSION", "../Grammar/Expression.code" );
    CallExpression.setFunctionPrototype ( "HEADER_CALL_EXPRESSION", "../Grammar/Expression.code" );
    CallExpression.editSubstitute       ( "HEADER_LIST_DECLARATIONS", "HEADER_LIST_FUNCTIONS", "../Grammar/Expression.code" );
    CallExpression.editSubstitute       ( "LIST_NAME", "arg" );
    CallExpression.setDataPrototype ( "SgExpression*", "function", "= NULL",
                                      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    CallExpression.setDataPrototype ( "SgExprListExp*", "args", "= NULL",
                                      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

    // DQ (1/14/2006): We should not store the type of unary operators but instead obtain it from the operand directly.
    // CallExpression.setDataPrototype ( "SgType*", "expression_type", "= NULL",
    //        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    CallExpression.setDataPrototype ( "SgType*", "expression_type", "= NULL",
                                      CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NEW_NONTERMINAL_MACRO (ExprListExp,
                           ListExp  | TupleExp | MatrixExp,
                           "ExprListExp","EXPR_LIST", /* can have instances = */ true);
    ExprListExp.setFunctionSource      ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    ExprListExp.setFunctionPrototype ( "HEADER_EXPRESSION_LIST_EXPRESSION", "../Grammar/Expression.code" );

    // MK: I moved the following data member declaration from ../Grammar/Expression.code to this position:
    ExprListExp.setDataPrototype("SgExpressionPtrList", "expressions", "",
                                 NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    ExprListExp.editSubstitute       ( "HEADER_LIST_DECLARATIONS", "HEADER_LIST_FUNCTIONS", "../Grammar/Expression.code" );
    // ExprListExp.editSubstitute       ( "LIST_DATA_TYPE", "Expression" );
    ExprListExp.editSubstitute       ( "LIST_NAME", "expression" );
    ExprListExp.setFunctionSource            ( "SOURCE_DEFAULT_GET_TYPE","../Grammar/Expression.code" );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         NEW_NONTERMINAL_MACRO (Initializer,
                            AggregateInitializer | CompoundInitializer | ConstructorInitializer |
                            AssignInitializer | DesignatedInitializer | BracedInitializer |
                            AdaAncestorInitializer,
                            "Initializer","EXPR_INIT", false);
     Initializer.excludeFunctionPrototype     ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );
     Initializer.setFunctionSource      ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     Initializer.setSubTreeFunctionPrototype ( "HEADER_GET_NEXT_EXPRESSION", "../Grammar/Expression.code" );
     Initializer.excludeFunctionPrototype ( "HEADER_GET_NEXT_EXPRESSION", "../Grammar/Expression.code" );
     Initializer.setFunctionPrototype ( "HEADER_INITIALIZER_EXPRESSION", "../Grammar/Expression.code" );
     Initializer.setDataPrototype     ( "bool", "is_explicit_cast", "= true",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     // DQ (11/6/2014): This is C++11 syntax for direct brace initalization (e.g. int n{}).
     Initializer.setDataPrototype     ( "bool", "is_braced_initialized", "= false",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
     // DQ (1/13/2014): Added Java support for Java annotations (hierarchy).
     NEW_NONTERMINAL_MACRO (JavaAnnotation,
                            JavaMarkerAnnotation | JavaSingleMemberAnnotation | JavaNormalAnnotation,
                            "JavaAnnotation","JAVA_ANNOTATION", false);
     // DQ (1/13/2014): Define the get_type meber function in the JavaAnnotation (default), but exclude it from the subtree.
     // JavaAnnotation.excludeFunctionPrototype        ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );
     JavaAnnotation.excludeSubTreeFunctionPrototype ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );
     // DQ (1/13/2014): Added Java support for Java annotations.
     JavaAnnotation.setFunctionSource             ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     // DQ (1/13/2014): Added Java support for Java annotations.
     JavaAnnotation.editSubstitute             ( "PRECEDENCE_VALUE", "16" );
     // DQ (1/13/2014): Added Java support for Java annotations.
     JavaAnnotation.setFunctionPrototype ( "HEADER_JAVA_ANNOTATION", "../Grammar/Expression.code" );
     // DQ (3/7/2014): Added support to build access functions for type to be reset in snippet support.
     JavaAnnotation.setDataPrototype ( "SgType*", "expression_type", "= NULL",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
     NEW_NONTERMINAL_MACRO (NaryOp,
                            NaryBooleanOp  | NaryComparisonOp,
                            "NaryOp","NARY_EXPRESSION", false);
     NaryOp.setFunctionPrototype              ( "HEADER_EXTRA_FUNCTIONS", "../Grammar/Expression.code" );
     NaryOp.excludeFunctionPrototype          ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );
     NaryOp.excludeSubTreeFunctionPrototype   ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );
     NaryOp.setFunctionSource ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION",
                                "../Grammar/Expression.code" );
     NaryOp.setSubTreeFunctionPrototype      ( "HEADER_PRECEDENCE", "../Grammar/Expression.code" );
     NaryOp.excludeFunctionPrototype         ( "HEADER_PRECEDENCE", "../Grammar/Expression.code" );
     NaryOp.setFunctionPrototype ( "HEADER_GET_NEXT_EXPRESSION", "../Grammar/Expression.code" );
     NaryOp.setFunctionPrototype ( "HEADER_NARY_OP", "../Grammar/Expression.code" );
     NaryOp.setDataPrototype     ( "SgExpressionPtrList", "operands", "",
                                   NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     NaryOp.editSubstitute       ( "HEADER_LIST_DECLARATIONS", "HEADER_LIST_FUNCTIONS", "../Grammar/Expression.code" );
     NaryOp.editSubstitute       ( "LIST_NAME", "operand" );
     NaryOp.setDataPrototype     ( "VariantTList", "operators", "",
                                   NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
     NEW_NONTERMINAL_MACRO (UnaryOp,
                            ExpressionRoot | MinusOp            | UnaryAddOp | NotOp           | PointerDerefExp |
                            AddressOfOp    | MinusMinusOp       | PlusPlusOp | BitComplementOp | CastExp         |
                            ThrowOp        | RealPartOp         | ImagPartOp | ConjugateOp     | UserDefinedUnaryOp |
                            MatrixTransposeOp | AbsOp,
                            "UnaryOp","UNARY_EXPRESSION", false);
     UnaryOp.setFunctionPrototype             ( "HEADER_EXTRA_FUNCTIONS", "../Grammar/Expression.code" );
     // get_type is included directly within UnaryOp and BinaryOp
     UnaryOp.excludeFunctionPrototype         ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );
     UnaryOp.excludeSubTreeFunctionPrototype  ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );
     // It is almost true that the presendence operators exist only on classes derived
     // from UnaryOp and BinaryOp plus the SizeOfOp and TypeIdOp, but in fact
     // the ExpressionRoot is derived from UnaryOp and does not have a precedence
     // member function.
     UnaryOp.setSubTreeFunctionPrototype     ( "HEADER_PRECEDENCE", "../Grammar/Expression.code" );
     UnaryOp.excludeFunctionPrototype        ( "HEADER_PRECEDENCE", "../Grammar/Expression.code" );
     UnaryOp.setFunctionPrototype ( "HEADER_GET_NEXT_EXPRESSION", "../Grammar/Expression.code" );
     UnaryOp.setFunctionPrototype ( "HEADER_UNARY_EXPRESSION", "../Grammar/Expression.code" );
     UnaryOp.setDataPrototype ( "SgExpression*", "operand_i", "= NULL",
                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     // DQ (1/14/2006): We should not store the type of unary operators but instead obtain it from the operand directly.
     // However, we can't do that because in a few cases the type is changed as a result of the operator (e.g. SgAddressOp, SgPointerDerefExp).
     // The solution is to have specially built versions of the get_type() function for those operators.
     // An incremental solution is to first eliminate the access functions.
     // UnaryOp.setDataPrototype ( "SgType*", "expression_type", "= NULL",
     //        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UnaryOp.setDataPrototype ( "SgType*", "expression_type", "= NULL",
                                CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     UnaryOp.setDataPrototype ( "SgUnaryOp::Sgop_mode", "mode", "= prefix",
                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // MK: UnaryOp.excludeDataPrototype ( "SgUnaryOp::Sgop_mode", "mode", "= prefix");

     ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
     // DQ (11/21/2017): This was removed in favor of using the SgLabelRefExp.
     // DQ (11/21/2017): Added support for label address value (see test2017_73.C).
     NEW_NONTERMINAL_MACRO (ValueExp,
                            BoolValExp           | StringVal        | ShortVal               | CharVal         | UnsignedCharVal |
                            WcharVal             | UnsignedShortVal | IntVal                 | EnumVal         | UnsignedIntVal  |
                            LongIntVal           | LongLongIntVal   | UnsignedLongLongIntVal | UnsignedLongVal | FloatVal        |
                            DoubleVal            | LongDoubleVal    | ComplexVal             | UpcThreads      | UpcMythread     |
                            TemplateParameterVal | NullptrValExp    | Char16Val              | Char32Val       | Float80Val      |
                            Float128Val          | VoidVal          | AdaFloatVal            |
                            *lookupTerminal(terminalList, "JovialBitVal"),
                            "ValueExp","ValueExpTag", false);

     ValueExp.excludeFunctionPrototype        ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );
     ValueExp.setFunctionSource         ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     ValueExp.setFunctionPrototype ( "HEADER_VALUE_EXPRESSION", "../Grammar/Expression.code" );
     // DQ (9/23/2011): Modified this to not be traversed.  The traversal leads to an inconsistant AST (incrementally applying fixes).
     // DQ (9/22/2011): Double checked and this was marked as DEF_TRAVERSAL in master (so we don't have to change this case).
     // DQ (9/17/2011): Put back the traversal over the originalExpressionTree (because it will be set to NULL in post processing).
     // DQ (9/16/2011): Modified this to not be traversed.  The traversal leads to an inconsistant AST
     // We now want to select two modes of consistant AST (using or not use the originalExpressionTree).
     // DQ (2/7/2011): Moved this to the SgExpression level in the IR node hierarchy because it requires
     // more general support.
     // DQ (6/19/2006): Changed name of data member to be consitant with more general use in SgCastExp
     // DQ (11/9/2005): Added reference to expression tree for original unfolded constant expressions.
     // Constant folding in EDG and ROSE allows us to ignore this subtree, but it is here to to permit
     // the original source code to be faithfully represented.
     ValueExp.setDataPrototype ( "SgExpression*", "originalExpressionTree", "= NULL",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
     // DQ (9/4/2013): Added compound literal support.
     // DQ (7/12/2013): Moved the TypeTraitBuiltinOperator to be derived from Expression.
     NEW_NONTERMINAL_MACRO (Expression,
                            UnaryOp                  | BinaryOp                 | ExprListExp             | VarRefExp           | ClassNameRefExp          |
                            FunctionRefExp           | MemberFunctionRefExp     | ValueExp                | CallExpression      | SizeOfOp                 |
                            UpcLocalsizeofExpression | UpcBlocksizeofExpression | UpcElemsizeofExpression | JavaInstanceOfOp    | SuperExp                 |
                            TypeIdOp                 | ConditionalExp           | NewExp                  | DeleteExp           | ThisExp                  |
                            RefExp                   | Initializer              | VarArgStartOp           | VarArgOp            | VarArgEndOp              |
                            VarArgCopyOp             | VarArgStartOneOperandOp  | NullExpression          | VariantExpression   | SubscriptExpression      |
                            ColonShapeExp            | AsteriskShapeExp         | /*UseOnlyExpression     |*/ ImpliedDo         | IOItemExpression         |
                            /* UseRenameExpression      | */ StatementExpression   | AsmOp                   | LabelRefExp         | ActualArgumentExpression |
                            UnknownArrayOrFunctionReference               | PseudoDestructorRefExp | CAFCoExpression  |
                            CudaKernelExecConfig    |  /* TV (04/22/2010): CUDA support */
                            LambdaRefExp        | DictionaryExp           | KeyDatumPair             |
                            Comprehension       | ListComprehension       | SetComprehension         | DictionaryComprehension      | NaryOp |
                            StringConversion    | YieldExpression         | TemplateFunctionRefExp   | TemplateMemberFunctionRefExp | AlignOfOp |
                            RangeExp            | MagicColonExp           | //SK(08/20/2015): RangeExp and MagicColonExp for Matlab
                            TypeTraitBuiltinOperator | CompoundLiteralExp | JavaAnnotation           | JavaTypeExpression           | TypeExpression |
                            ClassExp            | FunctionParameterRefExp | LambdaExp | HereExp | AtExp | FinishExp | NoexceptOp | NonrealRefExp |
                            AdaTaskRefExp       | AdaProtectedRefExp      | FoldExpression | AwaitExpression | ChooseExpression | AdaAttributeExp |
                            JovialTablePresetExp| JovialPresetPositionExp | AdaOthersExp | AdaRenamingRefExp |
                            AdaUnitRefExp, "Expression", "ExpressionTag", false);

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

     // DQ (8/21/2004): Allow expressions to be marked as lvalues
     Expression.setDataPrototype ( "bool", "lvalue", "= false",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE );

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

     Expression.setSubTreeFunctionPrototype   ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );

     // Within Expression get_type() is specific directly (as a virtual function)
     Expression.excludeFunctionPrototype      ( "HEADER_GET_TYPE", "../Grammar/Expression.code" );

     // This is the easiest solution, then where any post_construction_initialization() function
     // was ment to call the base class post_construction_initialization() function, we just do
     // so directly in thederived class post_construction_initialization() function.
     // MK: the following two function calls could be wrapped into a single one:
     Expression.setFunctionPrototype( "HEADER_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     Expression.setSubTreeFunctionPrototype ( "HEADER_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
     // Expression.excludeFunctionPrototype ( "HEADER_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );


     ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
     // What is this?
#ifdef HL_GRAMMARS
     X_Expression.setAutomaticGenerationOfCopyFunction(false);
#endif

}
