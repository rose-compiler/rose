// tps : Switching from rose.h to sage3 changed size from 22,7 MB to 12,4MB
#include "sage3basic.h"

#include "DependenceGraph.h"

#include "DominatorTree.h"
#include <set>
#include <iostream>

#include "filteredCFG.h"
#include "DominatorTree.h"
#include "DominanceFrontier.h"
#include "DefUseExtension.h"

using namespace DominatorTreesAndDominanceFrontiers;
using namespace std;

ControlDependenceGraph::ControlDependenceGraph(SgFunctionDefinition * functionStart, InterproceduralInfo * ii):
  source(SliceCFGNode(functionStart->cfgForBeginning())),
  sink(SliceCFGNode(functionStart->cfgForEnd())),
  dominatorTree(functionStart,POST_DOMINATOR)   , /*
                                                    dominanceFrontier(dominatorTree)*/
  head(functionStart)
{
  // store all function calls, they are needed often
  functionCalls = NodeQuery::querySubTree(head, V_SgFunctionCallExp);
    
  // create the control dependency graph
  buildCDG();
  createSyntacticDependencies();
  // after the construction, add the FORMAL parameters to the entry-node
  // enty to formal out (return)
  //            establishEdge(getNode(DependenceNode::ENTRY,functionStart),getNode(DependenceNode::FORMALRETURN,functionStart->get_declaration()),CONTROL);
  establishEdge(getNode(DependenceNode::ENTRY,functionStart),getNode(DependenceNode::FORMALRETURN,functionStart->get_declaration()),BELONGS_TO);
  // and give this node an explcit name, looks nicer in the graphs
  //            getNode(DependenceNode::FORMALRETURN,functionStart->get_declaration())->setName(std::string("RETURN"));
  // for all initialized parameters
  Rose_STL_Container<SgInitializedName*> argList=functionStart->get_declaration()->get_args();
  for (Rose_STL_Container<SgInitializedName*>::iterator i=argList.begin();i!=argList.end();i++)
    {
      //is the paremeter a elipsis, if so, continue....
      if (isSgTypeEllipse((*i)->get_type()))
        {
          cerr<<"Warning: Ellipsis found, NOT SUPPORTET"<<endl<<__LINE__<< " of "<<__FILE__<<endl;
          continue;
        }

      // the formal in parameters completely depend of the entry, since wihtout it, there are no parameters
      establishEdge(getNode(DependenceNode::ENTRY,functionStart),getNode(DependenceNode::FORMALIN,*i),CONTROL);
      establishEdge(getNode(DependenceNode::ENTRY,functionStart),getNode(DependenceNode::FORMALOUT,*i),CONTROL);
    }
  //      // create an sysntactic edge, this parameter is required to syntactically comple the call
  //    establishEdge(getNode(DependenceNode::FORMALIN,*i),getNode(DependenceNode::ENTRY,functionStart),SYNTACTIC);

                
  std::ofstream f("cfg.dot");
  cfgToDot(f,string("cfg"),source);
  f.close();
  dominatorTree.writeDot((char*)"dt.dot");
     

  // set internal parameters 
  decl=functionStart->get_declaration();
  def=functionStart;
}

void ControlDependenceGraph::createSyntacticDependencies()
{
  DependenceNode *source,*sink;
  // for each goto
  Rose_STL_Container< SgNode * >gotoStatemnts = NodeQuery::querySubTree(head, V_SgGotoStatement);
  for (Rose_STL_Container< SgNode * >::iterator  i=gotoStatemnts.begin();i!=gotoStatemnts.end();i++)
    {
      // get the goto and the labes as dependence node
      source=getNode(DUVariableAnalysisExt::getNextParentInterstingNode(*i));
      sink=getNode(DUVariableAnalysisExt::getNextParentInterstingNode(isSgGotoStatement(*i)->get_label ()));
      //    establishEdge(sink,source,SYNTACTIC);
      establishEdge(sink,source,CONTROL);
    }
  /* break and continue can handler locally
     list < SgNode * >breakStatemnts = NodeQuery::querySubTree(head, V_SgBreakStmt);
     for (list < SgNode * >::iterator  i=breakStatemnts.begin();i!=breakStatemnts.end();i++)
     {
     source=getNode(DUVariableAnalysisExt::getNextParentInterstingNode(*i));

     }
     list < SgNode * >continueStatements = NodeQuery::querySubTree(head, V_SgContinueStmt);
     for (list < SgNode * >::iterator  i=continueStatements.begin();i!=continueStatements.end();i++)
     {
     source=getNode(DUVariableAnalysisExt::getNextParentInterstingNode(*i));
     }
  */
  // for each break
  // for each continue

}

