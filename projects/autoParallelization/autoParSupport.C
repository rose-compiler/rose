#include "autoParSupport.h"

using namespace std;
// Everything should go into the name space here!!
namespace AutoParallelization{

DFAnalysis * defuse = NULL;
LivenessAnalysis* liv = NULL;

void autopar_command_processing(vector<string>&argvList)
{
  //Save -debugdep, -annot file .. etc, 
  // used internally in ReadAnnotation and Loop transformation
  CmdOptions::GetInstance()->SetOptions(argvList);
  bool dumpAnnot = CommandlineProcessing::isOption(argvList,"","-dumpannot",true);

  //Read in annotation files after -annot 
  ArrayAnnotation* annot = ArrayAnnotation::get_inst();
  annot->register_annot();
  ReadAnnotation::get_inst()->read();
  if (dumpAnnot)  
    annot->Dump();
    //Strip off custom options and their values to enable backend compiler 
  CommandlineProcessing::removeArgsWithParameters(argvList,"-annot");
}

bool initialize_analysis(SgProject* project,bool debug/*=false*/)
{
  ROSE_ASSERT(project != NULL);

  // Prepare def-use analysis
  defuse = new DefUseAnalysis(project);
  ROSE_ASSERT(defuse != NULL);
  int result = defuse->run(debug);
  if (result==0)
    std::cerr<<"Error in Def-use analysis!"<<std::endl;
  if (debug)  
    defuse->dfaToDOT();

  //Prepare variable liveness analysis
  liv = new LivenessAnalysis(debug,(DefUseAnalysis*)defuse);
  ROSE_ASSERT(liv != NULL);

  std::vector <FilteredCFGNode < IsDFAFilter > > dfaFunctions;
  NodeQuerySynthesizedAttributeType vars = 
          NodeQuery::querySubTree(project, V_SgFunctionDefinition); 
  NodeQuerySynthesizedAttributeType::const_iterator i;
  bool abortme=false;
     // run liveness analysis on each function body
  for (i= vars.begin(); i!=vars.end();++i) 
  {
    SgFunctionDefinition* func = isSgFunctionDefinition(*i);
    if (debug)
    {
      std::string name = func->class_name();
      string funcName = func->get_declaration()->get_qualified_name().str();
      cout<< " .. running liveness analysis for function: " << funcName << endl;
    }
    FilteredCFGNode <IsDFAFilter> rem_source = liv->run(func,abortme);
    if (rem_source.getNode()!=NULL)
      dfaFunctions.push_back(rem_source);    
    if (abortme)
      break;
  } // end for ()
  if(debug)
  {
    cout << "Writing out liveness analysis results into var.dot... " << endl;
    std::ofstream f2("var.dot");
    dfaToDot(f2, string("var"), dfaFunctions, (DefUseAnalysis*)defuse, liv);
    f2.close();
  }
  if (abortme) {
    cerr<<"Error: Liveness analysis is ABORTING ." << endl;
    ROSE_ASSERT(false);
  }
  return !abortme;
} // end initialize_analysis()

void release_analysis()
{
  if(defuse!=NULL) 
    delete defuse;
  if (liv !=NULL) 
    delete liv;
}

//Compute dependence graph for a loop, using ArrayInterface and ArrayAnnoation
LoopTreeDepGraph*  ComputeDependenceGraph(SgNode* loop, ArrayInterface* array_interface, ArrayAnnotation* annot)
{
  ROSE_ASSERT(loop && array_interface&& annot);
  //TODO check if its a canonical loop

  // Prepare AstInterface: implementation and head pointer
  AstInterfaceImpl faImpl_2 = AstInterfaceImpl(loop);
  //AstInterface fa(&faImpl); // Using CPP interface to handle templates etc.
  CPPAstInterface fa(&faImpl_2);
  AstNodePtr head = AstNodePtrImpl(loop);
  //AstNodePtr head = AstNodePtrImpl(body);
  fa.SetRoot(head);

  // Call dependence analysis directly on a loop node
    //LoopTransformInterface la (fa,aliasInfo,funcInfo); 
  LoopTransformInterface la (fa,*array_interface, annot, array_interface); 
  LoopTreeDepCompCreate* comp = new LoopTreeDepCompCreate(la,head);// TODO when to release this?
  // Retrieve dependence graph here!
  cout<<"Dump the dependence graph for the loop in question:"<<endl; 
  comp->DumpDep();

  // The following code was used when an entire function body with several loops
  // is analyzed for dependence analysis. I keep it to double check the computation.
   
  // Get the loop hierarchy :grab just a top one for now
  // TODO consider complex loop nests like loop {loop, loop} and loop{loop {loop}}
  LoopTreeNode * loop_root = comp->GetLoopTreeRoot();
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
    cout<<"Skipping a loop not recognized by LoopTreeTraverseSelectLoop ..."<<endl;
    return NULL;
    // Not all loop can be collected by LoopTreeTraverseSelectLoop right now
    // e.g: loops in template function bodies
    //ROSE_ASSERT(false);  
  }
  ROSE_ASSERT(ast_ptr!=NULL);
  SgNode* sg_node = AstNodePtr2Sage(ast_ptr);
  ROSE_ASSERT(sg_node == loop);
  // cout<<"-------------Dump the loops in question------------"<<endl; 
  //   cout<<sg_node->class_name()<<endl;
  return comp->GetDepGraph();   
}

