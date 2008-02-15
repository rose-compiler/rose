// MACHINE GENERATED HEADER FILE --- DO NOT MODIFY! 

#ifndef TRANSFORMABLE_GRAMMAR_3_H
#define TRANSFORMABLE_GRAMMAR_3_H


enum ROSE_TransformableGrammarVariants 
   {
// Variants used to identify the grammar used in ROSE 

     ROSE_TransformableNodeTag, 
     ROSE_TransformableStatementTag, 
     ROSE_NonTransformableStatementTag, 
     ROSE_TransformableStatementBlockTag, 
     ROSE_TransformableExpressionTag, 
     ROSE_TransformableOperatorExpressionTag, 
     ROSE_TransformableUnaryOperatorExpressionTag, 
     ROSE_TransformableBinaryOperatorExpressionTag, 
     ROSE_TransformableBinaryOperatorEqualsTag, 
     ROSE_TransformableBinaryOperatorNonAssignmentTag, 
     ROSE_TransformableOperandExpressionTag, 
     ROSE_NonTransformableExpressionTag, 
     ROSE_TRANSFORMABLE_LAST_TAG 
   };




// Forward Declarations used to represent the grammar used in ROSE 
class ROSE_TransformableNode;
class ROSE_TransformableStatement;
class ROSE_NonTransformableStatement;
class ROSE_TransformableStatementBlock;
class ROSE_TransformableExpression;
class ROSE_TransformableOperatorExpression;
class ROSE_TransformableUnaryOperatorExpression;
class ROSE_TransformableBinaryOperatorExpression;
class ROSE_TransformableBinaryOperatorEquals;
class ROSE_TransformableBinaryOperatorNonAssignment;
class ROSE_TransformableOperandExpression;
class ROSE_NonTransformableExpression;



// class ROSE_TreeBaseClass
class ROSE_TransformableNode // : public ArrayAssignmentUsingTransformationGrammar // : public ROSE_Node
   {
     public:
         ~ROSE_TransformableNode ();

          ROSE_TransformableNode ( ROSE_Node* subTree );

          void setUp ( ROSE_TransformableGrammarVariants newId, ROSE_Node* newSubTree );
          ROSE_TransformableGrammarVariants getVariant();
          void setVariant ( ROSE_TransformableGrammarVariants id );

       // This make this class an abstract base class
          virtual ROSE_TransformableNode* transform() = 0;

       // Error recovery mechanism (for the parser)
          Boolean error();
          void setParseError ( Boolean X );

          const ROSE_Node* getRoseSubTree();
          void setRoseSubTree ( const ROSE_Node* node );

     private:
	  ROSE_TransformableGrammarVariants id;
          ROSE_Node* roseSubTree;
          Boolean parseError;  // error recovery mechanism for parser

  // We don't want to have these be used within the interface
     private:
          ROSE_TransformableNode ();
          ROSE_TransformableNode ( const ROSE_TransformableNode & X );
          ROSE_TransformableNode & operator= ( const ROSE_TransformableNode & X );
   };


// Class Definition for ROSE_TransformableStatement
class ROSE_TransformableStatement : public ROSE_TransformableNode
   {
     public:
         ~ROSE_TransformableStatement ();

          StatementDataBase* localDataBase;

       // Special constructor for ROSE_TransformableStatement;
          ROSE_TransformableStatement ( ROSE_Statement* stmt );

       // Make this class an abstract base class so that we can be certain that
       // we will not ever try to build one of these directly.
          virtual ROSE_TransformableNode* transform ();

          ROSE_Statement* getRoseStatement ();
          void setRoseStatement ( ROSE_Statement* stmt );

       // ROSE_TransformableExpression* getExpression ();
       // void setExpression ( ROSE_TransformableExpression* expr );
          ROSE_TransformableBinaryOperatorEquals* getExpression ();
          void setExpression ( ROSE_TransformableBinaryOperatorEquals* expr );

       // The transformation process is divided into several subtransformations
          void oncePerScopeSubtransformation                  ( SgStatement *currentStatement );
          void oncePerTransformationSubtransformation         ( SgStatement *currentStatement );
          void repeatedForEachTransformationSubtransformation ( SgStatement *currentStatement );
          void outerLoopStructureSubtransformation            ( SgStatement *currentStatement, SgStatement* transformedStatement );
          void buildForLoopStructureAndModifySageTree         ( SgStatement *exprStmt,         SgStatement *transformedStatement );

       // Data base access functions
          void setUpDataBase();
          int numberOfRhsOperands();

     private:
       // This grammar (and the obejcts implementing its pieces) are very specific
       // thus we specify here that a TransformableStatement IS a statement containing
       // a TransformableBinaryOperatorEquals.  This is much more specific than saying 
       // that a statement contains an experssion.
          ROSE_TransformableBinaryOperatorEquals* transformableBinaryOperatorEquals;





     public:
       // We want to make this a pure virtual function in the base 
       // classes and a virtual function in the leaf classes
       // virtual ROSE_Node* transform() = 0;

       // Member function defined similarly across all array grammar objects
       // this MUST be a virtual function
          virtual Boolean error();

  // We don't want to have these be used within the interface
     private:
          ROSE_TransformableStatement ();
          ROSE_TransformableStatement ( const ROSE_TransformableStatement & X );
          ROSE_TransformableStatement & operator= ( const ROSE_TransformableStatement & X );
   };