void ControlDependenceGraph::addDependence(int aID,int bID,EdgeType type)
{
  SgNode * a,*b;
  a=dominatorTree.getCFGNodeFromID(aID).getNode();
  b=dominatorTree.getCFGNodeFromID(bID).getNode();
  DependenceNode *depNA,*depNB;
  //    cout << bID<<"("<<b->unparseToString()<<")->"<<aID<<"("<<b->unparseToString()<<")"<<endl;
  // this is probably not a good style, but this is the only place where the DependenceNodes are createated by using getNode. The source node is a specieal node and should be attributed as ENTRY. Sice the current graph structure does not allow to manipulate a node after it hase bee created, this has to be done on creation. def is the function definition node and the entry point for the function
  depNA=depNB=NULL;
  if (source==dominatorTree.getCFGNodeFromID(bID))
    {
      depNB=getNode(DependenceNode::ENTRY,b);
                
    }

  // if the a-node (direct child of the function definition) is a initialized name and its parent is the function definition)
  if (isSgFunctionParameterList(a->get_parent()) && isSgFunctionDeclaration(a->get_parent()->get_parent()))
    depNA=getNode(DependenceNode::FORMALIN,a);
        
  if (depNA==NULL) depNA=getNode(a);
  if (depNB==NULL) depNB=getNode(b);
  
  SgNode * depNode=depNA->getSgNode();
  if (isSgBreakStmt(depNode) ||
      isSgContinueStmt(depNode))
    {
      if (debugme)
        cout <<"control stmt found"<<endl;
      // his a explicit control changing node
      // use the SYNTACTIC EDGE to enforce a backwrad dependency
      //establishEdge(depNA,depNB,SYNTACTIC);
      establishEdge(depNA,depNB,CONTROL);
    }
  establishEdge(depNB,depNA);

}

void ControlDependenceGraph::processDependence(int aID,int bID)
{
}


stack<SliceCFGNode> L;
set<SliceCFGNode> T;
map<SliceCFGNode,int> dfsnum,low;
int N;

void dfsVisit(SliceCFGNode p)
{
  L.push(p);
  dfsnum[p]=N;
  N++;
  low[p]=dfsnum[p];
  std::vector<SliceCFGEdge> edges=p.outEdges();
  for (unsigned int childNr=0;childNr<edges.size();childNr++)
    {
      SliceCFGNode q=edges[childNr].target();
      if (!T.count(q))
        {
          T.insert(q);
          dfsVisit(q);
          low[p]=min(low[p],low[q]);                            
        }
      else
        {
          low[p]=min(low[p],dfsnum[q]);
        }                       
    }   
  if (low[p]==dfsnum[p])
    {
      //        cout<<"component: "<<p.getNode()->unparseToString()<<endl;
      while(L.top()!=p)
        {
          SliceCFGNode v=L.top();
          L.pop();
          //cout <<"\t"<<v.getNode()->unparseToString()<<endl;
        }
      L.pop();
    }   
}

void articualtionPoints(SliceCFGNode p)
{
}


