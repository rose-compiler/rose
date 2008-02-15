#!/bin/perl

# perl program to generate the class declarations for the implementation of the
# A++/P++ grammar

# This list is not used but represents the overall structure of the 
# design of the class hierarchy

# These are removed from the list of classes we will define because
# they will be represented by a typedef from an STL list.
# typedef ROSE_IndexArgumentList list<ROSE_IndexExpression>;
# typedef ROSE_ArgumentList      list<ROSE_ArrayExpression>;

# Potitial issues for the future:
# 1) The variable decalarations within our grammar don't have separate 
#    terminals for the "type var", "type & var", "type* var"
# 2) Review use of name "Indexing" in code review.
# 3) ROSE IndexOperand should be derived from ROSE_IndexExpression
# 4) The List of Lists is missing the DereferencedPointers additions to the grammar
# 5) Make ROSE_IndexingOperand a part of ROSE_IndexingExpression
# 6) Make Rose_IndexingExpression part of ROSE_Expression
# 7) Expression Suffix not uniform in naming
# 8) Add ROSE_IndexingStatement to grammar
# 9) Consider ROSE_C_Type

@ClassList = (
ROSE_Node,
     ROSE_IndexArgumentList,
     ROSE_IndexExpression,
          ROSE_IndexExpressionBinaryOperatorCExpression,
          ROSE_CExpressionBinaryOperatorIndexExpression,
          ROSE_UnaryOperatorIndexOperand,
     ROSE_IndexOperand,
          ROSE_IndexingVariable,
               ROSE_IndexVariable,
               ROSE_RangeVariable,
          ROSE_IndexingVariableReference,
               ROSE_IndexVariableReference,
               ROSE_RangeVariableReference,
          ROSE_IndexingVariablePointer,
               ROSE_IndexVariablePointer,
               ROSE_RangeVariablePointer,
          ROSE_IndexingVariableDereferencedPointer,
               ROSE_IndexVariableDereferencedPointer,
               ROSE_RangeVariableDereferencedPointer,
     ROSE_Type,
          ROSE_ArrayType,
               ROSE_RealArrayType,
                    ROSE_doubleArrayType,
                    ROSE_floatArrayType,
               ROSE_intArrayType,
          ROSE_IndexingType,
               ROSE_IndexType,
               ROSE_RangeType,
     ROSE_ArgumentList,
     ROSE_Statement,
          ROSE_VariableDeclaration,
               ROSE_C_VariableDeclaration,
               ROSE_ArrayVariableDeclaration,
               ROSE_IndexingVariableDeclaration,
          ROSE_StatementBlock,
          ROSE_C_Statement,
          ROSE_ArrayStatement,
               ROSE_ExpressionStatement,
               ROSE_ReturnStatement,
               ROSE_WhereStatement,
               ROSE_ElseWhereStatement,
               ROSE_DoWhileStatement,
               ROSE_WhileStatement,
               ROSE_ForStatement,
               ROSE_IfStatement,
     ROSE_Expression,
          ROSE_C_Expression,
          ROSE_UserFunction,
          ROSE_ArrayExpression,
               ROSE_ArrayOperator,
                    ROSE_UnaryArrayOperator,
                         ROSE_UnaryArrayOperatorMinus,
                         ROSE_UnaryArrayOperatorPlus,
                         ROSE_UnaryArrayOperatorPrefixPlusPlus,
                         ROSE_UnaryArrayOperatorPostfixPlusPlus,
                         ROSE_UnaryArrayOperatorPrefixMinusMinus,
                         ROSE_UnaryArrayOperatorPostfixMinusMinus,
                         ROSE_UnaryArrayLogicalOperator,
                              ROSE_UnaryArrayOperatorNOT,
                    ROSE_BinaryArrayOperator,
                         ROSE_BinaryArrayOperatorEquals,
                         ROSE_BinaryArrayOperatorAdd,
                         ROSE_BinaryArrayOperatorAddEquals,
                         ROSE_BinaryArrayOperatorMinus,
                         ROSE_BinaryArrayOperatorMinusEquals,
                         ROSE_BinaryArrayOperatorMultiply,
                         ROSE_BinaryArrayOperatorMultiplyEquals,
                         ROSE_BinaryArrayOperatorDivide,
                         ROSE_BinaryArrayOperatorDivideEquals,
                         ROSE_BinaryArrayOperatorModulo,
                         ROSE_BinaryArrayOperatorModuloEquals,
                         ROSE_BinaryArrayTestingOperator,
                              ROSE_BinaryArrayOperatorLT,
                              ROSE_BinaryArrayOperatorLTEquals,
                              ROSE_BinaryArrayOperatorGT,
                              ROSE_BinaryArrayOperatorGTEquals,
                         ROSE_BinaryArrayLogicalOperator,
                              ROSE_BinaryArrayOperatorEquivalence,
                              ROSE_BinaryArrayOperatorNOTEquals,
                              ROSE_BinaryArrayOperatorLogicalAND,
                              ROSE_BinaryArrayOperatorLogicalOR,
               ROSE_NumericExpression,
                    ROSE_ArrayOperandBinaryArrayOperatorArrayOperand,
                    ROSE_ArrayOperandBinaryArrayOperatorArrayExpression,
                    ROSE_UnaryArrayOperatorArrayOperand,
                    ROSE_UnaryArrayOperatorArrayExpression,
               ROSE_TestingExpression,
                    ROSE_ArrayOperandBinaryArrayTestingOperatorArrayOperand,
                    ROSE_ArrayOperandBinaryArrayTestingOperatorArrayExpression,
               ROSE_LogicalExpression,
                    ROSE_ArrayOperandBinaryArrayLogicalOperatorArrayOperand,
                    ROSE_ArrayOperandBinaryArrayLogicalOperatorArrayExpression,
               ROSE_ArrayOperandExpression,
                    ROSE_ArrayVariableExpression,
                         ROSE_RealArrayVariableExpression,
                              ROSE_doubleArrayVariableExpression,
                              ROSE_floatArrayVariableExpression,
                         ROSE_intArrayVariableExpression,
                    ROSE_ArrayVariableReferenceExpression,
                         ROSE_RealArrayVariableReferenceExpression,
                              ROSE_doubleArrayVariableReferenceExpression,
                              ROSE_floatArrayVariableReferenceExpression,
                         ROSE_intArrayVariableReferenceExpression,
                    ROSE_ArrayVariablePointerExpression,
                         ROSE_RealArrayVariablePointerExpression,
                              ROSE_doubleArrayVariablePointerExpression,
                              ROSE_floatArrayVariablePointerExpression,
                         ROSE_intArrayVariablePointerExpression,
                    ROSE_ArrayVariableDereferencedPointerExpression,
                         ROSE_RealArrayVariableDereferencedPointerExpression,
                              ROSE_doubleArrayVariableDereferencedPointerExpression,
                              ROSE_floatArrayVariableDereferencedPointerExpression,
                         ROSE_intArrayVariableDereferencedPointerExpression,
                    ROSE_ArrayParenthesisOperator,
               ROSE_ArrayFunctionExpression,
                    ROSE_ArrayCOS,
                    ROSE_ArraySIN,
                    ROSE_ArrayTAN,
                    ROSE_ArrayMIN,
                    ROSE_ArrayMAX,
                    ROSE_ArrayFMOD,
                    ROSE_ArrayMOD,
                    ROSE_ArrayPOW,
                    ROSE_ArraySIGN,
                    ROSE_ArrayLOG,
                    ROSE_ArrayLOG10,
                    ROSE_ArrayEXP,
                    ROSE_ArraySQRT,
                    ROSE_ArrayFABS,
                    ROSE_ArrayCEIL,
                    ROSE_ArrayFLOOR,
                    ROSE_ArrayABS,
                    ROSE_ArrayTRANSPOSE,
                    ROSE_ArrayACOS,
                    ROSE_ArrayASIN,
                    ROSE_ArrayATAN,
                    ROSE_ArrayCOSH,
                    ROSE_ArraySINH,
                    ROSE_ArrayTANH,
                    ROSE_ArrayACOSH,
                    ROSE_ArrayASINH,
                    ROSE_ArrayATANH
             );

