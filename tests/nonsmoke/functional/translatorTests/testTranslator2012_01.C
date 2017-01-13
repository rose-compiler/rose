// This program demonstrates reading two files, and putting
// the contents of the first file into the 2nd file (above the "main" 
// function of the second file, if it is present).  Numerous
// variations of this form of approach would be equivalent.
// Also, numerous variations of this program could allow
// for different kinds of behavior.  This is just a simple 
// test program to demonstrate the technique.
// The prograsm also show how to output a graph of the AST
// that is useful for debugging.

// Importantly, this simple version of the program does not
// move the symbols from the 1st file to the 2nd file.  This
// would be adviable as an additional modification to make
// the process more elegant, but the output would be the same.

#include "rose.h"

using namespace std;

void markAsTransformation (SgStatement* statement )
   {
  // Mark a subtree at "statement" as being a transformation (and to be 
  // output, so it will be output in the generated code).

  // AST treversal to make the subtree.
     class MarkAsTransformationTraversal : public AstSimpleProcessing
        {
          public:
               void visit ( SgNode* astNode )
                  {
                    SgLocatedNode* locatedNode = isSgLocatedNode(astNode);
                    if (locatedNode != NULL)
                       {
                         locatedNode->get_file_info()->setTransformation();
                         locatedNode->get_file_info()->setOutputInCodeGeneration();

                      // Uncomment to see the source position information for each SgLocatedNode IR node.
                      // locatedNode->get_file_info()->display("markAsTransformation(): debug");
                       }
                  }
        };

     MarkAsTransformationTraversal traversal;
     traversal.traverse(statement,preorder);
   }



// Inherited attribute (see ROSE Tutorial (Chapter 9)).
class InheritedAttribute
   {
     public:
          static bool isFirstFile;
          static std::vector<SgDeclarationStatement*> statements_from_first_file;

          InheritedAttribute();
          InheritedAttribute( const InheritedAttribute & X );
   };

// Declarations of static data members.
bool InheritedAttribute::isFirstFile = false;
std::vector<SgDeclarationStatement*> InheritedAttribute::statements_from_first_file;

// Constructor (not really needed)
InheritedAttribute::InheritedAttribute()
   {
   }

// Copy constructor (not really needed)
InheritedAttribute::InheritedAttribute( const InheritedAttribute & X )
   {
   }

// Synthesized attribute (see ROSE Tutorial (Chapter 9)).
class SynthesizedAttribute
   {
     public:
          SgFunctionDeclaration* main_function;

          SynthesizedAttribute();
          SynthesizedAttribute( const SynthesizedAttribute & X );
   };

// Constructor
SynthesizedAttribute::SynthesizedAttribute()
   {
     main_function = NULL;
   }

// Copy constructor
SynthesizedAttribute::SynthesizedAttribute( const SynthesizedAttribute & X )
   {
     main_function = X.main_function;
   }


class Traversal : public SgTopDownBottomUpProcessing<InheritedAttribute,SynthesizedAttribute>
   {
     public:
       // Functions required
          InheritedAttribute   evaluateInheritedAttribute   ( SgNode* astNode, InheritedAttribute inheritedAttribute );
          SynthesizedAttribute evaluateSynthesizedAttribute ( SgNode* astNode, InheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList );
   };


InheritedAttribute
Traversal::evaluateInheritedAttribute ( SgNode* astNode, InheritedAttribute inheritedAttribute )
   {
  // printf ("evaluateInheritedAttribute(): astNode = %p = %s inheritedAttribute.isFirstFile = %s \n",astNode,astNode->class_name().c_str(),inheritedAttribute.isFirstFile ? "true" : "false");

  // This assumes that we only visit 2 files.
     SgSourceFile* file = isSgSourceFile(astNode);
     if (file != NULL)
        {
       // Only modify the static boolean value from the SgSourceFile position in the tree.

          printf ("Found an SgSourceFile: file = %p \n",file);
          if (inheritedAttribute.isFirstFile == false)
               inheritedAttribute.isFirstFile = true;
            else
               inheritedAttribute.isFirstFile = false;

          printf ("evaluateInheritedAttribute(): inheritedAttribute.isFirstFile = %s \n",inheritedAttribute.isFirstFile ? "true" : "false");
        }

     return inheritedAttribute;
   }