void ControlDependenceGraph::buildCDG()
{
  // get all strong connected regions and determine dependences from there...
  T.insert(source);
  dfsVisit(source);

  //For details about the algorithm see: J. Ferrante & K. Ottenstein: The Program Dependence Graph and Its use in Opimisation
  int aID,bID;
  int leastCommonDominator;
  // for all nodes
  for (aID=0;aID<dominatorTree.getSize();aID++)
    {
      // get out edges for this node
      SliceCFGNode a=dominatorTree.getCFGNodeFromID(aID);
      std::vector<SliceCFGEdge> edges=a.outEdges();
      for (unsigned int j=0;j<edges.size();j++)
        {
          SliceCFGNode b=edges[j].target();
          bID=dominatorTree.getID(b);
          //            processDependence(aID,bID);
          if (!dominatorTree.dominates(bID,aID))
            {
              // calculate the least common dominator
              if (aID==0) leastCommonDominator=0;
              // lcd is either A or imdom(A)
              if (dominatorTree.dominates(aID,bID))
                {
                  // case 2 on page 325
                  leastCommonDominator=aID;// a dominates b -> lcd is A
                  // now attribute all nodes on the path from B to lCD as beubg deoebdebt on a
                  for (int current=bID;current!=leastCommonDominator;current=dominatorTree.getImDomID(current))
                    {
                      // mark as dependent on A
                      addDependence(current,aID);
                      //                                cout <<aID<<"->"<< current<<endl;
                    }
                  addDependence(aID,aID);
                  //                    cout <<aID<<"->"<<aID<<endl;                            
                }
              else
                {
                  leastCommonDominator=dominatorTree.getImDomID(aID);
                  //case 2 on page 325
                  for (int current=bID;current!=leastCommonDominator;current=dominatorTree.getImDomID(current))
                    {
                      // mark as dependent on A
                      addDependence(current,aID);
                      //                                cout <<aID<<"->"<< current<<endl;
                    }
                }
            }                   
        }
    }
  // J. Ferrante & K. Ottenstein added addition edges to the cfg, which I did not do. To account for this, the dependence between the source and the sink have to be processed
  // The entry-node is post-dominated by the sink, therefore the least common deminator is the sink
  // do a special pass for that edge.. , basically this is the imDom path from the source to the sink without source and sink
  aID=dominatorTree.getID(source);
  bID=dominatorTree.getID(sink);
  leastCommonDominator=bID;
  //case 2 on page 325
  for (int current=dominatorTree.getImDomID(aID);current!=leastCommonDominator;current=dominatorTree.getImDomID(current))
    {
      // mark as dependent on A
      addDependence(current,aID);
    }
  //cout<<"Source to string" << source.getNode()->unparseToString()<<endl;      
}


// adds ACTUAL-nodes to each function call
void ControlDependenceGraph::computeAdditionalFunctioncallDepencencies()
{
  for (Rose_STL_Container< SgNode * >::iterator  i=functionCalls.begin();i!=functionCalls.end();i++)
  {

    SgFunctionCallExp * call=isSgFunctionCallExp(*i);
    // get the next interesting node
    //SgNode * interestingNode=DUVariableAnalysisExt::getNextParentInterstingNode(*i);
    //ROSE_ASSERT(interestingNode!=NULL);
    
    //if (isSgStatement(interestingNode->get_parent())) interestingNode=interestingNode->get_parent();
    // add actual out (return-value)  edge                      
    
    establishEdge(getNode(call),getNode(DependenceNode::ACTUALRETURN,call),DependenceGraph::BELONGS_TO);
    getNode(DependenceNode::ACTUALRETURN,call)->setName(std::string("RETURN"));
    //for every parameter in the calls SgExpListExpr
    Rose_STL_Container<SgExpression*> params=call->get_args()->get_expressions();
    for (Rose_STL_Container<SgExpression*>::iterator j=params.begin();j!=params.end();j++)
    {
      // and establish an edge, since the depencencies for thoese nodes depende to the dependency of the parent node and parameters and subexpressions are not represented here... just belong
      establishEdge(getNode(call),getNode(DependenceNode::ACTUALIN,*j),DependenceGraph::BELONGS_TO);
      establishEdge(getNode(call),getNode(DependenceNode::ACTUALOUT,*j),DependenceGraph::CONTROL);
      // however it is necessary to maintain a sysntactic correctness for the parameters
      establishEdge(getNode(call),getNode(DependenceNode::ACTUALIN,*j),DependenceGraph::SYNTACTIC);
      establishEdge(getNode(DependenceNode::ACTUALIN,*j),getNode(call),DependenceGraph::SYNTACTIC);
      establishEdge(getNode(DependenceNode::ACTUALIN,*j),getNode(call),DependenceGraph::CONTROL);
      //                                establishEdge(getNode(parentStmt),getNode(DependenceNode::ACTUALOUT,*j),BELONGS_TO);
    }
  }
}



