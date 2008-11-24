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
  initialize_analysis (project,false);   
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
        OperatorInlineRewrite()( fa_body, AstNodePtrImpl(body));
         
	 // Pass annotations to arrayInterface and use them to collect 
         // alias info. function info etc.  
         ArrayAnnotation* annot = ArrayAnnotation::get_inst(); 
         ArrayInterface array_interface(*annot);
         array_interface.initialize(fa_body, AstNodePtrImpl(defn));
         array_interface.observe(fa_body);
       
        // 1. Loop normalization for all loops within body
        NormalizeForLoop(fa_body, AstNodePtrImpl(body));

        // 2. Compute dependence graph for the target loop
        // TODO working on more loops
        SgNode* sg_node = loops[0];
        LoopTreeDepGraph* depgraph= ComputeDependenceGraph(sg_node, &array_interface, annot);
        if (depgraph==NULL)
        {
          cout<<"Skipping a loop since failed to compute depgraph for it:"<<sg_node->unparseToString()<<endl;
          continue;
        }
	// 3. variable classification (autoscoping): 
	// This step is done before DependenceElimination(), so the irrelevant
	// dependencies associated with the autoscoped variabled can be
	// eliminated.
        OmpSupport::OmpAttribute* omp_attribute = new
	  OmpSupport::OmpAttribute();
	ROSE_ASSERT(omp_attribute != NULL);
	AutoScoping(sg_node, omp_attribute);

       //4. Judge if loops are parallelizable
       bool isParallelizable = true;
       vector<DepInfo>  remainingDependences;
       DependenceElimination(sg_node, depgraph, remainingDependences,omp_attribute,&array_interface, annot);
       // Set to unparallelizable if it has dependences 
       //which can not be eliminated
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
      //5.  Attach OmpAttribute to the loop node if it is parallelizable 
       if (isParallelizable)
       {  
         //= OmpSupport::buildOmpAttribute(OmpSupport::e_parallel_for,sg_node);
	  omp_attribute->setOmpDirectiveType(OmpSupport::e_parallel_for);
          OmpSupport::addOmpAttribute(omp_attribute,sg_node);
          hasOpenMP = true;
          // 6. Generate and insert #pragma omp parallel for 
          generatedOpenMPPragmas(sg_node); 
        } 
       else
       {
	 delete omp_attribute;
       }
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