// Class Definition for ROSE_TransformableExpression
class ROSE_TransformableExpression : public ROSE_TransformableNode
   {
     public:
         ~ROSE_TransformableExpression ();

       // Special constructor for this class
          ROSE_TransformableExpression ( ROSE_Expression* expr );

       // Make this class an abstract base class so that we can be certain that
       // we will not ever try to build one of these directly.
          virtual ROSE_TransformableNode* transform() = 0;

          void setRoseExpression ( ROSE_Expression* );
          ROSE_Expression* getRoseExpression ();

     private:
       // ROSE_Expression* roseExpression;

     public:
       // We want to make this a pure virtual function in the base 
       // classes and a virtual function in the leaf classes
       // virtual ROSE_Node* transform() = 0;

       // Member function defined similarly across all array grammar objects
       // this MUST be a virtual function
          virtual Boolean error();

  // We don't want to have these be used within the interface
     private:
          ROSE_TransformableExpression ();
          ROSE_TransformableExpression ( const ROSE_TransformableExpression & X );
          ROSE_TransformableExpression & operator= ( const ROSE_TransformableExpression & X );
   };



// Class Definition for ROSE_NonTransformableStatement
class ROSE_NonTransformableStatement : public ROSE_TransformableStatement
   {
     public:
         ~ROSE_NonTransformableStatement ();

       // Special constructor for ROSE_NonTransformableStatement;
          ROSE_NonTransformableStatement ( ROSE_Statement* stmt );

       // Make this class an abstract base class so that we can be certain that
       // we will not ever try to build one of these directly.
          virtual ROSE_TransformableNode* transform ();

          ROSE_Statement* getRoseStatement ();
          void setRoseStatement ( ROSE_Statement* stmt );

          ROSE_TransformableExpression* getTransformableExpression ();
          void setTransformableExpression ( ROSE_TransformableExpression* expr );

     private:
          ROSE_TransformableExpression* nonTransformableExpression;


     public:
       // We want to make this a pure virtual function in the base 
       // classes and a virtual function in the leaf classes
       // virtual ROSE_Node* transform() = 0;

       // Member function defined similarly across all array grammar objects
       // this MUST be a virtual function
          virtual Boolean error();

  // We don't want to have these be used within the interface
     private:
          ROSE_NonTransformableStatement ();
          ROSE_NonTransformableStatement ( const ROSE_NonTransformableStatement & X );
          ROSE_NonTransformableStatement & operator= ( const ROSE_NonTransformableStatement & X );
   };



// Class Definition for ROSE_TransformableStatementBlock
class ROSE_TransformableStatementBlock : public ROSE_TransformableStatement
   {
     public:
         ~ROSE_TransformableStatementBlock ();

       // Special constructor for ROSE_TransformableStatementBlock;
          ROSE_TransformableStatementBlock ( ROSE_StatementBlock* roseStatementBlock );

       // Define the abstractBaseClass function
          virtual ROSE_TransformableNode* transform();

          void recognizeRoseSubTree ( ROSE_StatementBlock* roseStatementBlock );

          int  getNumberOfStatements ();
          void addTransformableStatementToBlock ( ROSE_TransformableStatement* roseStatement );
          ROSE_TransformableStatement* operator[](int i);

     private:
       // We are using the List<T> list class here because we can't use STL 
       // (SAGE II causes multiple defines when used with STL, not clear if this is fixable)
          List<ROSE_TransformableStatement> transformableStatementList;



     public:
       // We want to make this a pure virtual function in the base 
       // classes and a virtual function in the leaf classes
       // virtual ROSE_Node* transform() = 0;

       // Member function defined similarly across all array grammar objects
       // this MUST be a virtual function
          virtual Boolean error();

  // We don't want to have these be used within the interface
     private:
          ROSE_TransformableStatementBlock ();
          ROSE_TransformableStatementBlock ( const ROSE_TransformableStatementBlock & X );
          ROSE_TransformableStatementBlock & operator= ( const ROSE_TransformableStatementBlock & X );
   };



