
#include "rose.h"

// DQ (2/26/2009): The copyGraph.[hC] files have been moved to the src/frontend/SageIII/astVisualization 
// directory and are not included in librose.so.
// #include "copyGraph.C"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

SgNode*
copyAST ( SgNode* node )
   {
     ROSE_ASSERT(node != NULL);

  // This is a better implementation using a derived class from SgCopyHelp to control the 
  // copying process (skipping the copy of any function definition).  This is a variable 
  // declaration with an explicitly declared class type.
     class RestrictedCopyType : public SgCopyHelp
        {
       // DQ (9/26/2005): This class demonstrates the use of the copy mechanism 
       // within Sage III (originally designed and implemented by Qing Yi).
       // One problem with it is that there is no context information permitted.

          public:
               virtual SgNode *copyAst(const SgNode *n)
                  {
                 // This is the simpliest possible version of a deep copy SgCopyHelp::copyAst() member function.
                    SgNode *returnValue = n->copy(*this);

                    return returnValue;
                  }
        } restrictedCopyType;

#if 0
  // DQ (6/14/2007): Added support for simpler function for generation of graph of whole AST.
     printf ("\n\nINITIAL Output a graph of the whole AST -- BEFORE AstPostProcessing(copyOfNode) \n");
#if 1
     SgProject* project_original = isSgProject(node);
     ROSE_ASSERT(project_original != NULL);

     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 4000;
     generateAstGraph(project_original,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
     system("mv test2005_152_WholeAST.dot test2005_152_WholeAST_before.dot");
#else
     set<SgNode*> emptyNodeSet;
     generateWholeGraphOfAST("wholeAST_before",emptyNodeSet);
#endif
#endif


#if 1
  // This triggers a bug with test2005_152.C (the unparsed code fails for g++ 4.1.2, but not 3.5.6)
     SgNode* copyOfNode = node->copy(restrictedCopyType);
#else
  // DQ (11/5/2007): Test the case of an extremely shallow copy so that we can run AstPostProcessing() twice to observe the effects.
     printf ("Running SHALLOW verstion of AST copy \n");

  // Ignore the copy as a test (so that it will exist in the memory pool) 
     SgNode* copyOfNode = node->copy(restrictedCopyType);

     copyOfNode = node;
#endif
     ROSE_ASSERT(copyOfNode != NULL);

  // std::vector<SgNode*> intersectionNodeList_early = SageInterface::astIntersection(node,copyOfNode,&restrictedCopyType);

  // DQ (10/19/2007): This might really be required inorder to pass our strict tests of the AST.
     if (SgProject::get_verbose() > 0)
          printf ("Running the AST Post Processing Phase on the new copy of the AST! \n");

     AstPostProcessing(copyOfNode);

     if (SgProject::get_verbose() > 0)
          printf ("DONE: Running the AST Post Processing Phase on the new copy of the AST! \n");

#if 0
  // DQ (6/14/2007): Added support for simpler function for generation of graph of whole AST.
     printf ("\n\nINITIAL Output a graph of the whole AST -- BEFORE AstPostProcessing(copyOfNode) \n");
#if 1
     SgProject* project_copy = isSgProject(copyOfNode);
     ROSE_ASSERT(project_copy != NULL);

     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 4000;
     generateAstGraph(project_copy,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
     system("mv test2005_152_WholeAST.dot test2005_152_WholeAST_after.dot");
#else
     set<SgNode*> emptyNodeSet_after;
     generateWholeGraphOfAST("wholeAST_after",emptyNodeSet_after);
#endif
#endif

     if (SgProject::get_verbose() > 0)
          printf ("\n\nCompare two generated ASTs ... \n");

     std::vector<SgNode*> intersectionNodeList = SageInterface::astIntersection(node,copyOfNode,&restrictedCopyType);

     if (SgProject::get_verbose() > 0)
          printf ("DONE: Compare two generated ASTs ... \n");

  // DQ (11/2/2007): Make this a stricter test!
     ROSE_ASSERT(intersectionNodeList.size() == 0);

     ROSE_ASSERT(copyOfNode != NULL);
     return copyOfNode;
   }



void
printOutTemplateDeclarations ()
   {
  // Debugging support

     class TraverseMemoryPool : public ROSE_VisitTraversal
        {
          public:
            // Required traversal function
               void visit (SgNode* node)
                  {
                    SgTemplateDeclaration* derivedDeclaration = isSgTemplateDeclaration(node);
                    if (derivedDeclaration != NULL)
                       {
                         printf ("case V_SgTemplateDeclaration: derivedDeclaration               = %p \n",derivedDeclaration);
                         printf ("case V_SgTemplateDeclaration: derivedDeclaration->get_parent() = %p = %s \n",derivedDeclaration->get_parent(),derivedDeclaration->get_parent()->class_name().c_str());
                         printf ("case V_SgTemplateDeclaration: derivedDeclaration->get_scope()  = %p = %s \n",derivedDeclaration->get_scope(),derivedDeclaration->get_scope()->class_name().c_str());
                         printf ("case V_SgTemplateDeclaration: derivedDeclaration->get_declarationModifier().isFriend() = %s \n",derivedDeclaration->get_declarationModifier().isFriend() ? "true" : "false");
                         printf ("case V_SgTemplateDeclaration: derivedDeclaration->get_name()   = %s \n",derivedDeclaration->get_name().str());
                         printf ("case V_SgTemplateDeclaration: derivedDeclaration->get_string() = %s \n",derivedDeclaration->get_string().str());
                       }
                  }

            // This avoids a warning by g++
               virtual ~TraverseMemoryPool() {};         
        };

     printf ("Friend template function declarations are not properly marked as friends \n");

  // This will traverse the whole memory pool
     TraverseMemoryPool traversal;
     traversal.traverseMemoryPool();
   }






int
main ( int argc, char* argv[] )
   {
  // Main Function for default example ROSE Preprocessor
  // This is an example of a preprocessor that can be built with ROSE
  // This example can be used to test the ROSE infrastructure

     ios::sync_with_stdio();     // Syncs C++ and C I/O subsystems!

     if (SgProject::get_verbose() > 0)
          printf ("In preprocessor.C: main() \n");

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

#if 0
  // This fails for test2005_63.C but Rich has fixed this
  // by updating the pdf library we are using within ROSE.
     printf ("Generate the pdf output of the SAGE III AST \n");
     generatePDF ( *project );
#endif

#if 0
     printf ("Generate the dot output of the SAGE III AST \n");
     generateDOT ( *project );
#endif

#if 1
  // DQ (2/6/2004): These tests fail in Coco for test2004_14.C
  // AstTests::runAllTests(const_cast<SgProject*>(project));
     AstTests::runAllTests(project);
#else
     printf ("Skipped agressive (slow) internal consistancy tests! \n");
#endif

     if (project->get_verbose() > 0)
          printf ("Calling the AST copy mechanism \n");

  // printf ("\n\nCalling the AST copy mechanism \n");

#if 1
     set<SgNode*> oldNodes;
     // if (numberOfNodes() < 2000)
        {
          oldNodes = getAllNodes();
        }
#endif

// Use this setting to control if we make a copy or not!
#if 1
  // Demonstrate the copying of the whole AST
     SgProject* newProject = static_cast<SgProject*>(copyAST(project));
     ROSE_ASSERT(newProject != NULL);
#else
     SgProject* newProject = project;
#endif

#if 1
  // DQ (6/14/2007): Added support for simpler function for generation of graph of whole AST.
     if (project->get_verbose() > 0)
          printf ("\n\nOutput a graph of the whole AST \n");
#if 1
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 4000;
     generateAstGraph(newProject,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
#else
     set<SgNode*> emptyNodeSet;
     generateWholeGraphOfAST("wholeAST",emptyNodeSet);
#endif
#endif

  // Debugging support
  // Output the declaration so that we can investigate friend template function declarations
  // (these are not properly marked as friend functions).
  // printOutTemplateDeclarations();

#if 1
  // printf ("\n\nRunning tests on the original AST \n");
     AstTests::runAllTests(project);
#if 1
  // DQ (10/19/2007): Turning this off allows for a lot of things to work great, but it is cheating :-).
     if (project->get_verbose() > 0)
          printf ("\n\nRunning tests on the copy of the AST \n");

     AstTests::runAllTests(newProject);
#else
     printf ("\n\n##################  Skipping the tests on the copy of the AST  ################## \n");
#endif
#endif

#if 0
  // DQ (11/7/2007): This is not moved to the copyAST function.
     printf ("\n\nCompare two generated ASTs ... \n");
     std::vector<SgNode*> intersectionNodeList = SageInterface::astIntersection(project,newProject);
     printf ("DONE: Compare two generated ASTs ... \n");

  // DQ (11/2/2007): Make this a stricter test!
     ROSE_ASSERT(intersectionNodeList.size() == 0);
#endif

#if 0
  // DQ (8/17/2006): This is a test that causes parent's of types to be set 
  // (and there is an assertion against this in the set_parent() member 
  // function which fails).
     ROSE_ASSERT(project != NULL);
  // list<SgNode*> declList = NodeQuery::querySubTree(project, V_SgFunctionDeclaration);
  // GB (09/26/2007)
     NodeQuerySynthesizedAttributeType declList = NodeQuery::querySubTree(project, V_SgFunctionDeclaration);
  // SgFunctionType* func_type2=isSgFunctionType(isSgFunctionDeclaration(*(declList.begin()))->get_type()->copy(treeCopy));
     SgTreeCopy treeCopy;
  // list<SgNode*>::iterator i = declList.begin();
  // GB (09/26/2007)
     NodeQuerySynthesizedAttributeType::iterator i = declList.begin();
     while ( i != declList.end() )
        {wholeAST_after.dot
          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(*i);
          ROSE_ASSERT(functionDeclaration != NULL);
          ROSE_ASSERT(functionDeclaration->get_type() != NULL);
          SgFunctionType* func_type2 = isSgFunctionType(functionDeclaration->get_type()->copy(treeCopy));
          i++;
        }
#endif

#if 0
  // DQ (10/3/2010): This is part of a mechanism to visualize the copy of the AST.
  // It generates a file that is used only for debugging.  This was work done by 
  // Jeremiah and it is very help; but not meant to be called in normal use.
     if (numberOfNodes() < 4000)
        {
       // graphNodesAfterCopy(oldNodes, "graph.dot");
          std::string filename = SageInterface::generateProjectName(project) + "_copy_graph";
          graphNodesAfterCopy(oldNodes, filename );
        }
#endif

#if 0
  // DQ (10/3/2010): This is part of a debug the AST copy mechanism (it should be off in normal testing).
     if (project->get_verbose() > 0)
          printf ("Generate the dot output of the SAGE III AST \n");
     generateDOT ( *project );
  // printf ("DONE: Generate the dot output of the SAGE III AST \n");
#endif

     if (project->get_verbose() > 0)
          printf ("Calling the backend() \n");

     int errorCode = 0;
     errorCode = backend(project);

  // DQ (7/7/2005): Only output the performance report if verbose is set (greater than zero)
     if (project->get_verbose() > 0)
        {
       // Output any saved performance data (see ROSE/src/astDiagnostics/AstPerformance.h)
          AstPerformance::generateReport();
        }

  // printf ("Exiting with errorCode = %d \n",errorCode);
     return errorCode;
   }

