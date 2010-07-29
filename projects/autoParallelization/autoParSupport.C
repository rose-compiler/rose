#include "rose.h"
#include "autoParSupport.h"

#include <iterator> // ostream_iterator
#include <algorithm> // for set union, intersection etc.
#include <fstream>
#include <iostream>
#include <map>

using namespace std;
using namespace OmpSupport;
// Everything should go into the name space here!!
namespace AutoParallelization
{
  bool enable_debug;
  bool enable_patch;
  bool enable_diff;
  bool b_unique_indirect_index;
  DFAnalysis * defuse = NULL;
  LivenessAnalysis* liv = NULL;

  void autopar_command_processing(vector<string>&argvList)
  {
    if (CommandlineProcessing::isOption (argvList,"-rose:autopar:","enable_debug",true))
    {
      cout<<"Enabling debugging mode for auto parallelization ..."<<endl;
      enable_debug= true;
    }
    else
      enable_debug= false;

    if (CommandlineProcessing::isOption (argvList,"-rose:autopar:","enable_patch",true))
    {
      cout<<"Enabling generating patch files for auto parallelization ..."<<endl;
      enable_patch= true;
    }
    else
      enable_patch= false;

    if (CommandlineProcessing::isOption (argvList,"-rose:autopar:","unique_indirect_index",true))
    {
      cout<<"Assuming all arrays used as indirect indices have unique elements (no overlapping) ..."<<endl;
      b_unique_indirect_index= true;
    }
    else
      b_unique_indirect_index= false;


    if (CommandlineProcessing::isOption (argvList,"-rose:autopar:","enable_diff",true))
    {
      cout<<"Enabling compare user defined OpenMP pragmas to auto parallelization generated ones ..."<<endl;
      enable_diff = true;
    }
    else
      enable_diff = false;

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
    // keep --help option after processing, let other modules respond also
    if ((CommandlineProcessing::isOption (argvList,"--help","",false)) ||
        (CommandlineProcessing::isOption (argvList,"-help","",false)))
    {
      cout<<"Auto parallelization-specific options"<<endl;
      cout<<"\t-rose:autopar:enable_debug          run automatic parallelization in a debugging mode"<<endl;
      cout<<"\t-rose:autopar:enable_patch          additionally generate patch files for translations"<<endl;
      cout<<"\t-rose:autopar:unique_indirect_index assuming all arrays used as indirect indices have unique elements (no overlapping)"<<endl;
      cout<<"\t-annot filename                     specify annotation file for semantics of abstractions"<<endl;
      cout<<"\t-dumpannot                          dump annotation file content"<<endl;
      cout <<"---------------------------------------------------------------"<<endl;
    }

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
  // TODO generate dep graph for the entire function and reuse it for all loops
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
    if (enable_debug) 
    {
      cout<<"Debug: Dump the dependence graph for the loop in question:"<<endl; 
      comp->DumpDep();
    }

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
      ROSE_ASSERT(ast_ptr!=NULL);
      SgNode* sg_node = AstNodePtr2Sage(ast_ptr);
      ROSE_ASSERT(sg_node == loop);
      // cout<<"-------------Dump the loops in question------------"<<endl; 
      //   cout<<sg_node->class_name()<<endl;
      return comp->GetDepGraph();   
    }
    else
    {
      cout<<"Skipping a loop not recognized by LoopTreeTraverseSelectLoop ..."<<endl;
      return NULL;
      // Not all loop can be collected by LoopTreeTraverseSelectLoop right now
      // e.g: loops in template function bodies
      //ROSE_ASSERT(false);  
    }
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
      // Used to verify CFG nodes in var.dot dump
      //x. Live-in (loop) = live-in (first-stmt-in-loop)
      if (edge.condition()==eckTrue)
      {
        SgNode* firstnode= edge.target().getNode();
        liveIns0 = liv->getIn(firstnode);
        if (enable_debug)
          cout<<"Live-in variables for loop:"<<endl;
        for (std::vector<SgInitializedName*>::iterator iter = liveIns0.begin();
            iter!=liveIns0.end(); iter++)
        {
          SgInitializedName* name = *iter;
          if ((SageInterface::isScalarType(name->get_type()))&&(name!=invarname))
          {
            liveIns.push_back(*iter);
            //          remove1.push_back(*iter);
            if (enable_debug)
              cout<< name->get_qualified_name().getString()<<endl;
          }
        }
      }
      //x. live-out(loop) = live-in (first-stmt-after-loop)
      else if (edge.condition()==eckFalse)
      {
        SgNode* firstnode= edge.target().getNode();
        liveOuts0 = liv->getIn(firstnode);
        if (enable_debug)
          cout<<"Live-out variables for loop:"<<endl;
        for (std::vector<SgInitializedName*>::iterator iter = liveOuts0.begin();
            iter!=liveOuts0.end(); iter++)
        {
          SgInitializedName* name = *iter;
          if ((SageInterface::isScalarType(name->get_type()))&&(name!=invarname))
          {
            if (enable_debug)
              cout<< name->get_qualified_name().getString()<<endl;
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
    if(enable_debug)
    {
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
  // Return NULL if the loop is not canonical
  SgInitializedName* getLoopInvariant(SgNode* loop)
  {
    AstInterfaceImpl faImpl(loop);
    AstInterface fa(&faImpl);
    AstNodePtr ivar2 ;
    AstNodePtrImpl loop2(loop);
    bool result=fa.IsFortranLoop(loop2, &ivar2);
    if (!result)
      return NULL;
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
      resultVars, std::vector<SgInitializedName*>& invariantVars, bool scalarOnly/*=false*/)
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

    // collect loop invariants here
    Rose_STL_Container<SgNode*> loopnests= NodeQuery::querySubTree(loop, V_SgForStatement);
    for (Rose_STL_Container<SgNode*>::iterator iter=loopnests.begin();
        iter!=loopnests.end(); iter++)
    {
      SgForStatement* forstmt= isSgForStatement(*iter);
      SgInitializedName* invariant = getLoopInvariant(forstmt);
      if (invariant)
      {
        SgScopeStatement* varscope=invariant->get_scope();
        // only collect variables which are visible at the loop's scope
        // varscope is equal or higher than currentscope 
        if ((currentscope==varscope)||(SageInterface::isAncestor(varscope,currentscope)))
          invariantVars.push_back(invariant); 
      }
    }
#if 0  // remove is not stable ??
    //skip loop invariant variable:
    SgInitializedName* invarname = getLoopInvariant(loop);
    remove(resultVars.begin(),resultVars.end(),invarname);
#endif
    //Remove duplicated items 
    sort(resultVars.begin(),resultVars.end()); 
    std::vector<SgInitializedName*>::iterator new_end= unique(resultVars.begin(),resultVars.end());
    resultVars.erase(new_end, resultVars.end());

    sort(invariantVars.begin(),invariantVars.end()); 
    new_end= unique(invariantVars.begin(),invariantVars.end());
    invariantVars.erase(new_end, invariantVars.end());
  }

  //! Collect a loop's variables which cause any kind of dependencies. Consider scalars only if requested.  
  // depgraph may contain dependencies for the entire function enclosing the loop. So we need to ignore irrelevant ones with respect to the loop
  void CollectVariablesWithDependence(SgNode* loop, LoopTreeDepGraph* depgraph,std::vector<SgInitializedName*>& resultVars,bool scalarOnly/*=false*/)
  {
    ROSE_ASSERT(isSgForStatement(loop)&& depgraph);
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
          // Indicate if the variable references happen within the loop
          bool insideLoop1=false, insideLoop2=false;

          SgScopeStatement * loopscope= SageInterface::getScope(loop);  
          SgScopeStatement* varscope =NULL;
          SgNode* src_node = AstNodePtr2Sage(info.SrcRef());
          SgInitializedName* src_name=NULL;
          if (src_node)
          { //TODO May need to consider a wider concept of variable reference 
            //like AstInterface::IsVarRef()
            SgVarRefExp* var_ref = isSgVarRefExp(src_node);
            if (var_ref)
            {  
              varscope= var_ref->get_symbol()->get_scope();
              src_name = var_ref->get_symbol()->get_declaration();
              // Ignore the local variables declared inside the loop
              if (SageInterface::isAncestor(loopscope,varscope))
                continue;
              if (SageInterface::isAncestor(loopscope,var_ref))
                insideLoop1= true;
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
              if (SageInterface::isAncestor(loopscope,varscope))
                continue;
              if (SageInterface::isAncestor(loopscope,var_ref))
                insideLoop2= true;
            } //end if(var_ref)
          } // end if (snk_node)
          // Only collect the dependence relation involving 
          // two variables referenced within the loop
          if (insideLoop1&& insideLoop2)
          {
            if (scalarOnly) 
            { // Only meaningful if both are scalars 
              if(SageInterface::isScalarType(src_name->get_type())
                  &&SageInterface::isScalarType(snk_name->get_type()))
              {  
                resultVars.push_back(src_name);
                resultVars.push_back(snk_name);
              }  
            }
            else
            {
              resultVars.push_back(src_name);
              resultVars.push_back(snk_name);
            }
          }  
        } //end iterator edges for a node
      } // end if has edge
    } // end of iterate dependence graph 
    // remove duplicated items
    sort(resultVars.begin(), resultVars.end());
    std::vector<SgInitializedName*>::iterator new_end=unique(resultVars.begin(),resultVars.end());
    resultVars.erase(new_end,resultVars.end());
  }

