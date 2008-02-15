/*
   Implementation Node:
       Express these as queries to avoid aliasing of nodes in the AST
       and the requirement of a complex update mechanism.
 */

class AbstractionAttribute : public AstAttribute
   {
  // Store this attribute at each operator abstraction in the library

     public:
          AbstractionAttribute() {};

         int variant(); // make this a proper variant type???
   };

class ExpressionAttribute : public AbstractionAttribute // , SgExpression (?)
   {
  // Identify unary operators using this attribute and 
  // give it the same interface as the rest of the SAGE III IR(???)

     public:
          ExpressionAttribute() {};

          SgExpression* chaseFunctionCall ( SgExpression* exp );
   };

class UnaryOperatorAttribute : public ExpressionAttribute // , SgUnaryOp (?)
   {
  // Identify unary operators using this attribute and 
  // give it the same interface as the rest of the SAGE III IR(???)

     public:
          UnaryOperatorAttribute() {};

       // Express these as queries to avoid aliasing of nodes in the AST 
       // and the requirement of a complex update mechanism
          SgExpression* getOperand(SgFunctionCallExp*);
   };

class BinaryOperatorAttribute : public ExpressionAttribute // , SgBinaryOp (?)
   {
  // Identify binary operators using this attribute and 
  // give it the same interface as the rest of the SAGE III IR

     public:
       // list<SgNode*> operatorNodeList;

          BinaryOperatorAttribute() {};

       // Express these as queries to avoid aliasing of nodes in the AST 
       // and the requirement of a complex update mechanism
          SgExpression* getLhsOperand(SgFunctionCallExp* fc);
          SgExpression* getRhsOperand(SgFunctionCallExp* fc);
          list<SgNode*> getOperatorNodes(SgFunctionCallExp* fc);
   };

// PARENTHESIS
class ParenthesisOperatorAttribute : public ExpressionAttribute // , SgUnaryOp (?)
   {
  // Identify functions and member functions in the library

     public:
          ParenthesisOperatorAttribute() {};

          int getNumberOfIndexOperands(SgFunctionCallExp* fc);
          SgExpression* getIndex(SgFunctionCallExp* fc,unsigned int n);
          SgExpression* getIndexedOperand(SgFunctionCallExp* fc);

       // SgExprListExp* getOperatorParameterList(SgFunctionCallExp*);

          list<SgNode*> getOperatorNodes(SgFunctionCallExp* fc);
   };

class OperatorAttribute : public ExpressionAttribute // , SgUnaryOp (?)
   {
  // Identify functions and member functions in the library

     public:
          OperatorAttribute() {};

          SgExprListExp* getOperatorParameterList(SgFunctionCallExp*);
   };

// ********************************************************
// What resolution do we want to have regarding statements?
// ********************************************************

class StatementAttribute : public AbstractionAttribute // , SgStatement (?)
   {
  // Identify statements using library abstractions in the application

     public:
          StatementAttribute() {};
   };

class ExpressionStatementAttribute : public StatementAttribute // , SgStatement (?)
   {
  // Identify statements using library abstractions in the application.
  // Since expression statements are particularly interesting provide 
  // special support for these.

     public:
          ExpressionStatementAttribute() {};

          SgExpression* getExpression(SgExprStatement* exp);
          list<SgNode*> getOperatorNodes(SgExprStatement* exp);
   };