# The indentation reflects the hierarchy (the less indented name is a base class)
@ClassListRoot = ( 
ROSE_Node );

# We can use a list of lists but I'm not clear if we require that!
@ClassList_DerivedFrom_Node = ( ClassList_Node );

@ClassList_Node = (
     ROSE_IndexArgumentList,
     ROSE_IndexExpression,
     ROSE_IndexOperand,
     ROSE_Type,
     ROSE_ArgumentList,
     ROSE_Statement,
     ROSE_Expression );

@ClassList_Statement = (
     ROSE_VariableDeclaration,
     ROSE_StatementBlock,
     ROSE_C_Statement,
     ROSE_ArrayStatement );

@ClassList_VariableDeclaration = (
     ROSE_C_VariableDeclaration,
     ROSE_ArrayVariableDeclaration,
     ROSE_IndexingVariableDeclaration );

@ClassList_Expression = (
     ROSE_UserFunction,
     ROSE_C_Expression,
     ROSE_ArrayExpression );

@ClassList_IndexExpression = (
          ROSE_IndexExpressionBinaryOperatorCExpression,
          ROSE_CExpressionBinaryOperatorIndexExpression,
          ROSE_UnaryOperatorIndexOperand );

@ClassList_IndexOperand = (
          ROSE_IndexingVariable,
          ROSE_IndexingVariableReference,
          ROSE_IndexingVariablePointer,
          ROSE_IndexingVariableDereferencedPointer );

