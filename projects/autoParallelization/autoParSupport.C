#include "autoParSupport.h"

#include <iterator> // ostream_iterator
#include <algorithm> // for set union, intersection etc.

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

bool initialize_analysis(SgProject* project/*=NULL*/,bool debug/*=false*/)
{
  // Prepare def-use analysis
  if (defuse==NULL) 
  { 
    ROSE_ASSERT(project != NULL);
    defuse = new DefUseAnalysis(project);
  }
  ROSE_ASSERT(defuse != NULL);
 // int result = ;
  defuse->run(debug);
//  if (result==1)
//    std::cerr<<"Error in Def-use analysis!"<<std::endl;
  if (debug)  
    defuse->dfaToDOT();

  //Prepare variable liveness analysis
  if (liv == NULL)
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

// Get the live-in and live-out variable sets for a for loop, 
// recomputing liveness analysis if requested (useful after program transformation)
// Only consider scalars for now, ignore non-scalar variables
// Also ignore loop invariant variables.
void GetLiveVariables(SgNode* loop, std::vector<SgInitializedName*> &liveIns,
         std::vector<SgInitializedName*> &liveOuts,bool reCompute/*=false*/)
{
  // TODO reCompute : call another liveness analysis function on a target function
  if (reCompute)
    initialize_analysis();

  std::vector<SgInitializedName*> liveIns0, liveOuts0; // store the original one
  SgInitializedName* invarname = getLoopInvariant(loop);
  // Grab the filtered CFG node for SgForStatement
  SgForStatement *forstmt = isSgForStatement(loop);
  ROSE_ASSERT(forstmt);
  // Jeremiah's hidden constructor to grab the right one
  // Several CFG nodes are used for the same SgForStatement
  CFGNode cfgnode(forstmt,2);
  FilteredCFGNode<IsDFAFilter> filternode= FilteredCFGNode<IsDFAFilter> (cfgnode);
  // This one does not return the one we want even its getNode returns the
  // right for statement
  //FilteredCFGNode<IsDFAFilter> filternode= FilteredCFGNode<IsDFAFilter> (forstmt->cfgForBeginning());
  ROSE_ASSERT(filternode.getNode()==forstmt);

  // Check out edges
  vector<FilteredCFGEdge < IsDFAFilter > > out_edges = filternode.outEdges();
  //cout<<"Found edge count:"<<out_edges.size()<<endl;
  //SgForStatement should have two outgoing edges, one true(going into the loop body) and one false (going out the loop)
  ROSE_ASSERT(out_edges.size()==2); 
  vector<FilteredCFGEdge < IsDFAFilter > >::iterator iter= out_edges.begin();
//  std::vector<SgInitializedName*> remove1, remove2;
  for (; iter!=out_edges.end();iter++)
  {
    FilteredCFGEdge < IsDFAFilter > edge= *iter;
   // cout<<"Out CFG edges for a loop:"<<edge.source().getNode()<<endl;
   // cout<<"Out CFG edges for a loop:"<<edge.target().getNode()<<endl;
    //x. Live-in (loop) = live-in (first-stmt-in-loop)
    if (edge.condition()==eckTrue)
    {
      SgNode* firstnode= edge.target().getNode();
      liveIns0 = liv->getIn(firstnode);
     // cout<<"Live-in variables for loop:"<<endl;
      for (std::vector<SgInitializedName*>::iterator iter = liveIns0.begin();
          iter!=liveIns0.end(); iter++)
      {
        SgInitializedName* name = *iter;
        if ((SageInterface::isScalarType(name->get_type()))&&(name!=invarname))
        {
            liveIns.push_back(*iter);
//          remove1.push_back(*iter);
//           cout<< name->get_qualified_name().getString()<<endl;
         }
      }
    }
    //x. live-out(loop) = live-in (first-stmt-after-loop)
    else if (edge.condition()==eckFalse)
    {
      SgNode* firstnode= edge.target().getNode();
      liveOuts0 = liv->getIn(firstnode);
//      cout<<"Live-out variables for loop:"<<endl;
      for (std::vector<SgInitializedName*>::iterator iter = liveOuts0.begin();
          iter!=liveOuts0.end(); iter++)
      {
        SgInitializedName* name = *iter;
        if ((SageInterface::isScalarType(name->get_type()))&&(name!=invarname))
        {
//          cout<< name->get_qualified_name().getString()<<endl;
          liveOuts.push_back(*iter);
//          remove2.push_back(*iter);
        }
      }
    }
    else
    {
      cerr<<"Unexpected CFG out edge type for SgForStmt!"<<endl;
      ROSE_ASSERT(false);
    }
  } // end for (edges)
#if 0 // remove is not stable for unkown reasons
  // sort them for better search/remove 
  sort(liveIns.begin(),liveIns.end());
  sort(liveOuts.begin(),liveOuts.end());

  // Remove non-scalar variables 
  std::vector<SgInitializedName*>::iterator iter2;
  for (iter2=remove1.begin();iter2!=remove1.end();iter2++)
    remove(liveIns.begin(),liveIns.end(),*iter2);
 
  std::vector<SgInitializedName*>::iterator iter3;
  for (iter3=remove2.begin();iter3!=remove2.end();iter3++)
    remove(liveOuts.begin(),liveOuts.end(),*iter3);

  // Remove loop invariant variables
  remove(liveIns.begin(),liveIns.end(),invarname);
  remove(liveOuts.begin(),liveOuts.end(),invarname);
#endif  
 // debug the final results
   cout<<"Final Live-in variables for loop:"<<endl;
  for (std::vector<SgInitializedName*>::iterator iter = liveIns.begin();
      iter!=liveIns.end(); iter++)
  {
    SgInitializedName* name = *iter;
    cout<< name->get_qualified_name().getString()<<endl;
  }
  cout<<"Final Live-out variables for loop:"<<endl;
  for (std::vector<SgInitializedName*>::iterator iter = liveOuts.begin();
      iter!=liveOuts.end(); iter++)
  {
    SgInitializedName* name = *iter;
    cout<< name->get_qualified_name().getString()<<endl;
  }

} // end GetLiveVariables()

// Check if a loop has a canonical form, which has
//  * initialization statements; 
//  * a test expression  using either <= or >= operations
//  * an increment expression using i=i+1, or i=i-1.
// If yes, grab its invariant, lower bound, upper bound, step, and body if requested
#if 0
bool IsCanonicalLoop(SgNode* loop,SgInitializedName* invar/*=0*/, SgExpression* lb/*=0*/,
                    SgExpression* ub/*=0*/, SgExpression* step/*=0*/, SgStatement* body/*=0*/)
{
  bool result;
  ROSE_ASSERT(loop != NULL);
  AstInterfaceImpl faImpl(loop);
  AstInterface fa(&faImpl);
  AstNodePtr ivar2, lb2, ub2,step2, body2;
  AstNodePtrImpl loop2(loop);
  result=fa.IsFortranLoop(loop2, &ivar2, &lb2, &ub2,&step2, &body2); 
  if (invar)
  {
     invar = isSgInitializedName(AstNodePtrImpl(ivar2).get_ptr());
     cout<<"debug IsCanonicalLoop() ivar = "<<invar->get_name().getString()<<" type "<<invar->class_name()<<endl;
  }  
  return result;
}
#endif
// Return the loop invariant of a canonical loop
SgInitializedName* getLoopInvariant(SgNode* loop)
{
  AstInterfaceImpl faImpl(loop);
  AstInterface fa(&faImpl);
  AstNodePtr ivar2 ;
  AstNodePtrImpl loop2(loop);
  bool result=fa.IsFortranLoop(loop2, &ivar2);
  ROSE_ASSERT(result); // Must be a canonical loop
  SgVarRefExp* invar = isSgVarRefExp(AstNodePtrImpl(ivar2).get_ptr());
  ROSE_ASSERT(invar);
  SgInitializedName* invarname = invar->get_symbol()->get_declaration();
  // cout<<"debug ivar:"<<invarname<< " name "
  // <<invarname->get_name().getString()<<endl;
  return invarname;
}

// Collect sorted and unique visible referenced variables within a scope. 
// ignoring loop invariant and local variables declared within the scope. 
// They are less interesting for auto parallelization
void CollectVisibleVaribles(SgNode* loop, std::vector<SgInitializedName*>&
      resultVars, bool scalarOnly/*=false*/)
{
  ROSE_ASSERT(loop !=NULL);
  //Get the scope of the loop
  SgScopeStatement* currentscope = isSgFunctionDeclaration(\
	                SageInterface::getEnclosingFunctionDeclaration(loop))\
	                  ->get_definition()->get_body();
  ROSE_ASSERT(currentscope != NULL);
 
  SgInitializedName* invarname = getLoopInvariant(loop);
  Rose_STL_Container<SgNode*> reflist = NodeQuery::querySubTree(loop, V_SgVarRefExp);
  for (Rose_STL_Container<SgNode*>::iterator i=reflist.begin();i!=reflist.end();i++)
   {
      SgInitializedName* initname= isSgVarRefExp(*i)->get_symbol()->get_declaration();
      SgScopeStatement* varscope=initname->get_scope();
      // only collect variables which are visible at the loop's scope
      // varscope is equal or higher than currentscope 
      if ((currentscope==varscope)||(SageInterface::isAncestor(varscope,currentscope)))
      { 
         // Skip non-scalar if scalarOnly is requested
         if ((scalarOnly)&& !SageInterface::isScalarType(initname->get_type()))
           continue;
         if (invarname!=initname)  
           resultVars.push_back(initname);
      }
   } // end for()

#if 0  // remove is not stable ??
 //skip loop invariant variable:
 SgInitializedName* invarname = getLoopInvariant(loop);
 remove(resultVars.begin(),resultVars.end(),invarname);
#endif
 //Remove duplicated items 
  sort(resultVars.begin(),resultVars.end()); 
  std::vector<SgInitializedName*>::iterator new_end= unique(resultVars.begin(),resultVars.end());
  resultVars.erase(new_end, resultVars.end());
}

// Variable classification for a loop node based on liveness analysis
// Collect private, firstprivate, lastprivate, reduction and save into attribute
// We only consider scalars for now 
void AutoScoping(SgNode *sg_node, OmpSupport::OmpAttribute* attribute)
{
  ROSE_ASSERT(sg_node&&attribute);
  // Variable liveness analysis
   std::vector<SgInitializedName*> liveIns;
   std::vector<SgInitializedName*> liveOuts;
   // Turn on recomputing since transformations have been done
   //GetLiveVariables(sg_node,liveIns,liveOuts,true);
   // TODO Loop normalization messes up AST or 
   // the existing analysis can not be called multiple times
   GetLiveVariables(sg_node,liveIns,liveOuts,false);
   // Remove loop invariant variable, which is always private 
  SgInitializedName* invarname = getLoopInvariant(sg_node);
  remove(liveIns.begin(),liveIns.end(),invarname);
  remove(liveOuts.begin(),liveOuts.end(),invarname);

  std::vector<SgInitializedName*> allVars,privateVars,lastprivateVars, firstprivateVars,reductionVars;
   CollectVisibleVaribles(sg_node,allVars,true);
#if 1
   cout<<"Debug after CollectVisibleVaribles():"<<endl;
   for (std::vector<SgInitializedName*>::iterator iter = allVars.begin(); iter!= allVars.end();iter++)
   {
     cout<<(*iter)<<" "<<(*iter)->get_qualified_name().getString()<<endl;
   }
#endif  
   // We should only concern about variables with some kind of dependences
  // But we use the variable to eleminate dependences later on, so we can tolerate a bigger set.
  /*               live-in      live-out
     private           N           N      allVars - liveIns - liveOuts  
     lastprivate       N           Y      liveOuts - liveIns
     firstprivate      Y           N      liveIns - liveOuts
     reduction         Y           Y      liveIns Intersection liveOuts
  */ 
  sort(liveIns.begin(), liveIns.end());
  sort(liveOuts.begin(), liveOuts.end());
  //private:TODO double check possible conflicts with shared, or reduction
  std::vector<SgInitializedName*> temp;
  set_difference(allVars.begin(),allVars.end(), liveIns.begin(), liveIns.end(),
               inserter(temp, temp.begin()));
  set_difference(temp.begin(),temp.end(), liveOuts.begin(), liveOuts.end(),
               inserter(privateVars, privateVars.end()));	
  privateVars.push_back(invarname);  //TODO check Does the scope matter? for (int i..)     
  cout<<"Debug dump private:"<<endl;
  for (std::vector<SgInitializedName*>::iterator iter = privateVars.begin(); iter!= privateVars.end();iter++) 
  {
    attribute->addVariable(OmpSupport::e_private ,(*iter)->get_name().getString(), *iter);
     cout<<(*iter)<<" "<<(*iter)->get_qualified_name().getString()<<endl;
  }
  //lastprivate: 
  set_difference(liveOuts.begin(), liveOuts.end(), liveIns.begin(), liveIns.end(),
            inserter(lastprivateVars, lastprivateVars.begin()));
   cout<<"Debug dump lastprivate:"<<endl;
  for (std::vector<SgInitializedName*>::iterator iter = lastprivateVars.begin(); iter!= lastprivateVars.end();iter++) 
  {
    attribute->addVariable(OmpSupport::e_lastprivate ,(*iter)->get_name().getString(), *iter);
     cout<<(*iter)<<" "<<(*iter)->get_qualified_name().getString()<<endl;
  }
  // firstprivate:
  set_difference(liveIns.begin(), liveIns.end(), liveOuts.begin(),liveOuts.end(),
                 inserter(firstprivateVars, firstprivateVars.begin()));
  cout<<"Debug dump firstprivate:"<<endl;
  for (std::vector<SgInitializedName*>::iterator iter = firstprivateVars.begin(); iter!= firstprivateVars.end();iter++) 
  {
    attribute->addVariable(OmpSupport::e_firstprivate ,(*iter)->get_name().getString(), *iter);
     cout<<(*iter)<<" "<<(*iter)->get_qualified_name().getString()<<endl;
  }
 
}

// Collect all classified variables from an OmpAttribute attached to a loop node
void CollectScopedVariables(OmpSupport::OmpAttribute* attribute, std::vector<SgInitializedName*>& result)
{
  ROSE_ASSERT(attribute!=NULL);
  // private, firstprivate, lastprivate, reduction
  std::vector < std::pair <std::string,SgNode*> > privateVars, firstprivateVars,
    lastprivateVars,reductionVars;
  privateVars     = attribute->getVariableList(OmpSupport::e_private);
  firstprivateVars= attribute->getVariableList(OmpSupport::e_firstprivate);
  lastprivateVars = attribute->getVariableList(OmpSupport::e_lastprivate);
  reductionVars   = attribute->getVariableList(OmpSupport::e_reduction);

  std::vector < std::pair <std::string,SgNode*> >::iterator iter;
  for (iter=privateVars.begin();iter!=privateVars.end();iter++)
  {
    SgInitializedName* initname= isSgInitializedName((*iter).second);
    ROSE_ASSERT(initname!=NULL);
    result.push_back(initname);
  }
  for (iter=firstprivateVars.begin();iter!=firstprivateVars.end();iter++)
  {
    SgInitializedName* initname= isSgInitializedName((*iter).second);
    ROSE_ASSERT(initname!=NULL);
    result.push_back(initname);
  }
  for (iter=lastprivateVars.begin();iter!=lastprivateVars.end();iter++)
  {
    SgInitializedName* initname= isSgInitializedName((*iter).second);
    ROSE_ASSERT(initname!=NULL);
    result.push_back(initname);
  }
  for (iter=reductionVars.begin();iter!=reductionVars.end();iter++)
  {
    SgInitializedName* initname= isSgInitializedName((*iter).second);
    ROSE_ASSERT(initname!=NULL);
    result.push_back(initname);
  }
  // avoid duplicated items
  sort(result.begin(), result.end());
  std::vector<SgInitializedName*>::iterator new_end=unique(result.begin(),result.end());
  result.erase(new_end,result.end());
}

// Algorithm, eliminate the following dependencies
// *  commonlevel >=0, depInfo is within a loop
// *  carry level !=0, loop independent,
// *  either source or sink variable is thread local variable 
// *  dependencies caused by autoscoped variables (private, firstprivate, lastprivate, reduction)
// * two array references, but SCALAR_DEP or SCALAR_BACK_DEP dependencies
// OmpAttribute provides scoped variables
// ArrayInterface and ArrayAnnotation support optional annotation based high level array abstractions
void DependenceElimination(SgNode* sg_node, LoopTreeDepGraph* depgraph, std::vector<DepInfo>& remainings, OmpSupport::OmpAttribute* att, ArrayInterface* array_interface/*=0*/, ArrayAnnotation* annot/*=0*/)
{
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
         //cout<<"dependence edge: "<<e->toString()<<endl;
         DepInfo info =e->GetInfo();
       
         // x. Eliminate dependence relationship if
         // either of the source or sink variables are thread-local: 
         // (within the scope of the loop's scope)
         SgScopeStatement * currentscope= SageInterface::getScope(sg_node);  
         SgScopeStatement* varscope =NULL;
         SgNode* src_node = AstNodePtr2Sage(info.SrcRef());
         SgInitializedName* src_name=NULL;
         if (src_node)
         {
           SgVarRefExp* var_ref = isSgVarRefExp(src_node);
           if (var_ref)
           {  
             varscope= var_ref->get_symbol()->get_scope();
             src_name = var_ref->get_symbol()->get_declaration();
             if (SageInterface::isAncestor(currentscope,varscope))
               continue;
           } //end if(var_ref)
         } // end if (src_node)
         SgNode* snk_node = AstNodePtr2Sage(info.SnkRef());
         SgInitializedName* snk_name=NULL;
         if (snk_node)
         {
           SgVarRefExp* var_ref = isSgVarRefExp(snk_node);
           if (var_ref)
           {  
             varscope= var_ref->get_symbol()->get_scope();
             snk_name = var_ref->get_symbol()->get_declaration();
             if (SageInterface::isAncestor(currentscope,varscope))
               continue;
           } //end if(var_ref)
         } // end if (snk_node)

         //x. Eliminate a dependence if 
         // both the source and sink variables are array references (not scalar) 
         // But the dependence type is scalar type
         bool isArray1=false, isArray2=false; 
         AstInterfaceImpl faImpl=AstInterfaceImpl(sg_node);
         AstInterface fa(&faImpl);
         // If we have array annotation, use loop transformation interface's IsArrayAccess()
         if (array_interface&& annot)
         {
           LoopTransformInterface la (fa,*array_interface, annot, array_interface);
           isArray1= la.IsArrayAccess(info.SrcRef());
           isArray2= la.IsArrayAccess(info.SnkRef());
         }
         else // use AstInterface's IsArrayAccess() otherwise
         {
           isArray1= fa.IsArrayAccess(info.SrcRef());
           isArray2= fa.IsArrayAccess(info.SnkRef());
         }
         if (isArray1 && isArray2)
         {
           if ((info.GetDepType() & DEPTYPE_SCALAR)||(info.GetDepType() & DEPTYPE_BACKSCALAR))
             continue;
         }


         //x. Eliminate dependencies caused by autoscoped variables
         // such as private, firstprivate, lastprivate, and reduction
	 if(att&& (src_name || snk_name)) // either src or snk might be an array reference 
	 {
	   std::vector<SgInitializedName*> scoped_vars;
	   CollectScopedVariables(att, scoped_vars);
	   std::vector<SgInitializedName*>::iterator hit1,hit2;
	   //for (hit1=scoped_vars.begin();hit1!=scoped_vars.end();hit1++)
	   //  cout<<"scoped var:"<<*hit1 <<" name:"<<(*hit1)->get_name().getString()<<endl;
	   if (src_name)
  	     hit1=find(scoped_vars.begin(),scoped_vars.end(),src_name);
	   if (snk_name)
	     hit2=find(scoped_vars.begin(),scoped_vars.end(),snk_name);
	   if (hit1!=scoped_vars.end() || (hit2!=scoped_vars.end()))
	     continue;
	 }
 
          // x. Eliminate loop-independent dependencies: 
          // loop independent dependencies: privatization can eliminate most of them
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