// ONLY compute the interprocedural information
void ControlDependenceGraph::computeInterproceduralInformation(InterproceduralInfo * ii)
{
  // add all nodes pointing to the sink to the exit-node list
  std::vector<SliceCFGEdge> inEdges=sink.inEdges();
  for (unsigned int i=0;i<inEdges.size();i++)
    {
      SliceCFGNode lastStmt=inEdges[i].source();
      // add those nodes to the exit-node list
      ii->addExitNode(lastStmt.getNode());
    }

  // find all callStmts
  for (Rose_STL_Container< SgNode * >::iterator  i=functionCalls.begin();i!=functionCalls.end();i++)
    {
      if (debugme)
        cout <<"callsite found"<<endl;
      if (debugme)
        cout <<"found: "<<(*i)->unparseToString()<< " of type " <<(*i)->class_name()<<endl;
      SgFunctionCallExp * call=isSgFunctionCallExp(*i);
      // get the next interesting node
      SgNode * interestingNode=DUVariableAnalysisExt::getNextParentInterstingNode(*i);
      ROSE_ASSERT(interestingNode!=NULL);
      int id=ii->addFunctionCall(call);
      //    establishEdge(getNode(interestingNode),getNode(DependenceNode::ACTUALOUT,call),DependenceGraph::BELONGS_TO);
      // if the callExpression itsel is interesting (according to the filter, add it)
      // geth the closest intersting node in the AST by traversing upwards
      ii->setActualReturn(id,call);
      // since there is a small difference for the call and the interesting node ...
      //    setCallInterestingNode

      // store the slice imporatnt node
      if (debugme) {
        cout <<call<<endl;
        cout <<interestingNode<<endl;
        cout <<interestingNode->class_name()<<endl;
        cout <<interestingNode->unparseToString()<<endl;
      }
      if (isSgExprStatement(interestingNode->get_parent())) interestingNode=interestingNode->get_parent();
      ii->setSliceImportantNode(id,interestingNode);
      // for every parameter in the calls SgExpListExpr
      Rose_STL_Container<SgExpression*> params=call->get_args()->get_expressions();
      for (Rose_STL_Container<SgExpression*>::iterator j=params.begin();j!=params.end();j++)
        {
          // add the ref to the actual in list ..
          ii->addActualIn(id,*j);
        }
    }
}