@ClassList_IndexingVariable = (
               ROSE_IndexVariable,
               ROSE_RangeVariable );

@ClassList_IndexingVariableReference = (
               ROSE_IndexVariableReference,
               ROSE_RangeVariableReference );

@ClassList_IndexingVariablePointer = (
               ROSE_IndexVariablePointer,
               ROSE_RangeVariablePointer );

@ClassList_IndexingVariableDereferencedPointer = (
               ROSE_IndexVariableDereferencedPointer,
               ROSE_RangeVariableDereferencedPointer );

@ClassList_Type = (
          ROSE_ArrayType,
          ROSE_IndexingType );

@ClassList_IndexingType = (
               ROSE_IndexType,
               ROSE_RangeType );

@ClassList_ArrayType = (
               ROSE_RealArrayType,
               ROSE_intArrayType );

@ClassList_RealArrayType = (
                    ROSE_doubleArrayType,
                    ROSE_floatArrayType );

@ClassList_ArrayOperator = (
          ROSE_UnaryArrayOperator,
          ROSE_BinaryArrayOperator );

@ClassList_UnaryArrayOperator = (
               ROSE_UnaryArrayOperatorMinus,
               ROSE_UnaryArrayOperatorPlus,
               ROSE_UnaryArrayOperatorPrefixPlusPlus,
               ROSE_UnaryArrayOperatorPrefixMinusMinus,
               ROSE_UnaryArrayOperatorPostfixPlusPlus,
               ROSE_UnaryArrayOperatorPostfixMinusMinus,
               ROSE_UnaryArrayLogicalOperator );

@ClassList_UnaryArrayLogicalOperator = (
                    ROSE_UnaryArrayOperatorNOT );

@ClassList_BinaryArrayOperator = (
               ROSE_BinaryArrayOperatorEquals,
               ROSE_BinaryArrayOperatorAdd,
               ROSE_BinaryArrayOperatorAddEquals,
               ROSE_BinaryArrayOperatorMinus,
               ROSE_BinaryArrayOperatorMinusEquals,
               ROSE_BinaryArrayOperatorMultiply,
               ROSE_BinaryArrayOperatorMultiplyEquals,
               ROSE_BinaryArrayOperatorDivide,
               ROSE_BinaryArrayOperatorDivideEquals,
               ROSE_BinaryArrayOperatorModulo,
               ROSE_BinaryArrayOperatorModuloEquals,
               ROSE_BinaryArrayTestingOperator,
               ROSE_BinaryArrayLogicalOperator );

@ClassList_BinaryArrayTestingOperator = (
               ROSE_BinaryArrayOperatorLT,
               ROSE_BinaryArrayOperatorLTEquals,
               ROSE_BinaryArrayOperatorGT,
               ROSE_BinaryArrayOperatorGTEquals );

@ClassList_BinaryArrayLogicalOperator = (
               ROSE_BinaryArrayOperatorEquivalence,
               ROSE_BinaryArrayOperatorNOTEquals,
               ROSE_BinaryArrayOperatorLogicalAND,
               ROSE_BinaryArrayOperatorLogicalOR );

@ClassList_ArrayStatement = (
          ROSE_ExpressionStatement,
          ROSE_WhereStatement,
          ROSE_ElseWhereStatement,
          ROSE_DoWhileStatement,
          ROSE_WhileStatement,
          ROSE_ForStatement,
          ROSE_IfStatement,
          ROSE_ReturnStatement );

@ClassList_ArrayExpression = (
          ROSE_NumericExpression,
          ROSE_TestingExpression,
          ROSE_LogicalExpression,
          ROSE_ArrayOperandExpression,
          ROSE_ArrayOperator,
          ROSE_ArrayFunctionExpression );

@ClassList_NumericExpression = (
               ROSE_ArrayOperandBinaryArrayOperatorArrayOperand,
               ROSE_ArrayOperandBinaryArrayOperatorArrayExpression,
               ROSE_UnaryArrayOperatorArrayOperand,
               ROSE_UnaryArrayOperatorArrayExpression );

@ClassList_TestingExpression = (
               ROSE_ArrayOperandBinaryArrayTestingOperatorArrayOperand,
               ROSE_ArrayOperandBinaryArrayTestingOperatorArrayExpression );

@ClassList_LogicalExpression = (
               ROSE_ArrayOperandBinaryArrayLogicalOperatorArrayOperand,
               ROSE_ArrayOperandBinaryArrayLogicalOperatorArrayExpression );