  // Variable classification for a loop node based on liveness analysis
  // Collect private, firstprivate, lastprivate, reduction and save into attribute
  // We only consider scalars for now 
  // Algorithm: private and reduction variables cause dependences (being written)
  //            firstprivate and lastprivate variables are never being written in the loop (no dependences)
    /*                              live-in      live-out
                     shared            Y           Y      no written, no dependences: no special handling, shared by default 
                     private           N           N      written (depVars), need privatization: depVars- liveIns - liveOuts  
                     firstprivate      Y           N      liveIns - LiveOus - writtenVariables
                     lastprivate       N           Y      liveOuts - LiveIns 
                     reduction         Y           Y      depVars Intersection (liveIns Intersection liveOuts)
                     */ 
 
  void AutoScoping(SgNode *sg_node, OmpSupport::OmpAttribute* attribute,LoopTreeDepGraph* depgraph)
  {
    ROSE_ASSERT(sg_node&&attribute&&depgraph);
    ROSE_ASSERT (isSgForStatement(sg_node));

    // Variable liveness analysis: original ones and 
    // the one containing only variables with some kind of dependencies
    std::vector<SgInitializedName*> liveIns0, liveIns;
    std::vector<SgInitializedName*> liveOuts0, liveOuts;
    // Turn on recomputing since transformations have been done
    //GetLiveVariables(sg_node,liveIns,liveOuts,true);
    // TODO Loop normalization messes up AST or 
    // the existing analysis can not be called multiple times
    GetLiveVariables(sg_node,liveIns0,liveOuts0,false);
    // Remove loop invariant variable, which is always private 
    SgInitializedName* invarname = getLoopInvariant(sg_node);
    remove(liveIns0.begin(),liveIns0.end(),invarname);
    remove(liveOuts0.begin(),liveOuts0.end(),invarname);

    std::vector<SgInitializedName*> allVars,depVars, invariantVars, privateVars,lastprivateVars, 
      firstprivateVars,reductionVars, reductionResults;
    // Only consider scalars for now
    CollectVisibleVaribles(sg_node,allVars,invariantVars,true);
    CollectVariablesWithDependence(sg_node,depgraph,depVars,true);
    if (enable_debug)
    {
      cout<<"Debug after CollectVariablesWithDependence():"<<endl;
      for (std::vector<SgInitializedName*>::iterator iter = depVars.begin(); iter!= depVars.end();iter++)
      {
        cout<<(*iter)<<" "<<(*iter)->get_qualified_name().getString()<<endl;
      }
    }
    sort(liveIns0.begin(), liveIns0.end());
    sort(liveOuts0.begin(), liveOuts0.end());

    // We concern about variables with some kind of dependences
    // Since private and reduction variables cause some kind of dependencies ,
    // which otherwise prevent parallelization
    // liveVars intersection depVars
    //Remove the live variables which have no relevant dependencies
    set_intersection(liveIns0.begin(),liveIns0.end(), depVars.begin(), depVars.end(),
        inserter(liveIns, liveIns.begin()));
    set_intersection(liveOuts0.begin(),liveOuts0.end(), depVars.begin(), depVars.end(),
        inserter(liveOuts, liveOuts.begin()));

    sort(liveIns.begin(), liveIns.end());
    sort(liveOuts.begin(), liveOuts.end());
    // shared: scalars for now: allVars - depVars, 

    //private:
    //---------------------------------------------
    //depVars- liveIns - liveOuts
    std::vector<SgInitializedName*> temp;
    set_difference(depVars.begin(),depVars.end(), liveIns.begin(), liveIns.end(),
        inserter(temp, temp.begin()));
    set_difference(temp.begin(),temp.end(), liveOuts.begin(), liveOuts.end(),
        inserter(privateVars, privateVars.end()));	
    // loop invariants are private
    // insert all loops, including nested ones' visible invariants
    for(std::vector<SgInitializedName*>::iterator iter =invariantVars.begin();
        iter!=invariantVars.end(); iter++)
      privateVars.push_back(*iter);
    if(enable_debug)
      cout<<"Debug dump private:"<<endl;
    for (std::vector<SgInitializedName*>::iterator iter = privateVars.begin(); iter!= privateVars.end();iter++) 
    {
      attribute->addVariable(OmpSupport::e_private ,(*iter)->get_name().getString(), *iter);

      if(enable_debug)
        cout<<(*iter)<<" "<<(*iter)->get_qualified_name().getString()<<endl;
    }

    //lastprivate: liveOuts - LiveIns 
    // Must be written and LiveOut to have the need to preserve the value:  DepVar Intersect LiveOut
    // Must not be Livein to ensure correct semantics: private for each iteration, not getting value from previous iteration.
    //  e.g.  for ()   {  a = 1; }  = a; 
    //---------------------------------------------
    set_difference(liveOuts.begin(), liveOuts.end(), liveIns0.begin(), liveIns0.end(),
        inserter(lastprivateVars, lastprivateVars.begin()));

    if(enable_debug)
      cout<<"Debug dump lastprivate:"<<endl;
    for (std::vector<SgInitializedName*>::iterator iter = lastprivateVars.begin(); iter!= lastprivateVars.end();iter++) 
    {
      attribute->addVariable(OmpSupport::e_lastprivate ,(*iter)->get_name().getString(), *iter);
      if(enable_debug)
        cout<<(*iter)<<" "<<(*iter)->get_qualified_name().getString()<<endl;
    }
    // reduction recognition
    //---------------------------------------------
    // Some 'bad' examples have reduction variables which are not used after the loop
    // So we relax the constrains as liveIns only for reduction variables
#if 0
    set_intersection(liveIns.begin(),liveIns.end(), liveOuts.begin(), liveOuts.end(),
        inserter(reductionVars, reductionVars.begin()));
    RecognizeReduction(sg_node,attribute, reductionVars);
#else
    reductionResults = RecognizeReduction(sg_node,attribute, liveIns);
#endif   

#if 0
    // this code is wrong as reduction variables definitely cause dependences
    // They don't intersect with firstprivate variables at all.
    // firstprivate:  liveIns - reductionResults  - liveOuts
    // reduction variables with relaxed constrains (not liveOut) may be wrongfully recognized 
    // as firstprivate, so we recognize reduction variables before recognizing 
    // firstprivate and exclude reduction variables first.
    //set_difference(liveIns0.begin(), liveIns0.end(), reductionResults.begin(),reductionResults.end(),
    //inserter(temp2, temp2.begin()));
    // set_difference(temp2.begin(), temp2.end(), liveOuts.begin(),liveOuts.end(),
    //    inserter(firstprivateVars, firstprivateVars.begin()));
#endif        
    // Liao 5/28/2010: firstprivate variables should not cause any dependencies, equal to should be be written in the loop    
    // firstprivate:  liveIns - LiveOuts - writtenVariables (or depVars)
    //---------------------------------------------
    //     liveIn : the need to pass in value
    //     not liveOut: differ from Shared, we considered shared first, then firstprivate
    //     not written: ensure the correct semantics: each iteration will use a copy from the original master, not redefined
    //                  value from the previous iteration
    if(enable_debug)
      cout<<"Debug dump firstprivate:"<<endl;
      
    std::vector<SgInitializedName*> temp2;
    set_difference(liveIns0.begin(), liveIns0.end(), liveOuts0.begin(),liveOuts0.end(),
        inserter(temp2, temp2.begin()));
    set_difference(temp2.begin(), temp2.end(), depVars.begin(), depVars.end(),
        inserter(firstprivateVars, firstprivateVars.begin()));
    for (std::vector<SgInitializedName*>::iterator iter = firstprivateVars.begin(); iter!= firstprivateVars.end();iter++) 
    {
       attribute->addVariable(OmpSupport::e_firstprivate ,(*iter)->get_name().getString(), *iter);
        if(enable_debug)
          cout<<(*iter)<<" "<<(*iter)->get_qualified_name().getString()<<endl;
    }
  } // end AutoScoping()