SynthesizedAttribute
Traversal::evaluateSynthesizedAttribute ( SgNode* astNode, InheritedAttribute inheritedAttribute, SynthesizedAttributesList childAttributes )
   {
     SynthesizedAttribute localResult;

  // printf ("evaluateSynthesizedAttribute(): astNode = %p = %s inheritedAttribute.isFirstFile = %s \n",astNode,astNode->class_name().c_str(),inheritedAttribute.isFirstFile ? "true" : "false");

  // Accumulate any valid pointer to main on a child node and pass it to the local synthesized attribute.
     for (size_t i = 0; i < childAttributes.size(); i++)
        {
          if (childAttributes[i].main_function != NULL)
             {
               localResult.main_function = childAttributes[i].main_function;
             }
        }

     if (inheritedAttribute.isFirstFile == true)
        {
          SgGlobal* globalScope = isSgGlobal(astNode);
          if (globalScope != NULL)
             {
            // Gather all of the functions in global scope of the first file.

               vector<SgDeclarationStatement*> globalScopeDeclarationsToMove = globalScope->get_declarations();
               inheritedAttribute.statements_from_first_file = globalScopeDeclarationsToMove;

            // printf ("evaluateSynthesizedAttribute(): Gather all of the functions in global scope of the first file inheritedAttribute.statements_from_first_file.size() = %zu \n",inheritedAttribute.statements_from_first_file.size());

            // Erase the declarations in the global scope of the first file.
               globalScope->get_declarations().clear();
             }

          SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(astNode);
          if (declarationStatement != NULL && isSgGlobal(declarationStatement->get_parent()) != NULL)
             {
            // Mark as a transformation (recursively mark the whole subtree).
            // printf ("*** Mark as a transformation: declarationStatement = %p \n",declarationStatement);
               markAsTransformation(declarationStatement);
               if (declarationStatement->get_firstNondefiningDeclaration() != NULL)
                    markAsTransformation(declarationStatement->get_firstNondefiningDeclaration());
             }
        }
       else
        {
          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(astNode);
          if (functionDeclaration != NULL && functionDeclaration->get_name() == "main")
             {
            // Save the pointer to the main function (in the second file).
               localResult.main_function = functionDeclaration;
            // printf ("Found the main function ...(saved pointer) inheritedAttribute.main_function = %p \n",localResult.main_function);
             }

       // printf ("evaluateSynthesizedAttribute(): localResult.main_function = %p \n",localResult.main_function);

       // Test for the selected insertion point in the 2nd file for the declarations gathered from the first file.
          SgGlobal* globalScope = isSgGlobal(astNode);
          if (globalScope != NULL && localResult.main_function != NULL)
             {
               printf ("evaluateSynthesizedAttribute(): Found the main function ...\n");
               vector<SgDeclarationStatement*>::iterator i = find(globalScope->get_declarations().begin(),globalScope->get_declarations().end(),localResult.main_function);
               globalScope->get_declarations().insert(i,inheritedAttribute.statements_from_first_file.begin(),inheritedAttribute.statements_from_first_file.end());
#if 0
            // Set the parents of each declaration to match the new location (avoids warning that might later be an error).
               for (size_t i = 0; i < inheritedAttribute.statements_from_first_file.size(); i++)
                  {
                    inheritedAttribute.statements_from_first_file[i]->set_parent(globalScope);
                  }
#endif
             }
        }

     return localResult;
   }


int
main ( int argc, char* argv[] )
   {
  // Build the abstract syntax tree
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

  // Build the inherited attribute
     InheritedAttribute inheritedAttribute;

  // Define the traversal
     Traversal myTraversal;

  // Call the traversal starting at the project (root) node of the AST
     myTraversal.traverseInputFiles(project,inheritedAttribute);

  // Demonstrate the the transformation will pass the AST tests.
     AstTests::runAllTests (project);

  // Output an optional graph of the AST (just the tree, when active)
     generateDOT ( *project );

  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 10000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"");

     return backend (project);	// only backend error code is reported
   }