@ClassList_ArrayOperandExpression = (
               ROSE_ArrayVariableExpression,
               ROSE_ArrayVariableReferenceExpression,
               ROSE_ArrayVariablePointerExpression,
               ROSE_ArrayVariableDereferencedPointerExpression,
               ROSE_ArrayParenthesisOperator);

@ClassList_ArrayVariableExpression = (
                    ROSE_RealArrayVariableExpression,
                    ROSE_intArrayVariableExpression );

@ClassList_RealArrayVariableExpression = (
                         ROSE_doubleArrayVariableExpression,
                         ROSE_floatArrayVariableExpression );

@ClassList_ArrayVariableReferenceExpression = (
                    ROSE_RealArrayVariableReferenceExpression,
                    ROSE_intArrayVariableReferenceExpression );

@ClassList_RealArrayVariableReferenceExpression = (
                         ROSE_doubleArrayVariableReferenceExpression,
                         ROSE_floatArrayVariableReferenceExpression );

@ClassList_ArrayVariablePointerExpression = (
                    ROSE_RealArrayVariablePointerExpression,
                    ROSE_intArrayVariablePointerExpression );

@ClassList_RealArrayVariablePointerExpression = (
                         ROSE_doubleArrayVariablePointerExpression,
                         ROSE_floatArrayVariablePointerExpression );

@ClassList_ArrayVariableDereferencedPointerExpression = (
                    ROSE_RealArrayVariableDereferencedPointerExpression,
                    ROSE_intArrayVariableDereferencedPointerExpression );

@ClassList_RealArrayVariableDereferencedPointerExpression = (
                         ROSE_doubleArrayVariableDereferencedPointerExpression,
                         ROSE_floatArrayVariableDereferencedPointerExpression );

@ClassList_ArrayFunctionExpression = (
               ROSE_ArrayCOS,
               ROSE_ArraySIN,
               ROSE_ArrayTAN,
               ROSE_ArrayMIN,
               ROSE_ArrayMAX,
               ROSE_ArrayFMOD,
               ROSE_ArrayMOD,
               ROSE_ArrayPOW,
               ROSE_ArraySIGN,
               ROSE_ArrayLOG,
               ROSE_ArrayLOG10,
               ROSE_ArrayEXP,
               ROSE_ArraySQRT,
               ROSE_ArrayFABS,
               ROSE_ArrayCEIL,
               ROSE_ArrayFLOOR,
               ROSE_ArrayABS,
               ROSE_ArrayTRANSPOSE,
               ROSE_ArrayACOS,
               ROSE_ArrayASIN,
               ROSE_ArrayATAN,
               ROSE_ArrayCOSH,
               ROSE_ArraySINH,
               ROSE_ArrayTANH,
               ROSE_ArrayACOSH,
               ROSE_ArrayASINH,
               ROSE_ArrayATANH
             );

@ListOfLists = (
     ClassListRoot,
     ClassList_Node,
     ClassList_IndexExpression,
     ClassList_IndexOperand,
     ClassList_IndexingVariable,
     ClassList_IndexingVariableReference,
     ClassList_IndexingVariablePointer,
     ClassList_Type,
     ClassList_IndexingType,
     ClassList_ArrayType,
     ClassList_RealArrayType,
     ClassList_Expression,
     ClassList_ArrayExpression,
     ClassList_ArrayOperator,
     ClassList_UnaryArrayOperator,
     ClassList_UnaryLogicalArrayOperator,
     ClassList_BinaryArrayOperator,
     ClassList_BinaryTestingArrayOperator,
     ClassList_BinaryLogicalArrayOperator,
     ClassList_Statement,
     ClassList_VariableDeclaration,
     ClassList_ArrayStatement,
     ClassList_NumericExpression,
     ClassList_TestingExpression,
     ClassList_LogicalExpression,
     ClassList_ArrayOperandExpression,
     ClassList_ArrayVariableExpression,
     ClassList_RealArrayVariableExpression,
     ClassList_ArrayVariableReferenceExpression,
     ClassList_RealArrayVariableReferenceExpression,
     ClassList_ArrayVariablePointerExpression,
     ClassList_RealArrayVariablePointerExpression,
     ClassList_ArrayFunctionExpression );