// Class Definition for ROSE_TransformableOperatorExpression
class ROSE_TransformableOperatorExpression : public ROSE_TransformableExpression
   {
     public:
         ~ROSE_TransformableOperatorExpression ();

       // Special constructor for this class
          ROSE_TransformableOperatorExpression ( ROSE_Expression* expr );

       // Make this class an abstract base class so that we can be certain that
       // we will not ever try to build one of these directly.
          virtual ROSE_TransformableNode* transform() = 0;

       // void addArgumentExpression ( ROSE_Expression* roseExpression );
       // int  numberOfArguments();

       // void           setSageExpressionListExpression ( SgExprListExp* exprListExp );
       // SgExprListExp* getSageExpressionListExpression();

       // List<ROSE_Expression> & getRoseExpressionList ();

     private:
       // SgExprListExp* sageExpressionListExpression;
       // List<ROSE_Expression> roseExpressionList;







     public:
       // We want to make this a pure virtual function in the base 
       // classes and a virtual function in the leaf classes
       // virtual ROSE_Node* transform() = 0;

       // Member function defined similarly across all array grammar objects
       // this MUST be a virtual function
          virtual Boolean error();

  // We don't want to have these be used within the interface
     private:
          ROSE_TransformableOperatorExpression ();
          ROSE_TransformableOperatorExpression ( const ROSE_TransformableOperatorExpression & X );
          ROSE_TransformableOperatorExpression & operator= ( const ROSE_TransformableOperatorExpression & X );
   };



// Class Definition for ROSE_TransformableOperandExpression
class ROSE_TransformableOperandExpression : public ROSE_TransformableExpression
   {
     public:
         ~ROSE_TransformableOperandExpression ();

       // Special constructor for ROSE_TransformableOperandExpression;
          ROSE_TransformableOperandExpression ( ROSE_Expression* expr );

       // Define the abstractBaseClass function
       // We later want to make this a pure virtual function (making this class and abstract base class)
#if 1
          virtual ROSE_TransformableNode* transform();
#else
          virtual ROSE_TransformableNode* transform() = 0;
#endif
          void recognizeSageSubTree ( ROSE_Expression* expr );

#if 0
       // Stuff about the indexing of array operands
          Boolean isAnIndexedOperand();
          int numberOfDimensions();

       // Rules on how we modify variable names (unique names are required)
          char* transformationVariablePrefix();
          char* transformationVariableSuffix();

       // get the names of the different parts
          char* getOperandName ();
          char* getIndexName (int dimension);

       // Build a new operand as a SgExpression*
          SgExpression* buildOperand ( char* arrayName,                 // variable name
                                       variant_type_enum arrayType,     // Type for the array pointer
                                       char* indexName,                 // variable name
                                       variant_type_enum indexOperator, // ADD_OP, SUBT_OP, etc. (semantics of A++/P++ Index and Range objects only)
                                       SgExpression* rhsExpression );   // integer expression for the rhs of the indexOperator
#endif

     public:
       // We want to make this a pure virtual function in the base 
       // classes and a virtual function in the leaf classes
       // virtual ROSE_Node* transform() = 0;

       // Member function defined similarly across all array grammar objects
       // this MUST be a virtual function
          virtual Boolean error();

  // We don't want to have these be used within the interface
     private:
          ROSE_TransformableOperandExpression ();
          ROSE_TransformableOperandExpression ( const ROSE_TransformableOperandExpression & X );
          ROSE_TransformableOperandExpression & operator= ( const ROSE_TransformableOperandExpression & X );
   };