// DQ (8/30/2009): This appears to be a large block of code commented out!
#if 0
void ControlDependenceGraph::_buildCDG()
{

  // Go through the dominance frontier. For every SgNode with a
  // non-empty frontier, add dependences from the nodes in the frontier
  // to that node.

  //    int size = _dt->getSize();

  // for all nodes in the dominator tree
  for (int i = 0; i < size; i++)
    {
      ControlNode *currentNode=_cfg->getNode(i, ControlFlowGraph::BACKWARD);
      if (debugme)
        if (currentNode==NULL)
          cout <<i<<"(NULL):"<<endl;
        else if (currentNode->getNode()==NULL)
          cout <<i<<"(empty):"<<currentNode->getType()<<endl;
        else
          cout <<i<<"("<<currentNode->getNode()->unparseToString()<<"):"<<currentNode->getType()<<endl;

      if (currentNode->getType() !=ControlNode::EMPTY)
        {
          DependenceNode * currentDepNode=createNodeC(currentNode);
                
          if (debugme)
            cout << "bdf:";
                                
          set < int >frontier = _df->getFrontier(i);
          // for all nodes in the reverse-dmonance-frontier (absoultely dominating nodes)
          for (set < int >::iterator j = frontier.begin(); j != frontier.end(); j++)
            {
              int nodeIndex = *j;


              ControlNode *parentNode = _cfg->getNode(nodeIndex, ControlFlowGraph::BACKWARD);
              if (debugme)
                cout <<"\t>"<< parentNode->getNode()->unparseToString()<<"<";

              if (parentNode->getType() == ControlNode::EMPTY)
                continue;

              DependenceNode *parentDepNode = createNodeC(parentNode);

              // link them (we need to link the nodes from the new graph, not
              // the originals)
              establishEdge(parentDepNode,currentDepNode);

              // If the "to" link is a break or a continue statement, we need
              // to link it in the opposite direction too, to ensure that they
              // are correctly accounted for when calculating
              // slices.
              /* If we have something like this:

                 while (x < 5) { if (x == 3) break; x++; }

                 We would determine that (x < 5) is control dependent on (x ==
                 3). This is true, but is only the case because of the break
                 statement. Thus, whenever (x == 3) is involved, we would also
                 like the break statement to be involved.

                 NOTE: I'm not convinced that making (x == 3) control dependent
                 on the break statement is the best way to do this, but it
                 suffices for now. */

              ///if ((currentNode->getType() == DependenceNode::SGNODE)
              if ((currentNode->getType() == DominatorTreesAndDominanceFrontiers::ControlNode::SGNODE)
                  && ((isSgBreakStmt(currentNode->getNode())) || (isSgContinueStmt(currentNode->getNode()))))
                establishEdge(currentDepNode, parentDepNode);
            }
          if (debugme)
            cout << endl;
        }
    }

  if (_interprocedural != NULL)
    {
      _buildInterprocedural();
    }

  printf("\n\n");

}

