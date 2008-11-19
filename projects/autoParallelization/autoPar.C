/*
 * Automatic Parallelization using OpenMP 
 *
 * Input: sequential C/C++ code
 * Output: parallelized C/C++ code using OpenMP
 *
 * Algorithm:
 *   Read in array abstraction files 
 *   for all loops
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
#include "OmpAttribute.h"
#include "CommandOptions.h"
#include <vector>
#include <string>
#include <iostream>

//Array Annotation headers
#include <CPPAstInterface.h>
#include <ArrayAnnot.h>
#include <ArrayRewrite.h>

//dependence graph headers
#include <AstInterface_ROSE.h>
#include <LoopTransformInterface.h>
#include <AnnotCollect.h>
#include <OperatorAnnotation.h>
//#include <DepCompTransform.h>
#include <LoopTreeDepComp.h>

using namespace std;
using namespace CommandlineProcessing;
// new attribute support for 3.0 specification
using namespace OmpSupport;
using namespace SageInterface;

int
main (int argc, char *argv[])
{
  vector<string> argvList(argv, argv+argc);

  //Save -debugdep, -annot file .. etc, 
  // used internally in ReadAnnotation and Loop transformation
  CmdOptions::GetInstance()->SetOptions(argvList);
  bool dumpAnnot = isOption(argvList,"","-dumpannot",true);

  //Read in annotations---------------------------- 
  ArrayAnnotation* annot = ArrayAnnotation::get_inst();
  annot->register_annot();
  ReadAnnotation::get_inst()->read();
  if (dumpAnnot)  
    annot->Dump();

  // Strip off custom options and their values to enable backend compiler 
  removeArgsWithParameters(argvList,"-annot");

  /* We use ArrayAnnotation now
  OperatorSideEffectAnnotation *funcInfo =  OperatorSideEffectAnnotation::get_inst();
  funcInfo->register_annot();
  ReadAnnotation::get_inst()->read();
  AssumeNoAlias aliasInfo;
  */

  SgProject *project = frontend (argvList);
  ROSE_ASSERT (project != NULL);
  
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
    //TODO ignore functions in system headers, keep them now for testing robustness
     for (SgDeclarationStatementPtrList::iterator p = declList.begin(); p != declList.end(); ++p) 
     {
        SgFunctionDeclaration *func = isSgFunctionDeclaration(*p);
        if (func == 0)  continue;
        SgFunctionDefinition *defn = func->get_definition();
        if (defn == 0)  continue;
        SgBasicBlock *body = defn->get_body();  

        // Replace operators with their equivalent counterparts defined 
        // in "inline" annotations
        AstInterfaceImpl scope(body);
        CPPAstInterface fa_body(&scope);
        OperatorInlineRewrite()( fa_body, AstNodePtrImpl(body));
         
	 // Pass annotations to arrayInterface and use them to collect alias info. function info etc.  
         ArrayInterface array_interface(*annot);
         array_interface.initialize(fa_body, AstNodePtrImpl(defn));
         array_interface.observe(fa_body);

        // For each loop (For-loop for now)
        Rose_STL_Container<SgNode*> loops = NodeQuery::querySubTree(defn,V_SgForStatement); 
        if (loops.size()==0) continue;
        
  // Invoke loop transformation, dependence analysis only
   
        // 1. Loop normalization changes the original code, 
        // may not be desired for s2s translation
        // Loop normalization, starting from the parent BB  is fine
        // Bridge pattern:  interface -- implementation grow independently
        // Implementation side: attach to AST function body
        AstInterfaceImpl faImpl_0 = AstInterfaceImpl(body);
        // Build AST abstraction side, a reference to the implementation 
        AstInterface fa_0(&faImpl_0);
        NormalizeForLoop(fa_0, AstNodePtrImpl(body));
        
        //TODO working on first loop, how to get all first level loops?
        // Prepare AstInterface: implementation and head pointer
        AstInterfaceImpl faImpl = AstInterfaceImpl(loops[0]);
        AstInterface fa(&faImpl);
        AstNodePtr head = AstNodePtrImpl(loops[0]);
        //AstNodePtr head = AstNodePtrImpl(body);
        fa.SetRoot(head);

        //2.  Call dependence analysis directly on a loop node
        //LoopTransformInterface la (fa,aliasInfo,funcInfo); 
        LoopTransformInterface la (fa,array_interface,annot, &array_interface); 
        LoopTreeDepCompCreate comp(la,head);

        // Retrieve dependence graph here!
        cout<<"Dump the dependence graph for the loop in question:"<<endl; 
        comp.DumpDep();
        //LoopTreeNodeDepMap depmap = comp.GetTreeNodeMap();
       
       // Get the loop hierarchy :grab just a top one for now
       // TODO consider complex loop nests like loop {loop, loop} and loop{loop {loop}}
       LoopTreeNode * loop_root = comp.GetLoopTreeRoot();
       ROSE_ASSERT(loop_root!=NULL);
       //loop_root->Dump();

       LoopTreeTraverseSelectLoop loop_nodes(loop_root, LoopTreeTraverse::PreOrder);
       LoopTreeNode * cur_loop = loop_nodes.Current();

       // three-level loop: i,j,k
       AstNodePtr ast_ptr;
       if (cur_loop)
       {  
         //cur_loop->Dump();
         //loop_nodes.Advance();
         //loop_nodes.Current()->Dump();
         //loop_nodes.Advance();
         //loop_nodes.Current()->Dump();
         ast_ptr = cur_loop->GetOrigStmt2();
        // cout<<AstToString(ast_ptr)<<endl;
       }
       else
       {
         continue;
         // Not all loop can be collected by LoopTreeTraverseSelectLoop right now
         // e.g: loops in template function bodies
         //ROSE_ASSERT(false);  
       }

       ROSE_ASSERT(ast_ptr!=NULL);
       SgNode* sg_node = AstNodePtr2Sage(ast_ptr);
       ROSE_ASSERT(sg_node);
      // cout<<"-------------Dump the loops in question------------"<<endl; 
      //   cout<<sg_node->class_name()<<endl;

       //3. Judge if loops are parallelizable
       // Algorithm, for each depInfo, 
       //   commonlevel >=0, depInfo is within a loop
       // simplest one first: 1 level loop only, 
       // iterate dependence edges, 
       // check if there is loop carried dependence at all 
       // (Carry level ==0 for top level common loops)
       bool isParallelizable = true;

       vector<DepInfo>  remainingDependences;

        LoopTreeDepGraph * depgraph =  comp.GetDepGraph(); 
        LoopTreeDepGraph::NodeIterator nodes = depgraph->GetNodeIterator();
        for (; !nodes.ReachEnd(); ++ nodes)
        {
          LoopTreeDepGraph::Node* curnode = *nodes;
          LoopTreeDepGraph::EdgeIterator edges = depgraph->GetNodeEdgeIterator(curnode, GraphAccess::EdgeOut);
          if (!edges.ReachEnd())
          {
             for (; !edges.ReachEnd(); ++edges)
             { 
               LoopTreeDepGraph::Edge *e= *edges;
              // cout<<"dependence edge: "<<e->toString()<<endl;
               DepInfo info =e->GetInfo();
             
               // eliminate dependence relationship if
               // the variables are thread-local: (within the scope of the loop's scope)
               SgScopeStatement * currentscope= getScope(sg_node);  
               SgScopeStatement* varscope =NULL;
               SgNode* src_node = AstNodePtr2Sage(info.SrcRef());
               if (src_node)
               {
                 SgVarRefExp* var_ref = isSgVarRefExp(src_node);
                 if (var_ref)
                 {  
                   varscope= var_ref->get_symbol()->get_scope();
                   if (isAncestor(currentscope,varscope))
                     continue;
                 } //end if(var_ref)
               } // end if (src_node)

                // skip non loop carried dependencies: 
                // TODO scalar issue, wrong CarryLevel
                // loop independent dependencies: need privatization can eliminate most of them
               if (info.CarryLevel()!=0) 
                 continue;
                
               remainingDependences.push_back(info); 
             } //end iterator edges for a node
          } // end if 
        } // end of iterate dependence graph 

      //TODO liveness analysis to classify variables and further eliminate dependences
      
      // Set to unparallelizable if it has dependences which can not eliminated
      if (remainingDependences.size()>0) isParallelizable = false;

       comp.DetachDepGraph();// release resources here
      //4.  Attach OmpAttribute to the loop node if it is parallelizable 
       if (isParallelizable)
       {  
          // TODO add variable classification to set private,shared variables 
          OmpAttribute* omp_attribute = buildOmpAttribute(e_parallel_for,sg_node);
          addOmpAttribute(omp_attribute,sg_node);
          hasOpenMP = true;
        } 
        else
        {
          cout<<"\n Unparallelizable loop at line:"<<sg_node->get_file_info()->get_line()<<
               " due to the following dependencies:"<<endl;
          for (vector<DepInfo>::iterator iter= remainingDependences.begin();     
               iter != remainingDependences.end(); iter ++ )
          {
            cout<<(*iter).toString()<<endl;
          }

        } // end if 

        // 5. Generate and insert #pragma omp parallel for 
        // This phase is deliberately separated from buildOmpAttribute()
        OmpAttribute* att = getOmpAttribute(sg_node); 
        if(att)
        {  
            cout<<"\n Parallelizing a loop at line:"
              <<sg_node->get_file_info()->get_line()<<endl;
            string pragma_str= att->toOpenMPString();
            SgPragmaDeclaration * pragma = SageBuilder::buildPragmaDeclaration(pragma_str); 
            SageInterface::insertStatementBefore(isSgStatement(sg_node), pragma);
        } // end inserting #pragma   

     } // end for-loop for declarations
     // insert omp.h if OpenMP directives have been inserted into the current file 
     if (hasOpenMP)
       SageInterface::insertHeader("omp.h",false,root);
   } //end for-loop of files

  // Qing's loop normalization is not robust enough to pass all tests
  //AstTests::runAllTests(project);
  
  //we only unparse it to avoid -I path/to/omp.h
   project->unparse();
  //return backend (project);
  return 0;
}