  // Recognize reduction variables for a loop
  /* 
   * Algorithms:
   *   for each scalar candidate which are both live-in and live-out for the loop body
   *    and which is not the loop invariant variable.
   *   Consider those with only 1 or 2 references
   *   1 reference
   *     the operation is one of x++, ++x, x--, --x, x binop= expr
   *   2 references belonging to the same operation
   *     operations: one of  x= x op expr,  x = expr op x (except for subtraction)
   * Also according to the specification.
   *  x is not referenced in exp
   *  expr has scalar type (no array, objects etc)
   *  x: scalar only, aggregate types (including arrays), pointer types and reference types may not appear in a reduction clause.
   *  op is not an overloaded operator, but +, *, -, &, ^ ,|, &&, ||
   *  binop is not an overloaded operator but: +, *, -, &, ^ ,| 
   *
   */ 
  std::vector<SgInitializedName*> 
    RecognizeReduction(SgNode *loop, OmpSupport::OmpAttribute* attribute, std::vector<SgInitializedName*>& candidateVars)
    {
      std::vector<SgInitializedName*> *resultVars = new std::vector<SgInitializedName*>;
      ROSE_ASSERT(loop && isSgForStatement(loop)&& attribute);
      if (candidateVars.size()==0) 
        return *resultVars;
      //Store the times of references for each variable
      std::map <SgInitializedName*, vector<SgVarRefExp* > > var_references;

      Rose_STL_Container<SgNode*> reflist = NodeQuery::querySubTree(loop, V_SgVarRefExp);
      Rose_STL_Container<SgNode*>::iterator iter = reflist.begin();
      for (; iter!=reflist.end(); iter++)
      {
        SgVarRefExp* ref_exp = isSgVarRefExp(*iter);
        SgInitializedName* initname= ref_exp->get_symbol()->get_declaration();
        std::vector<SgInitializedName*>::iterator hit= 
          find(candidateVars.begin(), candidateVars.end(), initname);
        if (hit!=candidateVars.end())
        { 
          var_references[initname].push_back(ref_exp);
        }
      }
      //Consider variables referenced at most twice
      std::vector<SgInitializedName*>::iterator niter=candidateVars.begin();
      for (; niter!=candidateVars.end(); niter++)
      {
        SgInitializedName* initname = *niter;
        bool isReduction = false;
        // referenced once only
        if (var_references[initname].size()==1) 
        {
          if(enable_debug)
            cout<<"Debug: A candidate used once:"<<initname->get_name().getString()<<endl;
          SgVarRefExp* ref_exp = *(var_references[initname].begin());
          SgStatement* stmt = SageInterface::getEnclosingStatement(ref_exp); 
          if (isSgExprStatement(stmt))
          {
            SgExpression* exp = isSgExprStatement(stmt)->get_expression();
            if (isSgPlusPlusOp(exp)) // x++ or ++x
            { // Could have multiple reduction clause with different operators!! 
              // So the variable list is associated with each kind of operator
              attribute->addVariable(OmpSupport::e_reduction_plus, initname->get_name().getString(),initname);
              isReduction = true;
            }  
            else if (isSgMinusMinusOp(exp)) // x-- or --x
            { 
              attribute->addVariable(OmpSupport::e_reduction_minus, initname->get_name().getString(),initname);
              isReduction = true;
            } 
            // x binop= expr where binop is one of + * - & ^ |
            // x must be on the left hand side

            SgExpression* binop = isSgBinaryOp(exp);
            if (binop==NULL) continue;
            SgExpression* lhs= isSgBinaryOp(exp)->get_lhs_operand ();
            if (lhs==ref_exp)
            {
              OmpSupport::omp_construct_enum optype;
              switch (exp->variantT())
              {
                case V_SgPlusAssignOp:
                  {
                    optype = OmpSupport::e_reduction_plus;
                    attribute->addVariable(optype,initname->get_name().getString(),initname);
                    isReduction = true;
                    break;
                  }  
                case V_SgMultAssignOp:
                  {
                    optype = OmpSupport::e_reduction_mul;
                    attribute->addVariable(optype,initname->get_name().getString(),initname);
                    isReduction = true;
                    break;
                  }  
                case V_SgMinusAssignOp:
                  {
                    optype = OmpSupport::e_reduction_minus;
                    attribute->addVariable(optype,initname->get_name().getString(),initname);
                    isReduction = true;
                    break;
                  }
                case V_SgAndAssignOp:
                  {
                    optype = OmpSupport::e_reduction_bitand;
                    attribute->addVariable(optype,initname->get_name().getString(),initname);
                    isReduction = true;
                    break;
                  }
                case V_SgXorAssignOp:
                  {
                    optype = OmpSupport::e_reduction_bitxor;
                    attribute->addVariable(optype,initname->get_name().getString(),initname);
                    isReduction = true;
                    break;
                  }
                case V_SgIorAssignOp:
                  {
                    optype = OmpSupport::e_reduction_bitor;
                    attribute->addVariable(optype,initname->get_name().getString(),initname);
                    isReduction = true;
                    break;
                  }
                default:
                  break;
              } // end 
            }// end if on left side  
          } 
        } 
        // referenced twice within a same statement
        else if (var_references[initname].size()==2)
        {
          if(enable_debug)
            cout<<"Debug: A candidate used twice:"<<initname->get_name().getString()<<endl;
          SgVarRefExp* ref_exp1 = *(var_references[initname].begin());
          SgVarRefExp* ref_exp2 = *(++var_references[initname].begin());
          SgStatement* stmt = SageInterface::getEnclosingStatement(ref_exp1);
          SgStatement* stmt2 = SageInterface::getEnclosingStatement(ref_exp2);
          if (stmt != stmt2) 
            continue;
          // must be assignment statement using 
          //  x= x op expr,  x = expr op x (except for subtraction)
          // one reference on left hand, the other on the right hand of assignment expression
          // the right hand uses associative operators +, *, -, &, ^ ,|, &&, ||
          SgExprStatement* exp_stmt =  isSgExprStatement(stmt);
          if (exp_stmt && isSgAssignOp(exp_stmt->get_expression())) 
          {
            SgExpression* assign_lhs=NULL, * assign_rhs =NULL;
            assign_lhs = isSgAssignOp(exp_stmt->get_expression())->get_lhs_operand();
            assign_rhs = isSgAssignOp(exp_stmt->get_expression())->get_rhs_operand();
            ROSE_ASSERT(assign_lhs && assign_rhs);
            // x must show up in both lhs and rhs in any order:
            //  e.g.: ref1 = ref2 op exp or ref2 = ref1 op exp
            if (((assign_lhs==ref_exp1)&&SageInterface::isAncestor(assign_rhs,ref_exp2))
                ||((assign_lhs==ref_exp2)&&SageInterface::isAncestor(assign_rhs,ref_exp1)))
            {
              // assignment's rhs must match the associative binary operations
              // +, *, -, &, ^ ,|, &&, ||
              SgBinaryOp * binop = isSgBinaryOp(assign_rhs);
              if (binop!=NULL){
                SgExpression* op_lhs = binop->get_lhs_operand();
                SgExpression* op_rhs = binop->get_rhs_operand();
                // double check that the binary expression has either ref1 or ref2 as one operand 
                if( !((op_lhs==ref_exp1)||(op_lhs==ref_exp2)) 
                    && !((op_rhs==ref_exp1)||(op_rhs==ref_exp2)))
                  continue;
                bool isOnLeft = false; // true if it has form (refx op exp), instead (exp or refx)
                if ((op_lhs==ref_exp1)||   // TODO might have in between !!
                    (op_lhs==ref_exp2))
                  isOnLeft = true;
                OmpSupport::omp_construct_enum optype;
                switch (binop->variantT())
                {
                  case V_SgAddOp:
                    {
                      optype = OmpSupport::e_reduction_plus;  
                      attribute->addVariable(optype,initname->get_name().getString(),initname);
                      isReduction = true;
                      break;
                    }  
                  case V_SgMultiplyOp:
                    {
                      optype = OmpSupport::e_reduction_mul;
                      attribute->addVariable(optype,initname->get_name().getString(),initname);
                      isReduction = true;
                      break;
                    }  
                  case V_SgSubtractOp: // special handle here!!
                    {
                      optype = OmpSupport::e_reduction_minus;
                      if (isOnLeft) // cannot allow (exp - x)a
                      {  
                        attribute->addVariable(optype,initname->get_name().getString(),initname);
                        isReduction = true;
                      }
                      break;
                    }  
                  case V_SgBitAndOp:
                    {
                      optype = OmpSupport::e_reduction_bitand;
                      attribute->addVariable(optype,initname->get_name().getString(),initname);
                      isReduction = true;
                      break;
                    }  
                  case V_SgBitXorOp:
                    {
                      optype = OmpSupport::e_reduction_bitxor;
                      attribute->addVariable(optype,initname->get_name().getString(),initname);
                      isReduction = true;
                      break;
                    }  
                  case V_SgBitOrOp:
                    {
                      optype = OmpSupport::e_reduction_bitor;
                      attribute->addVariable(optype,initname->get_name().getString(),initname);
                      isReduction = true;
                      break;
                    } 
                  case V_SgAndOp:
                    { 
                      optype = OmpSupport::e_reduction_logand;
                      attribute->addVariable(optype,initname->get_name().getString(),initname);
                      isReduction = true;
                      break;
                    }  
                  case V_SgOrOp:
                    {
                      optype = OmpSupport::e_reduction_logor;
                      attribute->addVariable(optype,initname->get_name().getString(),initname);
                      isReduction = true;
                      break;
                    }  
                  default:
                    break;
                }  
              } // end matching associative operations
            }  
          } // end if assignop  
        }// end referenced twice
        if (isReduction)
          resultVars->push_back(initname);
      }// end for ()  
      return *resultVars;
    } // end RecognizeReduction()

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
    //reduction is a little different: may have multiple reduction clauses for 
    // different reduction operators
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
  void DependenceElimination(SgNode* sg_node, LoopTreeDepGraph* depgraph, std::vector<DepInfo>& remainings, OmpSupport::OmpAttribute* att, 
        std::map<SgNode*, bool> &  indirect_table, ArrayInterface* array_interface/*=0*/, ArrayAnnotation* annot/*=0*/)
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
          // cout<<"Debug: dependence edge: "<<e->toString()<<endl;
          DepInfo info =e->GetInfo();

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
#if 1           
          // x. Eliminate dependence relationship if one of the pair is thread local
          // -----------------------------------------------
          // either of the source or sink variables are thread-local: 
          // (within the scope of the loop's scope)
          // There is no loop carried dependence in this case 
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
#endif
          //x. Eliminate a dependence if it is empty entry
          // -----------------------------------------------
          // Ignore possible empty depInfo entry
          if (src_node==NULL||snk_node==NULL)
            continue;

