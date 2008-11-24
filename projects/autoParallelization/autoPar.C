/*
 * Automatic Parallelization using OpenMP 
 *
 * Input: sequential C/C++ code
 * Output: parallelized C/C++ code using OpenMP
 *
 * Algorithm:
 *   Read in array abstraction files 
 *   Collect all loops with canonical forms
 *     x. Conduct loop normalization
 *     x. Call dependence analysis from Qing's loop transformations
 *     x. Conduct liveness analysis and variable classification
 *     x. Judge if the loop is parallelizable
 *     x. Attach OmpAttribute if it is
 *     x. Insert OpenMP pragma accordingly
 *
 * By Chunhua Liao
 * Nov 3, 2008
 */
#include "rose.h"
// all kinds of analyses needed
#include "autoParSupport.h" 
using namespace std;
using namespace AutoParallelization;
int
main (int argc, char *argv[])
{
  vector<string> argvList(argv, argv+argc);
  //Processing debugging and annotation options
  autopar_command_processing(argvList);
  SgProject *project = frontend (argvList);
  ROSE_ASSERT (project != NULL);
  //Prepare liveness analysis etc.
  initialize_analysis (project,true);   
  // For each source file in the project
    SgFilePtrList & ptr_list = project->get_fileList();
    for (SgFilePtrList::iterator iter = ptr_list.begin(); iter!=ptr_list.end();
        iter++)
   {
     SgFile* sageFile = (*iter);
     SgSourceFile * sfile = isSgSourceFile(sageFile);
     ROSE_ASSERT(sfile);
     SgGlobal *root = sfile->get_globalScope();
     SgDeclarationStatementPtrList& declList = root->get_declarations ();
     bool hasOpenMP= false; // flag to indicate if omp.h is needed

    //For each function body in the scope
     for (SgDeclarationStatementPtrList::iterator p = declList.begin(); p != declList.end(); ++p) 
     {
        SgFunctionDeclaration *func = isSgFunctionDeclaration(*p);
        if (func == 0)  continue;
        SgFunctionDefinition *defn = func->get_definition();
        if (defn == 0)  continue;
         //ignore functions in system headers, Can keep them to test robustness
        if (defn->get_file_info()->get_filename()!=sageFile->get_file_info()->get_filename())
          continue;
        SgBasicBlock *body = defn->get_body();  
        // For each loop (For-loop for now)
        Rose_STL_Container<SgNode*> loops = NodeQuery::querySubTree(defn,V_SgForStatement); 
        if (loops.size()==0) continue;
 
        // Replace operators with their equivalent counterparts defined 
        // in "inline" annotations
        AstInterfaceImpl faImpl_1(body);
        CPPAstInterface fa_body(&faImpl_1);
//        OperatorInlineRewrite()( fa_body, AstNodePtrImpl(body));
         
	 // Pass annotations to arrayInterface and use them to collect 
         // alias info. function info etc.  
         ArrayAnnotation* annot = ArrayAnnotation::get_inst(); 
         ArrayInterface array_interface(*annot);
         array_interface.initialize(fa_body, AstNodePtrImpl(defn));
         array_interface.observe(fa_body);
       
        // 1. Loop normalization for all loops within body
//        NormalizeForLoop(fa_body, AstNodePtrImpl(body));

        // 2. Compute dependence graph for the target loop
        // TODO working on more loops
        SgNode* sg_node = loops[0];
        LoopTreeDepGraph* depgraph= ComputeDependenceGraph(sg_node, &array_interface, annot);
        if (depgraph==NULL)
        {
          cout<<"Failed to compute depgraph for loop:"<<sg_node->unparseToString()<<endl;
          continue;
        }
       //3. Judge if loops are parallelizable
       bool isParallelizable = true;
       vector<DepInfo>  remainingDependences;
       DependenceElimination(sg_node, depgraph, remainingDependences);
         // Set to unparallelizable if it has dependences which can not eliminated
       if (remainingDependences.size()>0) 
       {
        isParallelizable = false;
        cout<<"\n Unparallelizable loop at line:"<<sg_node->get_file_info()->get_line()<<
             " due to the following dependencies:"<<endl;
        for (vector<DepInfo>::iterator iter= remainingDependences.begin();     
             iter != remainingDependences.end(); iter ++ )
        {
          cout<<(*iter).toString()<<endl;
        }
       }
      //comp.DetachDepGraph();// TODO release resources here
      //4.  Attach OmpAttribute to the loop node if it is parallelizable 
       if (isParallelizable)
       {  
          // TODO add variable classification to set private,shared variables 
          OmpSupport::OmpAttribute* omp_attribute = OmpSupport::buildOmpAttribute(OmpSupport::e_parallel_for,sg_node);
          OmpSupport::addOmpAttribute(omp_attribute,sg_node);
          hasOpenMP = true;
        } 
        // 5. Generate and insert #pragma omp parallel for 
        generatedOpenMPPragmas(sg_node); 
     } // end for-loop for declarations
     // insert omp.h if OpenMP directives have been inserted into the current file 
     if (hasOpenMP)
       SageInterface::insertHeader("omp.h",false,root);
   } //end for-loop of files

  // Qing's loop normalization is not robust enough to pass all tests
  //AstTests::runAllTests(project);
  
  release_analysis();
  //project->unparse();
  return backend (project);
}