@BaseClassList = (
     "",
     ROSE_Node,
     ROSE_IndexExpression,
     ROSE_IndexOperand,
     ROSE_IndexingVariable,
     ROSE_IndexingVariableReference,
     ROSE_IndexingVariablePointer,
     ROSE_Type,
     ROSE_IndexingType,
     ROSE_ArrayType,
     ROSE_RealArrayType,
     ROSE_Expression,
     ROSE_ArrayExpression,
     ROSE_ArrayOperator,
     ROSE_UnaryArrayOperator,
     ROSE_UnaryLogicalArrayOperator,
     ROSE_BinaryArrayOperator,
     ROSE_BinaryTestingArrayOperator,
     ROSE_BinaryLogicalArrayOperator,
     ROSE_Statement,
     ROSE_VariableDeclaration,
     ROSE_ArrayStatement,
     ROSE_NumericExpression,
     ROSE_TestingExpression,
     ROSE_LogicalExpression,
     ROSE_ArrayOperandExpression,
     ROSE_ArrayVariableExpression,
     ROSE_RealArrayVariableExpression,
     ROSE_ArrayVariableReferenceExpression,
     ROSE_RealArrayVariableReferenceExpression,
     ROSE_ArrayVariablePointerExpression,
     ROSE_RealArrayVariablePointerExpression,
     ROSE_ArrayFunctionExpression );


###############################
# Start of Function Definitions
###############################

sub buildMemberFunctionDeclarations
   {
     local($returnString);
     $returnString = "NULL";
     if ($_[0] eq "ROSE_Node")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_IndexArgumentList")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_IndexExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_IndexExpressionBinaryOperatorCExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_CExpressionBinaryOperatorIndexExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_UnaryOperatorIndexOperand")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_IndexOperand")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_IndexingVariable")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_IndexVariable")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_RangeVariable")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_IndexingVariableReference")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_IndexVariableReference")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_RangeVariableReference")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_IndexingVariablePointer")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_IndexVariablePointer")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_RangeVariablePointer")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_IndexingVariableDereferencedPointer")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_IndexVariableDereferencedPointer")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_RangeVariableDereferencedPointer")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_Type")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayType")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_RealArrayType")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_doubleArrayType")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_floatArrayType")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_intArrayType")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_IndexingType")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_IndexType")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_RangeType")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArgumentList")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_UserFunction")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_ArrayOperator")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_UnaryArrayOperator")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_UnaryArrayOperatorMinus")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_UnaryArrayOperatorPlus")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_UnaryArrayOperatorPrefixPlusPlus")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_UnaryArrayOperatorPostfixPlusPlus")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_UnaryArrayOperatorPrefixMinusMinus")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_UnaryArrayOperatorPostfixMinusMinus")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_UnaryArrayLogicalOperator")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_UnaryArrayOperatorNOT")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperator")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorEquals")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorAdd")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorAddEquals")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorMinus")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorMinusEquals")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorMultiply")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorMultiplyEquals")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorDivide")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorDivideEquals")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorModulo")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorModuloEquals")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayTestingOperator")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorLT")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorLTEquals")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorGT")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorGTEquals")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayLogicalOperator")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorEquivalence")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorNOTEquals")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorLogicalAND")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorLogicalOR")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_Statement")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_VariableDeclaration")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_C_VariableDeclaration")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_ArrayVariableDeclaration")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_IndexingVariableDeclaration")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_C_Statement")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_ArrayStatement")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_VariableDeclaration")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ExpressionStatement")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_StatementBlock")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_WhereStatement")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_ElseWhereStatement")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_DoWhileStatement")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_WhileStatement")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ForStatement")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_IfStatement")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_ReturnStatement")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_Expression")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_ArrayExpression")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_C_Expression")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_NumericExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayOperandBinaryArrayOperatorArrayOperand")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayOperandBinaryArrayOperatorArrayExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_UnaryArrayOperatorArrayOperand")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_UnaryArrayOperatorArrayExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_TestingExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayOperandBinaryArrayTestingOperatorArrayOperand")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayOperandBinaryArrayTestingOperatorArrayExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_LogicalExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayOperandBinaryArrayLogicalOperatorArrayOperand")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayOperandBinaryArrayLogicalOperatorArrayExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayOperandExpression")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_ArrayVariableExpression")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_RealArrayVariableExpression")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_doubleArrayVariableExpression")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_floatArrayVariableExpression")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_intArrayVariableExpression")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_ArrayVariableReferenceExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_RealArrayVariableReferenceExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_doubleArrayVariableReferenceExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_floatArrayVariableReferenceExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_intArrayVariableReferenceExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayVariablePointerExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_RealArrayVariablePointerExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_doubleArrayVariablePointerExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_floatArrayVariablePointerExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_intArrayVariablePointerExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayVariableDereferencedPointerExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_RealArrayVariableDereferencedPointerExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_doubleArrayVariableDereferencedPointerExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_floatArrayVariableDereferencedPointerExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_intArrayVariableDereferencedPointerExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayParenthesisOperator")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_ArrayFunctionExpression")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_ArrayCOS")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArraySIN")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayTAN")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayMIN")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayMAX")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayFMOD")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayMOD")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayPOW")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArraySIGN")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayLOG")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayLOG10")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayEXP")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArraySQRT")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_ArrayFABS")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayCEIL")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayFLOOR")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayABS")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayTRANSPOSE")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayACOS")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayASIN")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayATAN")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayCOSH")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArraySINH")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayTANH")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayACOSH")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayASINH")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayATANH")
        { $returnString = ""; }
     else
        { die "can not find in class declaration list $_[0]" }

     $returnString;
   }