void ControlDependenceGraph::_buildInterprocedural()
{


  // Go through the SGNODE dependence nodes and create the appropriate
  // call site nodes, entry nodes etc.

  SgFunctionDefinition *func = isSgFunctionDefinition(_head);

  ROSE_ASSERT(func != NULL);

  // First create the entry node for the procedure
  _interprocedural->procedureEntry.entry =
    new DependenceNode(DependenceNode::ENTRY, func->get_declaration());
  DependenceNode *entry = createNode(_interprocedural->procedureEntry.entry);

  // Link the entry node up with all the nodes in the CDG which do not have
  // predecessors
  for (set < SimpleDirectedGraphNode * >::iterator i = _nodes.begin(); i != _nodes.end(); i++)
    {
      DependenceNode *node = dynamic_cast < DependenceNode * >(*i);

      if ((node->numPredecessors() == 0) && (node != entry))
        {
          establishEdge(entry, node);
        }
    }

  // create a formal out return argument, control dependent on the entry
  // node
  string return_name = func->get_declaration()->get_name().str();

  return_name = return_name + " return";
  _interprocedural->procedureEntry.formal_return =
    new DependenceNode(DependenceNode::FORMALRETURN, return_name);
  DependenceNode *formal_return = createNode(_interprocedural->procedureEntry.formal_return);

  establishEdge(entry, formal_return);

  // for each of the arguments in the function parameter list, add a
  // formal-in and formal-out node
  SgFunctionParameterList *paramlist = func->get_declaration()->get_parameterList();
  SgInitializedNamePtrList params = paramlist->get_args();

  for (SgInitializedNamePtrList::iterator i = params.begin(); i != params.end(); i++)
    {
      SgInitializedName *name = *i;
      DependenceNode *formal_in = new DependenceNode(DependenceNode::FORMALIN,
                                                     name->get_name().str());
      DependenceNode *formal_out = new DependenceNode(DependenceNode::FORMALOUT,
                                                      name->get_name().str());

      establishEdge(entry, createNode(formal_in));
      establishEdge(entry, createNode(formal_out));
      _interprocedural->procedureEntry.formal_in[name] = formal_in;
      _interprocedural->procedureEntry.formal_out[name] = formal_out;

      // To preserve the order of arguments, we insert them into arg_order
      _interprocedural->procedureEntry.arg_order.push_back(name);
    }

  // Now we go through each of the SgNodes in our CDG. If any of them
  // contain a function call, we want to build a call site node for them.
  map < SgNode *, DependenceNode * >::iterator sgnode_iterator;
  for (sgnode_iterator = _sgnode_map.begin();
       sgnode_iterator != _sgnode_map.end(); sgnode_iterator++)
    {
      SgNode *currnode = sgnode_iterator->first;

      list < SgFunctionCallExp * >calls = InterproceduralInfo::extractFunctionCalls(currnode);
      if (calls.empty())
        continue;

      for (list < SgFunctionCallExp * >::iterator i = calls.begin(); i != calls.end(); i++)
        {
          SgFunctionCallExp *call = *i;

          // This needs to be replaced with some call graph analysis
          SgFunctionRefExp *func = isSgFunctionRefExp(call->get_function());

          ROSE_ASSERT(func != NULL);
          SgName func_name = func->get_symbol()->get_name();

          InterproceduralInfo::CallSiteStructure callstructure;
          callstructure.callsite = new DependenceNode(DependenceNode::CALLSITE, call);
          // the call site is control dependent on the statement (i.e. for
          // the call site to happen, the statement must be executed)
          DependenceNode *callsite = createNode(callstructure.callsite);

          // addLink(callsite, getNode(currnode));
          establishEdge(getNode(currnode), callsite);

          // create an actual out node for the return value, control
          // dependent on callsite
          string return_name = func_name.str();

          return_name = return_name + " return";
          callstructure.actual_return =
            new DependenceNode(DependenceNode::ACTUALOUT, return_name);
          DependenceNode *actual_return = createNode(callstructure.actual_return);

          establishEdge(callsite, actual_return);

          // For each argument in the function call, build an actual_in and
          // actual_out, control dependent on callsite
          SgExpressionPtrList args = call->get_args()->get_expressions();

          for (SgExpressionPtrList::iterator j = args.begin(); j != args.end(); j++)
            {
              SgExpression *arg = *j;
              DependenceNode *actual_in = new DependenceNode(DependenceNode::ACTUALIN, arg);
              DependenceNode *actual_out = new DependenceNode(DependenceNode::ACTUALOUT, arg);

              establishEdge(callsite, createNode(actual_in));
              establishEdge(callsite, createNode(actual_out));
              callstructure.actual_in[arg] = actual_in;
              callstructure.actual_out[arg] = actual_out;

              // To preserve the order of expressions in the parameter list, 
              // 
              // we insert them into expr_order
              callstructure.expr_order.push_back(arg);
            }

          // add the callstructure to interprocedural info
          _interprocedural->callsite_map[call] = callstructure;
        }
    }
}
#endif
        
        
        
        
        
        
        
        
        
        
        
/*      
        DependenceNode *ControlDependenceGraph::createNodeC(ControlNode * cnode)
        {
        DependenceNode *newNode;

        if (_cnode_map.count(cnode) == 0)
        {
        if (cnode->getType() == ControlNode::EMPTY)
        {
        newNode = createNode(new DependenceNode(DependenceNode::CONTROL));
        }
        else
        {
        SgNode *tmp = cnode->getNode();

        while (!isSgStatement(tmp) && !isSgExpressionRoot(tmp))
        {
        tmp = tmp->get_parent();
        }
        newNode = createNode(tmp);
        }

        _cnode_map[cnode] = newNode;
        return newNode;
        }
        else
        {
        return _cnode_map[cnode];
        }
        }*/
