#include <rose.h>

#include "transformation_2.h"

// Forward declaration
class StatementDataBase;

// We have to include this after the definition if ArrayAssignmentUsingTransformationGrammar
// because it is a base class of the ROSE_TransformableNode object!
#include "grammar_3.h"

#if 0
#if 0
extern struct 
   { 
     ROSE_GrammarVariants variant; 
     char *name;                   
   } arrayGrammarTerminalNames[134];
#else
extern struct 
   { 
     ROSE_GrammarVariants variant; 
     char *name;
   } arrayGrammarTerminalNames[143];
#endif
#endif

// Forward statements
class ROSE_TransformableStatement;
class ROSE_TransformableExpression;

#define MAX_NUMBER_OF_OPERANDS   256


class OperandDataBase
   {
  // This object implements a data base that is ment to be specific to a given transformable statement
     public:
          int numberOfDimensions;
          SgName variableName;
          ROSE_TransformableOperandExpression* transformableOperand;

     public:
         ~OperandDataBase ();
          OperandDataBase ( ROSE_TransformableOperandExpression* transformableStatement );

          static SgName transformationVariableSuffix();

	  SgName getVariableName() { return variableName; }
          SgName getVariableNamePointer();
	  int getStatementDimension();

     private:
       // We don't implement these, but listing them here prevents the compiler from implementing them for us
          OperandDataBase ();
          OperandDataBase ( const OperandDataBase & X );
          OperandDataBase & operator= ( const OperandDataBase & X );
   };

class StatementDataBase
   {
  // This object implements a data base that is ment to be specific to a given transformable statement
     public:
         ROSE_TransformableStatement* dataBaseTransformableStatement;
         int numberOfRhsOperands;
         OperandDataBase* lhsOperand;
         OperandDataBase* rhsOperand[MAX_NUMBER_OF_OPERANDS];

         int statementDimension;

     public:
         ~StatementDataBase ();
          StatementDataBase ( ROSE_TransformableStatement* transformableStatement );

          List<SgName> *computeUniqueOperands();
          void display( const char* label = "StatementDataBase: default label" );

       // void traverse (StatementDataBase* localStatementDataBase, ROSE_TransformableStatement* transformableStatement );
          static void traverse ( ROSE_TransformableStatement* transformableStatement );
          static void traverse ( StatementDataBase* localStatementDataBase, ROSE_TransformableExpression* transformableExpression );

          int getNumberOfRhsOperands();
	  void setStatementDimension();

     private:
       // We don't implement these, but listing them here prevents the compiler from implementing them for us
          StatementDataBase ();
          StatementDataBase ( const StatementDataBase & X );
          StatementDataBase & operator= ( const StatementDataBase & X );
   };


// class ArrayAssignmentUsingTransformationGrammar : public ROSE_TransformationBase

// Experiment with using the Transform_2 as a base class for this transformation!
class ArrayAssignmentUsingTransformationGrammar : public ArrayAssignmentUsingGrammar
   {
  // This class implements the transformation associated with an array assignment
  // it uses the array grammar defined in Transform_2/*.[h,C] and and builds on this grammar
  // with a grammar specific to the array assignment optimization.

     public:
         ~ArrayAssignmentUsingTransformationGrammar();
       // ArrayAssignmentUsingTransformationGrammar();
          ArrayAssignmentUsingTransformationGrammar( SgFile *file );

          ROSE_TransformableStatementBlock* pass( ROSE_StatementBlock* inputRoseProgramTree );
          ROSE_TransformableStatementBlock* traverseProgramTree ( ROSE_StatementBlock* roseProgramTree );

          ROSE_TransformableGrammarVariants getVariant ( ROSE_Node* node );

          ROSE_TransformableStatement* parse ( ROSE_Statement* roseStatement );

      // This is where the transformation is done
          void transform( ROSE_TransformableStatementBlock* roseProgramTree );

       // What should be the return type???
          ROSE_TransformableExpression* parse ( ROSE_Expression* roseExpression );

       // Data base mechanism!
       // static StatementDataBase* setUpDataBase( ROSE_TransformableStatementBlock* transformableStatementBlock );
          static void setUpDataBase( ROSE_TransformableStatementBlock* roseProgramTree );

       // void editLhsSpecificSubTransformation       ( SgStatement *inputStatement, char* pointerName, char* arrayName );
       // void editRhsSpecificSubTransformation       ( SgStatement *inputStatement, char* pointerName, char* arrayName );
       // void editOperandSpecificSubTransformation   ( SgStatement *inputStatement, char* pointerName, char* arrayName );

     private:
       // We don't implement these, but listing them here prevents the compiler from implementing them for us
          ArrayAssignmentUsingTransformationGrammar ( const ArrayAssignmentUsingTransformationGrammar & X );
          ArrayAssignmentUsingTransformationGrammar & operator= ( const ArrayAssignmentUsingTransformationGrammar & X );
   };

extern ArrayAssignmentUsingTransformationGrammar* globalArrayAssignmentUsingTransformationGrammar;