// Class Definition for ROSE_NonTransformableExpression
class ROSE_NonTransformableExpression : public ROSE_TransformableExpression
   {
     public:
         ~ROSE_NonTransformableExpression ();

       // Special constructor for this class
          ROSE_NonTransformableExpression ( ROSE_Expression* expr );

       // Make this class an abstract base class so that we can be certain that
       // we will not ever try to build one of these directly.
          virtual ROSE_TransformableNode* transform();

       // void setRoseExpression ( ROSE_Expression* );
       // ROSE_Expression* getRoseExpression ();

     private:
       // ROSE_Expression* roseExpression;

     public:
       // We want to make this a pure virtual function in the base 
       // classes and a virtual function in the leaf classes
       // virtual ROSE_Node* transform() = 0;

       // Member function defined similarly across all array grammar objects
       // this MUST be a virtual function
          virtual Boolean error();

  // We don't want to have these be used within the interface
     private:
          ROSE_NonTransformableExpression ();
          ROSE_NonTransformableExpression ( const ROSE_NonTransformableExpression & X );
          ROSE_NonTransformableExpression & operator= ( const ROSE_NonTransformableExpression & X );
   };



// Class Definition for ROSE_TransformableUnaryOperatorExpression
class ROSE_TransformableUnaryOperatorExpression : public ROSE_TransformableOperatorExpression
   {
     public:
         ~ROSE_TransformableUnaryOperatorExpression ();

       // Special constructor for this class
          ROSE_TransformableUnaryOperatorExpression ( ROSE_Expression* expr );

       // Make this class an abstract base class so that we can be certain that
       // we will not ever try to build one of these directly.
          virtual ROSE_TransformableNode* transform();

       // Member functions specific to ROSE_TransformableUnaryOperatorExpression
          virtual ROSE_Expression* getRoseExpression();
          virtual void setRoseExpression ( ROSE_Expression* expr );

          virtual void setTransformableExpression ( ROSE_TransformableExpression* expr );
          virtual ROSE_TransformableExpression* getTransformableExpression ();

       // void setSageBinaryOperator ( SgBinaryOp* newBinaryOperator );
       // SgBinaryOp* getSageBinaryOperator ();

     protected:
       // SgBinaryOp* sageBinaryOperator;

          ROSE_TransformableExpression* transformableExpression;


     public:
       // We want to make this a pure virtual function in the base 
       // classes and a virtual function in the leaf classes
       // virtual ROSE_Node* transform() = 0;

       // Member function defined similarly across all array grammar objects
       // this MUST be a virtual function
          virtual Boolean error();

  // We don't want to have these be used within the interface
     private:
          ROSE_TransformableUnaryOperatorExpression ();
          ROSE_TransformableUnaryOperatorExpression ( const ROSE_TransformableUnaryOperatorExpression & X );
          ROSE_TransformableUnaryOperatorExpression & operator= ( const ROSE_TransformableUnaryOperatorExpression & X );
   };



// Class Definition for ROSE_TransformableBinaryOperatorExpression
class ROSE_TransformableBinaryOperatorExpression : public ROSE_TransformableOperatorExpression
   {
     public:
         ~ROSE_TransformableBinaryOperatorExpression ();

       // Special constructor for this class
          ROSE_TransformableBinaryOperatorExpression ( ROSE_Expression* expr );

       // Make this class an abstract base class so that we can be certain that
       // we will not ever try to build one of these directly.
          virtual ROSE_TransformableNode* transform() = 0;
          ROSE_TransformableNode* assembleTransform ( ROSE_NonTransformableExpression* lhs,
                                                      ROSE_NonTransformableExpression* rhs );

       // Member functions specific to ROSE_TransformableBinaryOperatorExpression
          virtual ROSE_Expression* getLhsRoseExpression();
          virtual ROSE_Expression* getRhsRoseExpression();

          virtual void setLhsRoseExpression ( ROSE_Expression* expr );
          virtual void setRhsRoseExpression ( ROSE_Expression* expr );

          virtual void setLhsTransformableExpression ( ROSE_TransformableExpression* transformableExpr );
          virtual void setRhsTransformableExpression ( ROSE_TransformableExpression* transformableExpr );

          virtual ROSE_TransformableExpression* getLhsTransformableExpression ();
          virtual ROSE_TransformableExpression* getRhsTransformableExpression ();

       // void setSageBinaryOperator ( SgBinaryOp* newBinaryOperator );
       // SgBinaryOp* getSageBinaryOperator ();

     protected:
       // SgBinaryOp* sageBinaryOperator;

          ROSE_TransformableExpression* lhsTransformableExpression;
          ROSE_TransformableExpression* rhsTransformableExpression;


     public:
       // We want to make this a pure virtual function in the base 
       // classes and a virtual function in the leaf classes
       // virtual ROSE_Node* transform() = 0;

       // Member function defined similarly across all array grammar objects
       // this MUST be a virtual function
          virtual Boolean error();

  // We don't want to have these be used within the interface
     private:
          ROSE_TransformableBinaryOperatorExpression ();
          ROSE_TransformableBinaryOperatorExpression ( const ROSE_TransformableBinaryOperatorExpression & X );
          ROSE_TransformableBinaryOperatorExpression & operator= ( const ROSE_TransformableBinaryOperatorExpression & X );
   };



