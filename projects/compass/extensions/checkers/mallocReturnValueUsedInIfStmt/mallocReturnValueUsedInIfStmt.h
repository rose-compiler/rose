// Malloc Return Value Used In If Stmt
// Author: Gary M. Yuan
// Date: 24-July-2007

#include "compass.h"

#ifndef COMPASS_MALLOC_RETURN_VALUE_USED_IN_IF_STMT_H
#define COMPASS_MALLOC_RETURN_VALUE_USED_IN_IF_STMT_H

namespace CompassAnalyses
   { 
     namespace MallocReturnValueUsedInIfStmt
        { 
          /// \brief checkerName is a std::string containing the name of this
          /// checker.
          extern const std::string checkerName;
          /// \brief shortDescription is a std::string with a short description
          /// of this checker's patte
          extern const std::string shortDescription;
          /// \brief longDescription is a std::string with a detailed
          /// description of this checker's pattern and purpose.
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          //////////////////////////////////////////////////////////////////////          /// The CheckerOutput class implements the violation output for this
          /// checker
          //////////////////////////////////////////////////////////////////////
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               std::string context;

               public:
                 /// The constructor
                 /// \param node is a SgNode*
                 /// \param w is a const char*
                 CheckerOutput(SgNode* node, const std::string & context );

                 /// getString
                 /// \returns std::string
                 std::string getString() const;
             };


          //////////////////////////////////////////////////////////////////////          /// The NestedTraversal class performs a nested traversal of the AST 
          /// seeking out If-statements in the basic containing block of the 
          /// call to malloc.
          //////////////////////////////////////////////////////////////////////
          class NestedTraversal : public AstSimpleProcessing
          {
            std::string lhs;
            bool *foundIf;

            public:
              /// The constructor
              /// \param b is a bool*
              /// \param s is a std::string
              NestedTraversal( std::string s, bool *b ) : 
                lhs( s ), foundIf( b ) {}

              /// visit function
              /// \param n is a SgNode*
              virtual void visit( SgNode *n )
              {
                SgIfStmt *ifstmt = isSgIfStmt(n);

                if( ifstmt != NULL )
                {
                  std::string nodeString = n->unparseToString();

                  if( nodeString.find( lhs ) != std::string::npos )
                    *foundIf = true;
                } //if( ifstmt != NULL )
              } //visit( SgNode *n )
          }; // class NestedTraversal

       // Specification of Checker Traversal Implementation
          //////////////////////////////////////////////////////////////////////
          /// The Traversal class performs a simple AST traversal seeking out
          /// SgFunctionRefExp corresponding to malloc.
          //////////////////////////////////////////////////////////////////////
          class Traversal
             : public AstSimpleProcessing, public Compass::TraversalBase
             {
            // Checker specific parameters should be allocated here.
               enum visitModes{ RETURN=-1, INIT, ASSIGN };

               public:
                    /// The constructor
                    /// \param out is a Compass::OutputObject*
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    /// run function
                    /// \param n is a SgNode*
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    /// reverseVisit function
                    /// \param p is a SgNode*
                    /// \param expression is a std::string
                    /// \param mode is an int
                    bool reverseVisit( SgNode *p, std::string expression, 
                                       int mode );
                    /// elErase function 
                    /// s is a std::string
                    /// c is a char
                    void elErase( std::string & s, char c );
                    /// visit function
                    /// \param n is a SgNode
                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_MALLOC_RETURN_VALUE_USED_IN_IF_STMT_H
#endif 