// Algorithm, for each depInfo, 
//   commonlevel >=0, depInfo is within a loop
// simplest one first: 1 level loop only, 
// iterate dependence edges, 
// check if there is loop carried dependence at all 
// (Carry level ==0 for top level common loops)
//TODO liveness analysis to classify variables and further eliminate dependences
void DependenceElimination(SgNode* sg_node, LoopTreeDepGraph* depgraph, std::vector<DepInfo>& remainings)
{
#if 1 // experiments with liveness analysis and cfg
  // Grab the filtered CFG node for SgForStatement
  SgForStatement *forstmt = isSgForStatement(sg_node);
  ROSE_ASSERT(forstmt);
  CFGNode cfgnode(forstmt,2);// Jeremiah's hidden constructor
  FilteredCFGNode<IsDFAFilter> filternode= FilteredCFGNode<IsDFAFilter> (cfgnode);
  //FilteredCFGNode<IsDFAFilter> filternode= FilteredCFGNode<IsDFAFilter> (forstmt->cfgForBeginning());
  
  ROSE_ASSERT(filternode.getNode()==forstmt);

  std::vector<SgInitializedName*> invars = liv->getIn(forstmt);
  for (std::vector<SgInitializedName*>::iterator iter = invars.begin();
      iter!=invars.end(); iter++)
  {
    SgInitializedName* name = *iter;
    cout<<"Live-in variables for loop:"<< name->get_qualified_name().getString()<<endl;
  }  

  // Check out edges
  vector<FilteredCFGEdge < IsDFAFilter > > out_edges = filternode.outEdges(); 
  cout<<"Found edge count:"<<out_edges.size()<<endl;
  vector<FilteredCFGEdge < IsDFAFilter > >::iterator iter= out_edges.begin();
  for (; iter!=out_edges.end();iter++) 
  {
    FilteredCFGEdge < IsDFAFilter > edge= *iter;
    cout<<"Out CFG edges for a loop:"<<edge.source().getNode()<<endl;
    cout<<"Out CFG edges for a loop:"<<edge.target().getNode()<<endl;
  }
#endif  

  //LoopTreeDepGraph * depgraph =  comp.GetDepGraph(); 
  LoopTreeDepGraph::NodeIterator nodes = depgraph->GetNodeIterator();
  // For each node
  for (; !nodes.ReachEnd(); ++ nodes) 
  {
    LoopTreeDepGraph::Node* curnode = *nodes;
    LoopTreeDepGraph::EdgeIterator edges = depgraph->GetNodeEdgeIterator(curnode, GraphAccess::EdgeOut);
    // If the node has edges
    if (!edges.ReachEnd())
    {
       // for each edge
       for (; !edges.ReachEnd(); ++edges) 
       { 
         LoopTreeDepGraph::Edge *e= *edges;
        // cout<<"dependence edge: "<<e->toString()<<endl;
         DepInfo info =e->GetInfo();
       
         // eliminate dependence relationship if
         // the variables are thread-local: (within the scope of the loop's scope)
         SgScopeStatement * currentscope= SageInterface::getScope(sg_node);  
         SgScopeStatement* varscope =NULL;
         SgNode* src_node = AstNodePtr2Sage(info.SrcRef());
         if (src_node)
         {
           SgVarRefExp* var_ref = isSgVarRefExp(src_node);
           if (var_ref)
           {  
             varscope= var_ref->get_symbol()->get_scope();
             if (SageInterface::isAncestor(currentscope,varscope))
               continue;
           } //end if(var_ref)
         } // end if (src_node)

          // skip non loop carried dependencies: 
          // TODO scalar issue, wrong CarryLevel
          // loop independent dependencies: need privatization can eliminate most of them
         if (info.CarryLevel()!=0) 
           continue;
         // Save the rest dependences which can not be ruled out 
         remainings.push_back(info); 
       } //end iterator edges for a node
    } // end if has edge
  } // end of iterate dependence graph 

}// end DependenceElimination()


//Generate and insert #pragma omp parallel for 
//This phase is deliberately separated from building and attaching OmpAttribute
void generatedOpenMPPragmas(SgNode* sg_node)
{
  OmpSupport::OmpAttribute* att = OmpSupport::getOmpAttribute(sg_node); 
  if(att)
  {  
      cout<<"\n Parallelizing a loop at line:"
        <<sg_node->get_file_info()->get_line()<<endl;
      string pragma_str= att->toOpenMPString();
      SgPragmaDeclaration * pragma = SageBuilder::buildPragmaDeclaration(pragma_str); 
      SageInterface::insertStatementBefore(isSgStatement(sg_node), pragma);
  }
}


} // end namespace