sub buildMemberFunctionDefinitions
   {
     local($returnString);
     $returnString = "NULL";
     if ($_[0] eq "ROSE_Node")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_IndexArgumentList")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_IndexExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_IndexExpressionBinaryOperatorCExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_CExpressionBinaryOperatorIndexExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_UnaryOperatorIndexOperand")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_IndexOperand")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_IndexingVariable")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_IndexVariable")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_RangeVariable")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_IndexingVariableReference")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_IndexVariableReference")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_RangeVariableReference")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_IndexingVariablePointer")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_IndexVariablePointer")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_RangeVariablePointer")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_IndexingVariableDereferencedPointer")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_IndexVariableDereferencedPointer")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_RangeVariableDereferencedPointer")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_Type")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayType")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_RealArrayType")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_doubleArrayType")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_floatArrayType")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_intArrayType")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_IndexingType")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_IndexType")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_RangeType")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArgumentList")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_UserFunction")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_ArrayOperator")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_UnaryArrayOperator")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_UnaryArrayOperatorMinus")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_UnaryArrayOperatorPlus")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_UnaryArrayOperatorPrefixPlusPlus")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_UnaryArrayOperatorPostfixPlusPlus")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_UnaryArrayOperatorPrefixMinusMinus")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_UnaryArrayOperatorPostfixMinusMinus")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_UnaryArrayLogicalOperator")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_UnaryArrayOperatorNOT")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperator")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorEquals")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorAdd")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorAddEquals")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorMinus")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorMinusEquals")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorMultiply")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorMultiplyEquals")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorDivide")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorDivideEquals")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorModulo")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorModuloEquals")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayTestingOperator")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorLT")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorLTEquals")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorGT")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorGTEquals")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayLogicalOperator")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorEquivalence")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorNOTEquals")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorLogicalAND")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_BinaryArrayOperatorLogicalOR")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_Statement")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_VariableDeclaration")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_C_VariableDeclaration")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_ArrayVariableDeclaration")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_IndexingVariableDeclaration")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_C_Statement")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_ArrayStatement")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_VariableDeclaration")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ExpressionStatement")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_StatementBlock")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_WhereStatement")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_ElseWhereStatement")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_DoWhileStatement")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_WhileStatement")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ForStatement")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_IfStatement")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_ReturnStatement")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_Expression")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_ArrayExpression")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_C_Expression")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_NumericExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayOperandBinaryArrayOperatorArrayOperand")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayOperandBinaryArrayOperatorArrayExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_UnaryArrayOperatorArrayOperand")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_UnaryArrayOperatorArrayExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_TestingExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayOperandBinaryArrayTestingOperatorArrayOperand")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayOperandBinaryArrayTestingOperatorArrayExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_LogicalExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayOperandBinaryArrayLogicalOperatorArrayOperand")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayOperandBinaryArrayLogicalOperatorArrayExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayOperandExpression")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_ArrayVariableExpression")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_RealArrayVariableExpression")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_doubleArrayVariableExpression")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_floatArrayVariableExpression")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_intArrayVariableExpression")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_ArrayVariableReferenceExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_RealArrayVariableReferenceExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_doubleArrayVariableReferenceExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_floatArrayVariableReferenceExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_intArrayVariableReferenceExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayVariablePointerExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_RealArrayVariablePointerExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_doubleArrayVariablePointerExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_floatArrayVariablePointerExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_intArrayVariablePointerExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayVariableDereferencedPointerExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_RealArrayVariableDereferencedPointerExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_doubleArrayVariableDereferencedPointerExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_floatArrayVariableDereferencedPointerExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_intArrayVariableDereferencedPointerExpression")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayParenthesisOperator")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_ArrayFunctionExpression")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_ArrayCOS")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArraySIN")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayTAN")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayMIN")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayMAX")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayFMOD")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayMOD")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayPOW")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArraySIGN")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayLOG")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayLOG10")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayEXP")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArraySQRT")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_ArrayFABS")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayCEIL")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayFLOOR")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayABS")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayTRANSPOSE")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayACOS")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayASIN")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayATAN")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayCOSH")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArraySINH")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayTANH")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayACOSH")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayASINH")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_ArrayATANH")
        { $returnString = ""; }
     else
        { die "can not find in member function definition list $_[0]" }

     $returnString;
   }

