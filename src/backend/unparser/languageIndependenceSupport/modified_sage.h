
/* unparser.h
 * This header file contains the class declaration for the newest unparser. Six
 * C files include this header file: unparser.C, modified_sage.C, unparse_stmt.C, 
 * unparse_expr.C, unparse_type.C, and unparse_sym.C.
 */

#ifndef UNPARSER_MOD_SAGE
#define UNPARSER_MOD_SAGE


#include "unparser.h"
class SgExpression;
class SgType;
class SgExprListExp;
class SgConstructorInitializer;

class Unparser;

class Unparse_MOD_SAGE
   {
     private:
          Unparser* unp;

     public:
          Unparse_MOD_SAGE(Unparser* unp);
          virtual ~Unparse_MOD_SAGE() {};

          int cur_get_linewrap ();

          void cur_set_linewrap (int nr);

          void curprint(std::string str);
          void curprint_newline();

      //! functions that test for overloaded operator function (modified_sage.C)
      //! Pattern matching on a single IR node (usually the SgMemberFunctionRefExp)
          bool isOperator(SgExpression* expr);
          bool isBinaryEqualsOperator(SgExpression* expr);
          bool isBinaryEqualityOperator(SgExpression* expr);
          bool isBinaryInequalityOperator(SgExpression* expr);
          bool isBinaryArithmeticOperator(SgExpression* expr);
          bool isBinaryParenOperator(SgExpression* expr);
          bool isBinaryBracketOperator(SgExpression* expr);
          bool isBinaryOperator(SgExpression* expr);

          bool isUnaryOperatorPlus(SgExpression* expr);
          bool isUnaryOperatorMinus(SgExpression* expr);
          bool isUnaryAddressOperator(SgExpression* expr);
          bool isUnaryDereferenceOperator(SgExpression* expr);
          bool isUnaryComplementOperator(SgExpression* expr);

       // DQ (2/12/2019): Adding support for C++11 literal operators.
          bool isUnaryLiteralOperator(SgExpression* expr);

       // DQ (2/1/2018): I don't think this operator can exist.
       // bool isUnaryOrOperator(SgExpression* expr);

       // DQ (2/1/2018): Added support for operator!
          bool isUnaryNotOperator(SgExpression* expr);

       // DQ (2/20/2005): The overloaded versions of these operators have to be recognized so that we can skip 
       // the output of their function arguments which control the interpritation as prefix/postfix operators 
       // when expressed using the function syntax.
          bool isUnaryIncrementOperator(SgExpression* expr);
          bool isUnaryDecrementOperator(SgExpression* expr);

          bool isUnaryOperator(SgExpression* expr);
          bool isUnaryPostfixOperator(SgExpression* expr);
          bool isOverloadedArrowOperator(SgExpression* expr);
          bool isIOStreamOperator(SgExpression* expr);

      //! Subtree query operators (pattern recognition on subtree, more complex 
      //! than the test for a function name on the SgMemberFunctionRefExp node).
      //! This function is used to get for the combination of "operator->()->operator[](i)"
      //! since in this case the unparsed code can not be rediced to "->[i]".
          bool isUnaryOperatorArrowSubtree(SgExpression* expr);

      //! auxiliary functions (some code from original modified_sage.C)
          bool NoDereference(SgExpression* expr);
          bool isCast_ConstCharStar(SgType* type);
          bool PrintStartParen(SgExpression* expr, SgUnparse_Info& info);
          bool RemovePareninExprList(SgExprListExp* expr_list);
          bool isOneElementList(SgConstructorInitializer* con_init);

          bool printConstructorName(SgExpression* expr);
          bool noQualifiedName(SgExpression* expr);

          void printSpecifier1(SgDeclarationStatement* decl_stmt, SgUnparse_Info& info);
          void printSpecifier2(SgDeclarationStatement* decl_stmt, SgUnparse_Info& info);
          void printSpecifier (SgDeclarationStatement* decl_stmt, SgUnparse_Info& info);

       // DQ (2/26/2013): Added support for missing attributes in unparsed code.
          void printAttributes(SgDeclarationStatement* decl_stmt, SgUnparse_Info& info);

       // DQ (2/27/2013): Added support for missing attributes in unparsed code.
          void printAttributes(SgInitializedName* initializedName, SgUnparse_Info& info);

       // DQ (1/19/2014): Added support for prefixed attributes (it makes a difference where they go...).
          void printPrefixAttributes(SgDeclarationStatement* decl_stmt, SgUnparse_Info& info);

       // DQ (12/31/2013): Added support for missing attributes in unparsed code (on types in variable declarations).
         void printAttributesForType(SgDeclarationStatement* decl_stmt, SgUnparse_Info& info);

          void directives(SgLocatedNode* lnode);

       // DQ (8/29/2005): Added to support differences in the order of "extern C" and "template<>" with g++ version 3.3.x and 3.4.x
          void outputExternLinkageSpecifier ( SgDeclarationStatement* decl_stmt );
          void outputTemplateSpecializationSpecifier ( SgDeclarationStatement* decl_stmt );


       // DQ (10/25/2006): Support for color code to identify errors in source code position, 
       // compiler generated code, and general properties of the code.
      //! Support for color codes in unparse output.
          void printColorCodes ( SgNode* node, bool openState, std::vector< std::pair<bool,std::string> > & stateVector );

          void setupColorCodes ( std::vector< std::pair<bool,std::string> > & stateVector );

       // DQ (2/4/2006): Removed since this is not used
       // DQ (4/3/2004): Added to output modifiers (e.g. register) in formal function arguments
       // void printFunctionFormalArgumentSpecifier ( SgType* type, SgUnparse_Info& info );

       // MS: temporary flag for experiments with uparsing of template instantiations
          static bool experimentalMode;
          static int experimentalModeVerbose;
   private:
          void outputTemplateSpecializationSpecifier2 ( SgDeclarationStatement* decl_stmt );

   };

#endif


int GetOperatorVariant(SgExpression* expr);
SgExpression* GetFirstOperand(SgExpression* expr);