          //x. Eliminate a dependence if scalar type dependence involving array references.
          // -----------------------------------------------
          // At least one of the source and sink variables are array references (not scalar) 
          // But the dependence type is scalar type
          //   * array-to-array, but scalar type dependence
          //   * scalar-to-array dependence.  
          //    We essentially assume no aliasing between arrays and scalars here!!
          //    I cannot think of a case in which a scalar and array element can access the same memory location otherwise.
          // According to Qing:  
          //   A scalar dep is simply the dependence between two scalar variables.
          //   There is no dependence between a scalar variable and an array variable. 
          //   The GlobalDep function simply computes dependences between two scalar 
          //   variable references (to the same variable)
          //   inside a loop, and the scalar variable is not considered private.
          // We have autoscoping to take care of scalars, so we can safely skip them 
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
          //if (isArray1 && isArray2) // changed from both to either to be aggressive, 5/25/2010
          if (isArray1 || isArray2)
          {
            if ((info.GetDepType() & DEPTYPE_SCALAR)||(info.GetDepType() & DEPTYPE_BACKSCALAR))
              continue;
          }
          //x. Eliminate dependencies caused by autoscoped variables
          // -----------------------------------------------
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

          //x. Eliminate dependencies caused by a pair of indirect indexed array reference,
          // -----------------------------------------------
          //   if users provide the semantics that all indirect indexed array references have 
          //   unique element accesses (via -rose:autopar:unique_indirect_index )
          //   Since each iteration will access a unique element of the array, no loop carried data dependences
          //  Lookup the table, rule out a dependence relationship if both source and sink are one of the unique array reference expressions.
          //  AND both references to the same array symbol , and uses the same index variable!!
           if (b_unique_indirect_index ) 
           { 
             if (indirect_table[src_node] && indirect_table[snk_node])
               continue;
           }
          // x. Eliminate loop-independent dependencies: 
          // -----------------------------------------------
          // loop independent dependencies: privatization can eliminate most of them
          if (info.CarryLevel()!=0) 
            continue;
          // Save the rest dependences which can not be ruled out 
          remainings.push_back(info); 
        } //end iterator edges for a node
      } // end if has edge
    } // end of iterate dependence graph 
  }// end DependenceElimination()