######################################
# Start of Simple Function Definitions
######################################

sub copyNodeClassDeclaration 
   {
     $line = "$ARGV[0]";
     $file = "$line/node.macro";
   # print $file;
   # exit 1;
     open(FILE,"$file") || die "cannot open file $file!" ;

     while( <FILE> )
        {
          $line = $_;
          print OUTFILE $line;
        # print $line;
        }
   }

sub readClassDeclarationBody
   {
     &readInFileToString ("classDeclarationBody_$_[0].macro",$_[0]);
   }

sub readMemberFunctionDefinition
   {
     &readInFileToString ("memberFunctionDefinition_$_[0].macro",$_[0]);
   }

sub readInFileToString
   {
     $line = "$ARGV[0]";
   # print "Inside of readInFileToString($_[0]) \n";
     local($readInFileToString_file);
     $readInFileToString_file = $_[0];
     open(readInFileToString_FILE,"$line/$readInFileToString_file") || die "cannot open file $readInFileToString_file!" ;

     local($line);
     local($returnStr);
     while( <readInFileToString_FILE> )
        {
          $line = $_;
          $line =~ s/\bCLASSNAME\b/$_[1]/g;
          $line =~ s/\bCLASSNAME_TAG\b/$_[1]Tag/g;
          $returnStr = $returnStr . $line;
        # print $line;
        }

     close(readInFileToString_FILE);
     $returnStr;
   }

sub classDeclaration
   {
     $line = "$ARGV[0]";
     $file = "$line/grammarClassDeclatationMacros.macro";
     open(FILE,"$file") || die "cannot open file $file!";

   # Compute this once
     $memberFunctionDeclarations = &buildMemberFunctionDeclarations($_[0]);

   # print "Member function data = $memberFunctionDeclarations \n";

     while( <FILE> )
        {
          $line = $_;
          $line =~ s/\bCLASSNAME\b/$_[0]/g;
          $line =~ s/\bCLASSNAME_TAG\b/$_[0]Tag/g;
          $line =~ s/\bBASECLASSNAME\b/$_[1]/g;
          $line =~ s/\bMEMBER_FUNCTION_DECLARATIONS\b/$memberFunctionDeclarations/g;
          print OUTFILE $line;
        # print $line;
        }
   }

sub BuildClassDeclatations
   {
     foreach $class ( @{$_[0]} )
        {
        # print "Generating $class derived from $_[1] \n";
          classDeclaration($class,": public $_[1]");
	}
   }

sub BuildGrammarDeclarations
   {
   # Copy the node.h file into the output file
     copyNodeClassDeclaration;

     local($list);
     local($baseClass);
     local($i);
     for ($i=1; $i <= @ListOfLists; $i++)
        {
          $list = @ListOfLists[$i];
          $baseClass = @BaseClassList[$i];
          BuildClassDeclatations ($list,$baseClass);
        }
   }

sub classDefinition
   {
   # Compute this once
     $memberFunctionDefinitions  = &buildMemberFunctionDefinitions ($_[0]);
     $memberFunctionDefinitions =~ s/\bBASECLASSNAME\b/$_[1]/g;
     print OUTFILE_SOURCE $memberFunctionDefinitions;

     if ( $_[0] ne "ROSE_Node")
        {
          $file = "$ARGV[0]/grammarClassDefinitionMacros.macro";
          open(DEFINITION_FILE,"$file") || die "cannot open file $file!";

          while( <DEFINITION_FILE> )
             {
               $line = $_;
               $line =~ s/\bCLASSNAME\b/$_[0]/g;
               $line =~ s/\bCLASSNAME_TAG\b/$_[0]Tag/g;
               print OUTFILE_SOURCE $line;
             # print $line;
             }
        }
   }

sub BuildClassDefinitions
   {
     foreach $class ( @{$_[0]} )
        {
        # print "Generating $class member functions derived from $_[1] \n";
          classDefinition($class,$_[1]);
	}
   }

sub BuildGrammarDefinitions
   {
     local($list);
     local($baseClass);
     local($i);
     for ($i=0; $i <= @ListOfLists; $i++)
        {
          $list = @ListOfLists[$i];
          $baseClass = @BaseClassList[$i];
          BuildClassDefinitions ($list,$baseClass);
        }
   }

