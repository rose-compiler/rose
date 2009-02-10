// Malloc Return Value Used In If Stmt
// Author: Gary M. Yuan
// Date: 24-July-2007

#include "rose.h"
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
             : public Compass::AstSimpleProcessingWithRunFunction
             {
            // Checker specific parameters should be allocated here.
               Compass::OutputObject* output;
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

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Malloc Return Value Used In If Stmt Analysis
// Author: Gary M. Yuan
// Date: 24-July-2007

#include "compass.h"
// #include "mallocReturnValueUsedInIfStmt.h"

namespace CompassAnalyses
   { 
     namespace MallocReturnValueUsedInIfStmt
        { 
          const std::string checkerName      = "MallocReturnValueUsedInIfStmt";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks if the return value of calling malloc is part of an If-Statement conditional test.";
          const std::string longDescription  = "Memory dynamically allocated with malloc should be part of an If-statement conditional expression before using the allocated block of memory. Ideally, the If-statement conditional appears directly after the call to malloc but may simply occur in the same scope. These simple conditional checks help prevent many segmentation fault errors.";
        } //End of namespace MallocReturnValueUsedInIfStmt.
   } //End of namespace CompassAnalyses.

CompassAnalyses::MallocReturnValueUsedInIfStmt::
CheckerOutput::CheckerOutput ( SgNode* node, const std::string & s )
   : OutputViolationBase(node,checkerName,shortDescription), context(s)
   {}

std::string
CompassAnalyses::MallocReturnValueUsedInIfStmt::
CheckerOutput::getString() const
{
     ROSE_ASSERT(getNodeArray().size() <= 1);

  // Default implementation for getString
     SgLocatedNode* locatedNode = isSgLocatedNode(getNode());
     std::string sourceCodeLocation;
     if (locatedNode != NULL)
        {
          Sg_File_Info* start = locatedNode->get_startOfConstruct();
          Sg_File_Info* end   = locatedNode->get_endOfConstruct();
          sourceCodeLocation = (end ? Compass::formatStandardSourcePosition(start, end) 
                                    : Compass::formatStandardSourcePosition(start));
       }
      else
       {
      // Else this could be a SgInitializedName or SgTemplateArgument (not yet moved to be a SgLocatedNode)
         Sg_File_Info* start = getNode()->get_file_info();
         ROSE_ASSERT(start != NULL);
         sourceCodeLocation = Compass::formatStandardSourcePosition(start);
       }

     std::string nodeName = getNode()->class_name();

  // The short description used here needs to be put into a separate function (can this be part of what is filled in by the script?)
  // return loc + ": " + nodeName + ": variable requiring static constructor initialization";

     return m_checkerName + ": " + sourceCodeLocation + ": " + nodeName + ": " + m_shortDescription + "\ncall to malloc does not have a corresponding " + "If-statement conditional in this block {}. " + context;
} //CompassAnalyses::MallocReturnValueUsedInIfStmt::CheckerOutput::getString()

CompassAnalyses::MallocReturnValueUsedInIfStmt::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["MallocReturnValueUsedInIfStmt.YourParameter"]);


   }

void
CompassAnalyses::MallocReturnValueUsedInIfStmt::Traversal::
elErase( std::string & s, char c )
{
  size_t position = s.find(c);

  while( position != std::string::npos )
  {
    s.erase( position, 1 );
    position = s.find(c);
  } //while

  return;
} //CompassAnalyses::MallocReturnValueUsedInIfStmt::Traversal:: 
  //elErase( std::string & s, char c )

bool
CompassAnalyses::MallocReturnValueUsedInIfStmt::Traversal::
reverseVisit( SgNode *p, std::string expression, int mode )
{
  bool foundIf = false;
  std::string lhs = expression;

  if( mode > RETURN )
  {
    elErase( expression, '*' );
    elErase( expression, '&' );

    lhs.assign( expression, 0, expression.find( " =" ) );

    if( mode == INIT )
    {
      lhs.assign( lhs,
        lhs.find_last_of( ' ' ) + 1, lhs.length() - lhs.find_last_of( ' ' ) );
    } //if( mode == INIT )
  } //if( mode > RETURN )

  while( p != NULL )
  {
    if( isSgBasicBlock(p) )
    {
      NestedTraversal
      nest( lhs, &foundIf );

      nest.traverse( p, postorder );
      break;
    } //if( isSgBasicBlock(p) )

    p = p->get_parent();
  } //while( p != NULL )

  return foundIf;
} //CompassAnalyses::MallocReturnValueUsedInIfStmt::Traversal::
  //reverseVisit( SgNode *p, std::string expression, int mode )

void
CompassAnalyses::MallocReturnValueUsedInIfStmt::Traversal::
visit(SgNode* node)
   { 
     bool foundIf = false;

     std::string context;
     SgFunctionRefExp *f = isSgFunctionRefExp(node);

     if( f != NULL )
     {
       SgFunctionSymbol *sym = f->get_symbol();

       if( sym->get_name().getString() == "malloc" )
       {
         for( SgNode *parent = node->get_parent(); parent != NULL;
              parent = parent->get_parent() )
         {
           if( isSgAssignInitializer(parent) )
           {
             std::string expression = 
               parent->get_parent()->get_parent()->unparseToString();

             foundIf = reverseVisit( parent, expression, INIT );
             break;
           } //if( isSgAssignInitializer(parent) )

           if( isSgAssignOp(parent) )
           {
             std::string expression = parent->unparseToString();

             foundIf = reverseVisit( parent, expression, ASSIGN );
             break;
           } //if( isSgAssignOp(parent) )

           if( isSgReturnStmt(parent) )
           {
             context.assign( "Calls to malloc used in return statements should be checked with an If-statement conditional before the function returns" );
             foundIf = false;
             break;
           } //if( isSgReturnStmt(parent) )
         } //for 
       } //if( sym->get_name.getString() == "malloc" )
       else
         foundIf = true;

       if( !foundIf && isSgLocatedNode(node)
            && node->unparseToString() != "free" )
       {
         output->addOutput( new CheckerOutput( node, context ) );
       } //if( !foundIf... )

     } //if( f != NULL )
     return;
   } //End of the visit function.

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::MallocReturnValueUsedInIfStmt::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::MallocReturnValueUsedInIfStmt::Traversal(params, output);
}

extern const Compass::Checker* const mallocReturnValueUsedInIfStmtChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::MallocReturnValueUsedInIfStmt::checkerName,
        CompassAnalyses::MallocReturnValueUsedInIfStmt::shortDescription,
        CompassAnalyses::MallocReturnValueUsedInIfStmt::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
