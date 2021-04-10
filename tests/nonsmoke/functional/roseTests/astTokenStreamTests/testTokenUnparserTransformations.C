
// This test code tests the unparsing using the token stream using any
// input code and using transformations of variables with a specific suffix.

#include "rose.h"

#include <string>

using namespace std;

class TransformVisitor : public AstSimpleProcessing 
   {
     private:
          static const string matchEnding;
          static const size_t matchEndingSize;
          static const string renameEnding;

     protected:
          void visit(SgNode* astNode);
   };

const string TransformVisitor::matchEnding = "_rename_me";
const size_t TransformVisitor::matchEndingSize = matchEnding.size();
const string TransformVisitor::renameEnding = "_renamed";


void TransformVisitor::visit(SgNode* node)
   {
#if 0
     printf ("In TransformVisitor::visit(): node = %p = %s \n",node,node->class_name().c_str());
#endif

  // Use a pointer to a constant SgVariableDeclaration to be able to call the constant getter variableDeclaration -> get_variables(), 
  // which does not mark the node as modified.
     const SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(node);
     if (variableDeclaration != NULL)
        {
          const SgInitializedNamePtrList& nameList = variableDeclaration -> get_variables();
          for (SgInitializedNamePtrList::const_iterator nameListIterator = nameList.begin(); nameListIterator != nameList.end(); nameListIterator++)
             {
               string originalName = ((*nameListIterator) -> get_name()).getString();

#if 0
               printf ("variable: originalName = %s \n",originalName.c_str());
#endif

            // Rename any variable, whose name ends with matchEnding.
               if (originalName.size() >= matchEndingSize && originalName.compare(originalName.size() - matchEndingSize, matchEndingSize, matchEnding) == 0)
                  {
                    string new_name = originalName + renameEnding;

                 // SageInterface::set_name(*nameListIterator, originalName + renameEnding);
                    SageInterface::set_name(*nameListIterator, new_name);
#if 1
                    printf ("variable: new_name = %s \n",new_name.c_str());
#endif
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
             }
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
        }

#if 1
  // DQ (2/4/2021): Adding support to rename enum values in SgEnumDeclaration.
     const SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(node);
     if (enumDeclaration != NULL)
        {
          const SgInitializedNamePtrList& enumerators = enumDeclaration -> get_enumerators();
          for (SgInitializedNamePtrList::const_iterator nameListIterator = enumerators.begin(); nameListIterator != enumerators.end(); nameListIterator++)
             {
               string originalName = ((*nameListIterator) -> get_name()).getString();
#if 0
               printf ("enumerator: originalName = %s \n",originalName.c_str());
#endif
            // Rename any variable, whose name ends with matchEnding.
               if (originalName.size() >= matchEndingSize && originalName.compare(originalName.size() - matchEndingSize, matchEndingSize, matchEnding) == 0)
                  {
                    string new_name = originalName + renameEnding;

                 // SageInterface::set_name(*nameListIterator, originalName + renameEnding);
                    SageInterface::set_name(*nameListIterator, new_name);
#if 1
                    printf ("enumerator: new_name = %s \n",new_name.c_str());
#endif
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
             }
        }
#endif



  // DQ (2/4/2021): Adding support to rename enum values in SgEnumDeclaration.
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(node);
     if (functionDeclaration != NULL)
        {
       // SgFunctionDeclaration* SageInterface::replaceDefiningFunctionDeclarationWithFunctionPrototype ( SgFunctionDeclaration* functionDeclaration )

          string originalName = functionDeclaration->get_name();

          string matchEnding = "_make_prototype";
          size_t matchEndingSize = matchEnding.size();
#if 0
          printf ("functionDeclaration: originalName = %s \n",originalName.c_str());
#endif
       // Rename any variable, whose name ends with matchEnding.
          if (originalName.size() >= matchEndingSize && originalName.compare(originalName.size() - matchEndingSize, matchEndingSize, matchEnding) == 0)
             {
               printf ("Calling SageInterface::replaceDefiningFunctionDeclarationWithFunctionPrototype(): functionDeclaration = %p \n",functionDeclaration);

               SgFunctionDeclaration* functionPrototype = SageInterface::replaceDefiningFunctionDeclarationWithFunctionPrototype(functionDeclaration);

               printf ("Done: calling SageInterface::replaceDefiningFunctionDeclarationWithFunctionPrototype(): functionDeclaration = %p functionPrototype = %p \n",functionDeclaration,functionPrototype);
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

  // DQ (9/20/2018): If we are using the token based unparsing, then any change to the SgInitializedName 
  // must also touch the associated variable reference expressions.  I don't think there is a good way to 
  // automate this except to put this support into the SageInterface::set_name() function (which we could 
  // do later).

  // If we find a variable in a SgExprStatement then we want to be able to outline it.
     SgVarRefExp* varRefExp = isSgVarRefExp(node);
     if (varRefExp != NULL)
        {
          SgVariableSymbol* variableSymbol = varRefExp->get_symbol();
          ROSE_ASSERT(variableSymbol != NULL);
          string originalName = variableSymbol->get_name().str();
#if 0
          printf ("varRefExp: originalName = %s \n",originalName.c_str());
#endif
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
   }



int main(int argc, char* argv[])
   {
     ROSE_ASSERT(argc > 1);

     SgProject* project = frontend(argc,argv);

  // AstTests::runAllTests(project);

#if 1
     TransformVisitor transformation;
     transformation.traverse(project, preorder);
#endif

#if 0
  // Output an optional graph of the AST (just the tree, when active)
     printf ("Generating a dot file... (ROSE Release Note: turn off output of dot files before committing code) \n");
  // generateDOT ( *project );
     generateDOT_withIncludes ( *project );
#endif

#if 0
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 8000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

     return backend(project);
   }


