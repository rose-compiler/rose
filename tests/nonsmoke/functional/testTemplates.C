// Example ROSE Translator used for testing ROSE infrastructure
#include "rose.h"

#include "RoseAst.h"

#if 0
bool isTemplateInstantiationNode(SgNode* node) 
   {
     return isSgTemplateInstantiationDecl(node)
         || isSgTemplateInstantiationDefn(node)
         || isSgTemplateInstantiationFunctionDecl(node)
         || isSgTemplateInstantiationMemberFunctionDecl(node)
         || isSgTemplateInstantiationTypedefDeclaration(node)
         || isSgTemplateInstantiationDirectiveStatement(node)
         ;
   }
#endif

void markNodeToBeUnparsed(SgNode* node) 
   {
     Sg_File_Info* fileInfo=node->get_file_info();
     if (fileInfo != NULL) 
        {
          fileInfo->setTransformation();
          fileInfo->setOutputInCodeGeneration();

          SgLocatedNode* locatedNode = isSgLocatedNode(node);
          if (locatedNode != NULL)
             {
            // DQ (7/7/2015): Make the subtree as transformed.
               locatedNode->setTransformation();
               locatedNode->setOutputInCodeGeneration();
#if 0
               printf ("Note: calling node markTransformationsForOutput(): node = %p = %s \n",node,node->class_name().c_str());
#endif
               markTransformationsForOutput(node);
             }
            else
             {
#if 0
               printf ("Note: node is not a SgLocatedNode: node = %p = %s \n",node,node->class_name().c_str());
#endif
             }
        }
       else
        {
#if 0
          printf ("Note: no Sg_File_Info was found: node = %p = %s \n",node,node->class_name().c_str());
#endif
        }
   }

int markAllTemplateInstantiationsToBeUnparsed(SgProject* root) 
   {
     RoseAst ast(root);
     int n = 0;
     for (RoseAst::iterator i=ast.begin();i!=ast.end();++i) 
        {
#if 0
       // DQ (5/9/2017): Debugging code.
          if (isSgGlobal((*i)->get_parent()) != NULL)
             {
               printf ("In global scope: *i = %p = %s \n",*i,(*i)->class_name().c_str());
             }
#endif
       // if (isTemplateInstantiationNode(*i)) 
          if (SageInterface::isTemplateInstantiationNode(*i)) 
             {
#if 0
               printf ("Calling markNodeToBeUnparsed(): *i = %p = %s \n",*i,(*i)->class_name().c_str());
#endif
               markNodeToBeUnparsed(*i);
               n++;
             }
       }

     return n;
   }

#if 0
// Moved to the SageInterface as general support for fixup of instantiated templates.
int wrapAllTemplateInstantiationsInAssociatedNamespaces(SgProject* root) 
   {
  // DQ (7/19/2015): This function can't use an iterator since it will be 
  // doing transformations on the AST and will cause iterator invalidation errors.

     std::vector<SgStatement*> templateInstantiationVector;

     RoseAst ast(root);

     int n = 0;
     for (RoseAst::iterator i= ast.begin(); i!=ast.end(); ++i) 
        {
          if (isTemplateInstantiationNode(*i)) 
             {
               markNodeToBeUnparsed(*i);
               n++;
             }
       }

     return n;
   }
#endif


int main( int argc, char * argv[] )
   {
#if 0
  // Output the ROSE specific predefined macros.
     outputPredefinedMacros();
#endif

  // Generate the ROSE AST.
     SgProject* project = frontend(argc,argv);

  // AST consistency tests (optional for users, but this enforces more of our tests)
     AstTests::runAllTests(project);

     markAllTemplateInstantiationsToBeUnparsed(project);

  // DQ (9/17/2015): Call fixup function for template instantiations so that they can be unparsed with the GNU g++ backend compiler.
     SageInterface::wrapAllTemplateInstantiationsInAssociatedNamespaces(project);

#if 0
  // DQ (5/11/2017): Adding support for detection of template specializations that will be output and building there forward 
  // declarations as required. Note that EDG does not always build the forward declarations that are required for the output
  // of template specializations (see test2017_22.C and test2017_23.C for examples where EDG does and does not build the 
  // forward declarations.
     markTemplateInstantiationsForOutput(project);
#endif

#if 0
  // Output an optional graph of the AST (just the tree, when active)
     printf ("Generating a dot file... (ROSE Release Note: turn off output of dot files before committing code) \n");
     generateDOT ( *project );
     // generateAstGraph(project, 2000);
#endif

#if 0
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 8000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
#endif

#if 0
     SgNode::get_globalTypeTable()->print_typetable();
#endif

  // regenerate the source code and call the vendor 
  // compiler, only backend error code is reported.
     return backend(project);
   }