sub BuildVariantsEnum
   {
     print OUTFILE "\n\n";
#    print OUTFILE "extern arrayGrammarTerminalNames; \n";
#    print OUTFILE "extern arrayGrammarTerminalNames; \n";
#    print OUTFILE "\n\n";
     print OUTFILE "enum ROSE_GrammarVariants \n";
     print OUTFILE "   {\n";
     print OUTFILE "// Variants used to identify the grammar used in ROSE \n";
     print OUTFILE "\n";
     foreach $class ( @ClassList )
        {
          print OUTFILE "     ${class}Tag, \n";
	}

     print OUTFILE "     ROSE_UNKNOWN_GRAMMAR, \n";
     print OUTFILE "     ROSE_LAST_TAG \n";
     print OUTFILE "   };\n";
     print OUTFILE "\n\n";
   }

sub BuildVariantsStringDataBase
   {
     $listLength = 0;
     foreach $class ( @ClassList )
        {
          $listLength++;
	}

     $listLength++;
     print OUTFILE_SOURCE "\n\n";

     print OUTFILE_SOURCE "struct \n";
     print OUTFILE_SOURCE "   \{ \n";
     print OUTFILE_SOURCE "     ROSE_GrammarVariants variant; \n";
     print OUTFILE_SOURCE "     char *name;                   \n";
     print OUTFILE_SOURCE "   } arrayGrammarTerminalNames[$listLength] = {  \n";

     foreach $class ( @ClassList )
        {
          print OUTFILE_SOURCE "          {${class}Tag, \"${class}\"}, \n";
	}

     print OUTFILE_SOURCE "          {ROSE_LAST_TAG, \"last tag\" } \n";
     print OUTFILE_SOURCE "  \}; \n";
   }

sub BuildForwardDeclarations
   {
     print OUTFILE "\n\n";
     print OUTFILE "// Forward Declarations used to represent the grammar used in ROSE \n";
     foreach $class ( @ClassList )
        {
          print OUTFILE "class ${class};\n";
	}

     print OUTFILE "\n\n";
   }

############################
# Start of MAIN Perl program
############################

# Put the header file into the $top_builddir/include directory
if ($ARGV[1] eq "")
   {
     $outfile="grammar.h";
   }
  else
   {
     $outfile="$ARGV[1]/grammar.h";
   }

open(OUTFILE,">$outfile") || die "cannot open file $outfile!" ;

$line = "// MACHINE GENERATED HEADER FILE --- DO NOT MODIFY! \n\n";
print OUTFILE $line;
$line = "\#ifndef ARRAY_GRAMMAR_H\n";
print OUTFILE $line;
$line = "\#define ARRAY_GRAMMAR_H\n\n";
print OUTFILE $line;
$line = "\#include <tlist.h>\n\n";
print OUTFILE $line;

# We want the lists to be objects that have ROSE_Node as a base class just as the 
# SgExprListExp is an object in Sage and also a list of SgExpression objects too.
# $typedefStatement = "typedef ROSE_IndexArgumentList list<ROSE_IndexExpression>;\n";
# print OUTFILE $typedefStatement;
# $typedefStatement = "typedef ROSE_ArgumentList      list<ROSE_ArrayExpression>;\n";
# print OUTFILE $typedefStatement;

# Build enum for variants
BuildVariantsEnum;

#Build the forward declarations for all the classes which implement the grammar
BuildForwardDeclarations;

# Now build all the class declarations for the rest of the grammar
BuildGrammarDeclarations;

$line = "\#endif // ifndef ARRAY_GRAMMAR_H \n";
print OUTFILE $line;

close(FILE);
close(OUTFILE);

# Generate the Class Member Function Definitions
$outfileSource="grammar.C";
open(OUTFILE_SOURCE,">$outfileSource") || die "cannot open file $outfileSource!" ;

$line = "// MACHINE GENERATED SOURCE FILE --- DO NOT MODIFY! \n\n";
print OUTFILE_SOURCE $line;

# New version using autoconf and automake configuration tools requires all C++ source to have this line
$line = "#ifdef HAVE_CONFIG_H \n#include \"config.h\" \n#endif \n\n";
print OUTFILE_SOURCE $line;

# line = "#include \"transformation_2.h\" \n\n";
$line = "#include \"rose.h\" \n\n";
print OUTFILE_SOURCE $line;

$line = "#include \"ROSE_Transformations.h\" \n\n";
print OUTFILE_SOURCE $line;

BuildVariantsStringDataBase;

# Now build all the class declarations for the rest of the grammar
BuildGrammarDefinitions;

close(OUTFILE_SOURCE);

exit;