// Class Definition for ROSE_TransformableBinaryOperatorEquals
class ROSE_TransformableBinaryOperatorEquals : public ROSE_TransformableBinaryOperatorExpression
   {
     public:
         ~ROSE_TransformableBinaryOperatorEquals ();

       // Special constructor for this class
          ROSE_TransformableBinaryOperatorEquals ( ROSE_Expression* expr );

       // Make this class an abstract base class so that we can be certain that
       // we will not ever try to build one of these directly.
          virtual ROSE_TransformableNode* transform();

       // virtual ROSE_TransformableNode* transform ( ROSE_Expression* lhs, ROSE_Expression* rhs );
       // virtual ROSE_TransformableNode* transform ( ROSE_NonTransformableExpression* lhs,
       //                                             ROSE_NonTransformableExpression* rhs );

       // ROSE_ArrayOperandExpression*  getLhsTransformableExpression();
       // ROSE_Expression*              getRhsTransformableExpression();

       // We want to define a special implementation of this function since 
       // we want to return a ROSE_TransformableOperandExpression*
          ROSE_TransformableExpression* getLhsTransformableExpression();

       // Use the base class implementation of this function
       // ROSE_TransformableExpression* getRhsTransformableExpression();

       // We need this to simplify the casting of the base class getRoseExpression() member function
          ROSE_BinaryArrayOperatorEquals* getRoseExpression ();

     public:
       // We want to make this a pure virtual function in the base 
       // classes and a virtual function in the leaf classes
       // virtual ROSE_Node* transform() = 0;

       // Member function defined similarly across all array grammar objects
       // this MUST be a virtual function
          virtual Boolean error();

  // We don't want to have these be used within the interface
     private:
          ROSE_TransformableBinaryOperatorEquals ();
          ROSE_TransformableBinaryOperatorEquals ( const ROSE_TransformableBinaryOperatorEquals & X );
          ROSE_TransformableBinaryOperatorEquals & operator= ( const ROSE_TransformableBinaryOperatorEquals & X );
   };



// Class Definition for ROSE_TransformableBinaryOperatorNonAssignment
class ROSE_TransformableBinaryOperatorNonAssignment : public ROSE_TransformableBinaryOperatorExpression
   {
     public:
         ~ROSE_TransformableBinaryOperatorNonAssignment ();

       // Special constructor for this class
          ROSE_TransformableBinaryOperatorNonAssignment ( ROSE_Expression* expr );

       // Make this class an abstract base class so that we can be certain that
       // we will not ever try to build one of these directly.
          virtual ROSE_TransformableNode* transform();

       // virtual ROSE_TransformableNode* transform ( ROSE_NonTransformableExpression* lhs,
       //                                             ROSE_NonTransformableExpression* rhs );


     public:
       // We want to make this a pure virtual function in the base 
       // classes and a virtual function in the leaf classes
       // virtual ROSE_Node* transform() = 0;

       // Member function defined similarly across all array grammar objects
       // this MUST be a virtual function
          virtual Boolean error();

  // We don't want to have these be used within the interface
     private:
          ROSE_TransformableBinaryOperatorNonAssignment ();
          ROSE_TransformableBinaryOperatorNonAssignment ( const ROSE_TransformableBinaryOperatorNonAssignment & X );
          ROSE_TransformableBinaryOperatorNonAssignment & operator= ( const ROSE_TransformableBinaryOperatorNonAssignment & X );
   };



#endif // ifndef TRANSFORMABLE_GRAMMAR_3_H 