/*
 Uniforming multiple forms of array indirect accessing into a single form: 
        arrayX[arrayY...[loop_index]]

  Common forms of array references using indirect indexing: 
 
  Form 1:  naive one (the normalized/uniformed form)
        ... array_X [array_Y [current_loop_index]] ..

  Form 2:  used more often in real code from Jeff
       indirect_loop_index  = array_Y [current_loop_index] ;
        ...  array_X[indirect_loop_index] ...

  Cases of multiple dimensions, multiple levels of indirections are also handled.  

  We uniform them into a single form (Form 2) to simplify later recognition of indirect indexed array refs
   For Form 2: if the rhs operand is a variable
      find the reaching definition of the index based on data flow analysis
        case 1: if it is the current loop's index variable, nothing to do further. stop
        case 2: outside the current loop's scope: for example higher level loop's index. stop
        case 3: the definition is within the  current loop ?  
                 replace the rhs operand with its reaching definition's right hand value.
                one assignment to another array with the current  (?? higher level is considered at its own level) loop index
 
Algorithm: Replace the index variable with its right hand value of its reaching definition,
           or if the definition 's scope is within the current  loop's body   

*/
 static void uniformIndirectIndexedArrayRefs (SgForStatement* for_loop)
 {
   ROSE_ASSERT (for_loop != NULL);
   ROSE_ASSERT (for_loop->get_loop_body() != NULL);
   SgInitializedName * loop_index_name = NULL;
   bool isCanonical = SageInterface::isCanonicalForLoop (for_loop, &loop_index_name);
   ROSE_ASSERT (isCanonical == true);

   // prepare def/use analysis, it should already exist as part of initialize_analysis()
   //SgProject * project = getProject();
   ROSE_ASSERT (defuse != NULL);  
   Rose_STL_Container <SgNode* > nodeList = NodeQuery::querySubTree(for_loop->get_loop_body(), V_SgPntrArrRefExp);
   for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i != nodeList.end(); i++)
   {
     SgPntrArrRefExp *aRef = isSgPntrArrRefExp((*i));
     ROSE_ASSERT (aRef != NULL); 
     SgExpression* rhs = aRef-> get_rhs_operand_i();
     switch (rhs->variantT())
     {
       case V_SgVarRefExp:
         {
           // SgVarRefExp * varRef = isSgVarRefExp(rhs);
           // trace back to the 'root' value of rhs according to def/use analysis
           // Initialize the end value to the current rhs of the array reference expression
           SgExpression * the_end_value = rhs; 
           while (isSgVarRefExp(the_end_value))
           {
             SgVarRefExp * varRef = isSgVarRefExp(the_end_value);
             SgInitializedName * initName = isSgInitializedName(varRef->get_symbol()->get_declaration());
             ROSE_ASSERT (initName != NULL);
             // stop tracing if it is already the current loop's index
             if (initName  == loop_index_name) break;

             // get the reaching definitions of the variable
             vector <SgNode* > vec = defuse ->getDefFor (varRef, initName);
             if (vec.size() == 0)
             {
               cerr<<"Warning: cannot find a reaching definition for an initialized name:"<<endl;
               cerr<<"initName:"<<initName->get_name().getString()<<"@";
               cerr<<varRef->get_file_info()->get_line()<<":"<< varRef->get_file_info()->get_col() <<endl;
               // ROSE_ASSERT (vec.size()>0);
               break; 
             }

             // stop tracing if there are more than one reaching definitions
             if (vec.size()>1) break;

             // stop if the defining statement is out side of the scope of the loop body
             SgStatement* def_stmt = SageInterface::getEnclosingStatement(vec[0]);
             if  (! SageInterface::isAncestor(for_loop->get_loop_body(), def_stmt)) 
               break;

             // now get the end value depending on the definition node's type
             if (isSgAssignOp(vec[0]))
               the_end_value = isSgAssignOp(vec[0])->get_rhs_operand_i();
             else if (isSgAssignInitializer(vec[0]))
             {
               the_end_value = isSgAssignInitializer(vec[0])->get_operand_i();
             }
             else
             {  
               if (!isSgMinusMinusOp(vec[0])) // (! && !)
               {
                 cerr<<"Warning: uniformIndirectIndexedArrayRefs() ignoring a reaching definition of a type: "
                   << vec[0]->class_name()<<"@";
                 if (isSgLocatedNode(vec[0]))
                 {
                   SgLocatedNode* lnode = isSgLocatedNode(vec[0]);
                   cerr<<lnode->get_file_info()->get_line()<<":"<< lnode->get_file_info()->get_col() ;
                 }
                 cerr<<endl;
               }
               //ROSE_ASSERT(false);
               break;
             }
           } // end while() to trace down to root definition expression
           //replace rhs with its root value if rhs != end_value
           if (rhs != the_end_value)
           {
             SgExpression* new_rhs = SageInterface::deepCopy<SgExpression> (the_end_value);
             //TODO use replaceExpression() instead
             aRef->set_rhs_operand_i(new_rhs);
             new_rhs->set_parent(aRef);
             delete rhs; 
           }

           break;
         } // end case V_SgVarRefExp:
       case V_SgPntrArrRefExp: // uniform form already, do nothing
       case V_SgIntVal: // element access using number, do nothing
         // ignore array index arithmetics 
         // since we narrow down the simplest case for indirection without additional calculation
       case V_SgSubtractOp:
       case V_SgAddOp:
       case V_SgMinusMinusOp:
       case V_SgPlusPlusOp:
       case V_SgModOp:
       case V_SgMultiplyOp:
         break;
       default:
         {
           cerr<<"Warning: uniformIndirectIndexedArrayRefs(): ignoring an array access expression type: "<< rhs->class_name()<<endl;
           break;
         }
     } // end switch
   } //end for

 }
  /* Check if an array reference expression is an indirect indexed with respect to a loop
   * This function should be called after all array references are uniformed already.
   *
  Algorithm:  
    find all array variables within the loop in consideration
    for each array variable, do the following to tell if such an array is accessed via an indirect index
        SgPntrArrRefExp  :
            lhs_operatnd: SgVarRefExp, SgVariableSymbol, SgInitializedName  SgArrayType
            rhs_operand: SgVarRefExp, SgVariableSymbol, SgInitializedName, i
        Check a lookup table to see if this kind of reference is already recognized
              two keys: array symbol, index expression, bool
        if not, do the actual pattern recognition
            // in a function  
            if is another array reference
            Found an array reference using indirect index,
         store it in a look up table : SySymbol (array being accessed ) , index Ref Exp, true/false 
   */

  static bool isIndirectIndexedArrayRef (SgForStatement* for_loop, SgPntrArrRefExp *aRef)
  {
    bool rtval = false;
    ROSE_ASSERT (for_loop != NULL);
    ROSE_ASSERT (aRef != NULL);
    // grab the loop index variable
    SgInitializedName * loop_index_name = NULL;
    bool isCanonical = SageInterface::isCanonicalForLoop (for_loop, &loop_index_name);
    bool hasIndirecting = false;
    ROSE_ASSERT (isCanonical == true);

    // grab the array index  from arrayX[arrayY...[loop_index]]
    SgPntrArrRefExp* innerMostArrExp =  aRef;
    while (isSgPntrArrRefExp(innerMostArrExp->get_rhs_operand_i()))
    {
       innerMostArrExp = isSgPntrArrRefExp(innerMostArrExp->get_rhs_operand_i());
       hasIndirecting = true;
    }

    SgExpression* array_index_exp = innerMostArrExp->get_rhs_operand_i();

    switch (array_index_exp->variantT() )
    {
      case V_SgPntrArrRefExp: 
        {
          cerr<<"Error: isIndirectIndexedArrayRef(). inner most loop index should not be of an array type anymore! "<<endl;
          ROSE_ASSERT (false);
          break;
        }
      case V_SgVarRefExp: 
        {
          SgVarRefExp * varRef = isSgVarRefExp(array_index_exp);
          // We only concern about the indirection based on the current loop's loop index variable
          // since we consider all loop levels one by one
          if (hasIndirecting && (varRef->get_symbol()->get_declaration() == loop_index_name))
            rtval = true; 
          break;
        }
      case V_SgIntVal: 
        // ignore array index arithmetics 
        // since we narrow down the simplest case for indirection without additional calculation
      case V_SgSubtractOp:
      case V_SgAddOp:
      case V_SgPlusPlusOp:
      case V_SgMultiplyOp:
         break;
      default:
        // This should not matter. We output something anyway for improvements.
        cerr<<"Warning: isIndirectIndexedArrayRef(): unhandled array index type: "<< array_index_exp->class_name()<<endl;
      //  ROSE_ASSERT (false);
        break;
    }

    return rtval;
  }
  
  // collect array references with indirect indexing within a loop, save the result in a lookup table
  /*
  Algorithm:  
    find all array variables within the loop in consideration
    for each array variable, do the following to tell if such an array is accessed via an indirect index
       Check a lookup table to see if this kind of reference is already recognized
              two keys: array symbol, index expression, bool
        if not, do the actual pattern recognition
           Found an array reference using indirect index,
         store it in a look up table : SySymbol (array being accessed ) , index Ref Exp, true/false 
   */
  static void collectIndirectIndexedArrayReferences(SgNode* loop,  std::map<SgNode*, bool>& indirect_array_table)
  {
    ROSE_ASSERT (loop != NULL);
    SgForStatement* for_loop = isSgForStatement(loop);
    ROSE_ASSERT (for_loop != NULL);

    Rose_STL_Container <SgNode* > nodeList = NodeQuery::querySubTree(for_loop->get_loop_body(), V_SgPntrArrRefExp); 
    for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i != nodeList.end(); i++)
    {
      SgPntrArrRefExp *aRef = isSgPntrArrRefExp((*i));
      if (isIndirectIndexedArrayRef(for_loop, aRef))
      {
        indirect_array_table[aRef] = true; 
       // cout<<"Found an indirect indexed array ref:"<<aRef->unparseToString()
       // << "@" << aRef <<endl;
      }
    }
  }

  bool ParallelizeOutermostLoop(SgNode* loop, ArrayInterface* array_interface, ArrayAnnotation* annot)
  {
    ROSE_ASSERT(loop&& array_interface && annot);
    ROSE_ASSERT(isSgForStatement(loop));
    bool isParallelizable = true;
   

    // collect array references with indirect indexing within a loop, save the result in a lookup table
    // This work is context sensitive (depending on the outer loops), so we declare the table for each loop.
    std::map<SgNode*, bool> indirect_array_table;
   if (b_unique_indirect_index) // uniform and collect indirect indexed array only when needed
   {
    // uniform array reference expressions
    uniformIndirectIndexedArrayRefs(isSgForStatement(loop));
    collectIndirectIndexedArrayReferences (loop, indirect_array_table);
   }
    // X. Compute dependence graph for the target loop
    SgNode* sg_node = loop;
    LoopTreeDepGraph* depgraph= ComputeDependenceGraph(sg_node, array_interface, annot);
    if (depgraph==NULL)
    {
      cout<<"Warning: skipping a loop since failed to compute depgraph for it:"<<sg_node->unparseToString()<<endl;
      return false;
    }

    // X. Variable classification (autoscoping): 
    // This step is done before DependenceElimination(), so the irrelevant
    // dependencies associated with the autoscoped variabled can be
    // eliminated.
    //OmpSupport::OmpAttribute* omp_attribute = new OmpSupport::OmpAttribute();
    OmpSupport::OmpAttribute* omp_attribute = buildOmpAttribute(e_unknown, NULL, false);
    ROSE_ASSERT(omp_attribute != NULL);
    AutoScoping(sg_node, omp_attribute,depgraph);

    //X. Eliminate irrelevant dependence relations.
    vector<DepInfo>  remainingDependences;
    DependenceElimination(sg_node, depgraph, remainingDependences,omp_attribute, indirect_array_table,  array_interface, annot);
    if (remainingDependences.size()>0)
    {
      isParallelizable = false;
      cout<<"\nUnparallelizable loop at line:"<<sg_node->get_file_info()->get_line()<<
        " due to the following dependencies:"<<endl;
      for (vector<DepInfo>::iterator iter= remainingDependences.begin();     
          iter != remainingDependences.end(); iter ++ )
      {
        cout<<(*iter).toString()<<endl;
      }
    }
    else
    {
      cout<<"\nAutomatically parallelized a loop at line:"<<sg_node->get_file_info()->get_line()<<endl;
    }

    // comp.DetachDepGraph();// TODO release resources here
    //X.  Attach OmpAttribute to the loop node if it is parallelizable 
    if (isParallelizable)
    {
      //= OmpSupport::buildOmpAttribute(OmpSupport::e_parallel_for,sg_node);
      omp_attribute->setOmpDirectiveType(OmpSupport::e_parallel_for);
      //cout<<"debug autoParSupport.C attaching att to sg_node "<<sg_node<<endl;
      //cout<<"at line "<<isSgLocatedNode(sg_node)->get_file_info()->get_line()<<endl;
      OmpSupport::addOmpAttribute(omp_attribute,sg_node);
      // 6. Generate and insert #pragma omp parallel for 
      // Liao, 2/12/2010
      // In the enable_diff mode, we don't want to generate pragmas from compiler-generated OmpAttribute.
      // Comparing OmpAttributes from both sources is enough
      if (! enable_diff) 
        OmpSupport::generatePragmaFromOmpAttribute(sg_node);
    }
    else
    {
      delete omp_attribute;
    }
    return isParallelizable;
  }

  // Generate a normal patch file representing the addition of OpenMP pragmas
  // An example patch file may contain:
  // diff -ar /home/liao6/desktop/keywords/patch/project1/sub1/file3.c rose_file3.c
  // 0a1
  // > #include <omp.h>
  // 4a5
  // > #pragma omp parallel for
  // This is called only after checking if sfile uses OpenMP
  void generatePatchFile(SgSourceFile* sfile)
  {
    ROSE_ASSERT (sfile != NULL);
    std::string filename =  sfile->get_file_info()->get_filenameString();
    std::string diff_header = "diff -ar "+ filename; 
    // this output file name can be any file name, we just choose rose_file.c
    string ofilename= "rose_"+StringUtility::stripPathFromFileName(filename); 
    diff_header += " "+ ofilename+"\n";

    //debug only
    //cout<<"diff_header\n"<<diff_header<<endl;

    //always insert omp.h 
    string patchContent="0a1\n> #include <omp.h>\n";

    // now accumulate diff text for each OmpAttribute
    Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(sfile, V_SgStatement);
    for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i != nodeList.end(); i++)
    {
      patchContent += OmpSupport::generateDiffTextFromOmpAttribute (*i);
    }

    //cout<<"patch content is\n"<<patchContent<<endl;
    string patch_file_name = StringUtility::stripPathFromFileName(filename)+".patch";
    ofstream patchFile (patch_file_name.c_str(), ios::out);
    patchFile <<diff_header << patchContent ;
  }
  //! Output the difference between user-defined OpenMP and compiler-generated OpenMP
  /*
   AST layout for a loop with both user-defined and compiler-introduced OmpAttribute
       SgPragmaDeclaration .. OmpAttribute 
       SgForStatement    .. OmpAttribute

    Algorithm
    * for each OmpAttribute, if it is not processed
      check if it is processed:  map [attribute, processedFlag]
    ** if it is attached to pragma: then it is user defined
        search for a possible autoPar generated OmpAttributes
            find affected statement,
             if it is a loop, find possible attached OmpAttributes, mark it as processed
            diff the output:
    ** if attached to loop:  autoPar generated
        search for a possible user defined attributes
          find preceding pragma--> attrbiute
        diff them
   */
  void diffUserDefinedAndCompilerGeneratedOpenMP(SgSourceFile* sfile)
  {
    // A table to store processed attributes
    std::map <OmpAttribute*, bool> attributeTable; 
    Rose_STL_Container <SgNode* > nodeList = NodeQuery::querySubTree(sfile, V_SgStatement);
    for (Rose_STL_Container<SgNode *>::iterator i1 = nodeList.begin(); i1 != nodeList.end(); i1++)
    {
      bool isUserDefined = false; 
      SgStatement * stmt = isSgStatement(*i1);
      ROSE_ASSERT (stmt != NULL);

      OmpAttributeList* oattlist= getOmpAttributeList(stmt);
      if (oattlist == NULL) continue ;
      // we attach user-defined OmpAttribute to SgPragmaDeclaration
      if (isSgPragmaDeclaration(stmt))
        isUserDefined = true;

      vector <OmpAttribute* > ompattlist = oattlist->ompAttriList;
      ROSE_ASSERT (ompattlist.size() != 0) ;
      vector <OmpAttribute* >::iterator i2 = ompattlist.begin();
      for (; i2!=ompattlist.end(); i2++)
      {
        OmpAttribute* oa = *i2;
        if (attributeTable[oa])
           continue; // processed already , used as one of the pair being compared
        else 
          attributeTable[oa] = true; // tag it as being processed
       std::string user_pragma_str, compiler_pragma_str;   
       OmpAttribute* user_attr = NULL, * compiler_attr =NULL;
       // user defined, try to grab a compiler generated attributed attached to the affected loop, etc.
        if (isUserDefined)
        {
          user_attr = oa; 
          user_pragma_str  = oa->toOpenMPString();
          SgStatement* next_stmt = SageInterface::getNextStatement(stmt);
          // TODO we currently only auto-generate pragmas for loops, 
          // we should extend the type to others later on
          if (next_stmt &&isSgForStatement(next_stmt) )
          {
             OmpAttributeList* next_attlist = getOmpAttributeList (next_stmt);
             if (next_attlist!= NULL)
             {
               vector <OmpAttribute* > ompattlist2 = next_attlist->ompAttriList;
               // there should could be more than one OmpAttribute attached 
               // To facilitate outlining a loop with user defined pragma, we redundantly attach OmpAttribute
               // to both the pragma and the affected loop
                 //cout<<"Warning: found a loop attached with multiple OmpAttribute s"<<endl;
                 //cout<<"memory address:"<<next_stmt<<endl;
                // cout<<next_stmt->get_file_info()->get_line()<<endl;
                  vector <OmpAttribute* >::iterator i3 = ompattlist2.begin();
                  OmpAttribute* theone = NULL;
                  for (; i3!=ompattlist2.end(); i3++)
                  {
                    //cout<<(*i3)->toOpenMPString()<<endl;
                    if (!(*i3)->get_isUserDefined())
                    {
                      theone = *i3;
                      break;
                    }
                  } 
               //ROSE_ASSERT (ompattlist2.size() == 1) ; 
               compiler_pragma_str = theone->toOpenMPString(); 
               compiler_attr = theone;
               attributeTable[theone] = true; // tag the counterpart as processed also
             }
          }
        }
        // compiler-generated attribute, find a possible user-introduced pragma if it exists
        else
        {
          compiler_pragma_str = oa->toOpenMPString();
          compiler_attr = oa; 
          SgStatement* prev_stmt = SageInterface::getPreviousStatement(stmt);
          if (prev_stmt)
          {
            SgPragmaDeclaration * prev_pragma = isSgPragmaDeclaration(prev_stmt);
            if (prev_pragma)
            {
              OmpAttributeList* prev_attlist= getOmpAttributeList(prev_pragma);
              if (prev_attlist)
              {
                vector <OmpAttribute* > ompattlist2 = prev_attlist->ompAttriList;
                // there should be only one omp attribute attached to pragma
                ROSE_ASSERT (ompattlist2.size() == 1) ;
                user_pragma_str = ompattlist2[0]->toOpenMPString();
                user_attr = ompattlist2[0];
                attributeTable[ompattlist2[0]] = true; // tag the counterpart as processed also
              }
            }
          }
        }
        // diff them and report the difference
        //  <<<<<<<<
        //  user defined pragma
        //  --------
        //  compiler generated pragma
        //  >>>>>>>>
        Sg_File_Info * file_info = stmt->get_file_info();  
        if (user_pragma_str.size()!=0)
          user_pragma_str = "#pragma omp "+user_pragma_str;
        if (compiler_pragma_str.size()!=0)
          compiler_pragma_str = "#pragma omp "+compiler_pragma_str;
        //if (user_pragma_str != compiler_pragma_str)
        if (!isEquivalentOmpAttribute(user_attr, compiler_attr))
         {
            cout<<"<<<<<<<<"<<endl;
           if (isUserDefined)
           {
             cout<<file_info->get_filename()<<":"<<file_info->get_line()<<endl;
             cout<<"user defined      :";
             cout<<user_pragma_str<<endl;
             cout<<"--------"<<endl;
             cout<<"compiler generated:";
             cout<<compiler_pragma_str<<endl;
             }
           else
           {
             cout<<"user defined      :";
             cout<<user_pragma_str<<endl;
             cout<<"--------"<<endl;
             cout<<file_info->get_filename()<<":"<<file_info->get_line()<<endl;
             cout<<"compiler generated:";
             cout<<compiler_pragma_str<<endl;
           }
           cout<<">>>>>>>>"<<endl<<endl;
         } 
      } // end for omp attribute within a att list   

    } // end for (stmt)

  } //end diffUserDefinedAndCompilerGeneratedOpenMP()

} // end namespace
