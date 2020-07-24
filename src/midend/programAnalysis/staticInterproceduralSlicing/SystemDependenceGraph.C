// tps (1/14/2010) : Switching from rose.h to sage3 changed size from 20,9 MB to 10,7MB
#include "sage3basic.h"

#include "DependenceGraph.h"
#include "CallGraph.h"
#include <map>
#include <list>
using namespace std;

#include <assert.h>

bool SystemDependenceGraph::isKnownLibraryFunction(SgFunctionDeclaration *fDec)
{
  if (debug)
    cout << libraryExtenders.size()<<" stored"<<endl;
  for (unsigned int i=0;i<libraryExtenders.size();i++)
  {
    if (debug)
      cout <<"calling library entender #"<<i<<endl;
    if (libraryExtenders[i]->isKnownLibraryFunction(fDec)) return true;
  }
  SgName fName=fDec->get_mangled_name();// fDec->get_qualified_name();
//  cout <<"mangled name is "<<fName.getString()<<endl;
  if (fName.getString()==string("printf___Fb_i_Gb_RST__Pb__Cc__Pe____sep__e_Fe_")) return true;
  else if (fName.getString()==string("fflush___Fb_i_Gb___Pb__FILE_IO_FILE__Pe___Fe_")) return true;
  return false;
}
void SystemDependenceGraph::createConnectionsForLibaryFunction(SgFunctionDeclaration *fDec)
{
  // if the function is described in an libraryExtender...
  for (unsigned int i=0;i<libraryExtenders.size();i++)
  {
    if (libraryExtenders[i]->isKnownLibraryFunction(fDec))
    {
      // use it to generate interpocedural connection
      libraryExtenders[i]->createConnectionsForLibaryFunction(fDec,this);
       return;
    }
  }
  SgName fName=fDec->get_mangled_name();
  InterproceduralInfo * ii=getInterproceduralInformation(fDec);

// DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
// DependenceNode * formalIn,*formalOut,*entry,*formalReturn;
   DependenceNode * formalIn,*entry,*formalReturn;

  ROSE_ASSERT(ii != NULL);
  entry=getNode(DependenceNode::ENTRY,ii->getFunctionEntry());
  formalReturn=getNode(DependenceNode::FORMALRETURN,ii->getFormalReturn());
  if (fName.getString()==string("printf___Fb_i_Gb_RST__Pb__Cc__Pe____sep__e_Fe_")
    ||fName.getString()==string("fflush___Fb_i_Gb___Pb__FILE_IO_FILE__Pe___Fe_")
     )
  {
    if (debug)
      cout <<"creating library-function-sdg-sub-graph"<<endl;
    // returnvalue is dependant of input but input is not changed!
    for (int i=0;i<ii->getFormalCount();i++)
    {
      formalIn=getNode(DependenceNode::FORMALIN,ii->getFormal(i));

   // DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
   // formalOut=getNode(DependenceNode::FORMALOUT,ii->getFormal(i));
      getNode(DependenceNode::FORMALOUT,ii->getFormal(i));

      establishEdge(formalIn,entry,DATA);     
      establishEdge(formalIn,formalReturn,DATA);
    }
  }
}

void SystemDependenceGraph::createSafeConfiguration(SgFunctionDeclaration *fDec)
{
  InterproceduralInfo * ii=getInterproceduralInformation(fDec);
  DependenceNode * formalIn,*formalOut,*entry,*formalReturn;
  assert(ii != NULL);
  entry=getNode(DependenceNode::ENTRY,ii->getFunctionEntry());
  formalReturn=getNode(DependenceNode::FORMALRETURN,ii->getFormalReturn());
  for (int i=0;i<ii->getFormalCount();i++)
  {
    formalIn=getNode(DependenceNode::FORMALIN,ii->getFormal(i));
    formalOut=getNode(DependenceNode::FORMALOUT,ii->getFormal(i));
    establishEdge(formalIn,entry,DATA);
    establishEdge(formalIn,formalOut,DATA);
    establishEdge(formalIn,formalReturn,DATA);
    // helper
    establishEdge(entry,formalIn,SYNTACTIC);
    establishEdge(formalOut,entry,CONTROL);
  }
  if (ii->getEllipse())
  {
    formalIn=getNode(DependenceNode::FORMALIN,ii->getEllipse());
    formalOut=getNode(DependenceNode::FORMALOUT,ii->getEllipse());
    establishEdge(formalIn,entry,DATA);
    establishEdge(formalIn,formalOut,DATA);
    establishEdge(formalIn,formalReturn,DATA);
    // helper
    establishEdge(entry,formalIn,SYNTACTIC);
    establishEdge(formalOut,entry,CONTROL);
  }
  establishEdge(entry,formalReturn,CONTROL);
}

void SystemDependenceGraph::parseProject(SgProject *project)
{
#ifdef NEWDU
  // Create the global def-use analysis
  EDefUse  *defUseAnalysis=new EDefUse(project);
  if (defUseAnalysis->run(false)==1)
  {
    std::cerr<<"SystemDependenceGraph :: DFAnalysis failed!  -- defUseAnalysis->run(false)==0"<<endl;
    exit(0);
  }
#endif
  // restrict to only aclually called functions an the main function, 
  // find all function calls ->get the function defintion and add it to the set
  {
    Rose_STL_Container<SgNode * >functionCalls= NodeQuery::querySubTree(project,V_SgFunctionCallExp);
    for (Rose_STL_Container < SgNode * >::iterator callIt=functionCalls.begin();
         callIt!=functionCalls.end();
         callIt++)
    {

    }
  }
  Rose_STL_Container < SgNode * >functionDeclarations = NodeQuery::querySubTree(project, V_SgFunctionDeclaration);
  for (Rose_STL_Container< SgNode * >::iterator i = functionDeclarations.begin(); i != functionDeclarations.end(); i++)
  {
    ControlDependenceGraph *cdg;
    DataDependenceGraph *ddg;
    InterproceduralInfo *ipi;
    SgFunctionDeclaration *fDec = isSgFunctionDeclaration(*i);

    ROSE_ASSERT(fDec != NULL);
    if (fDec->get_definition() == NULL)
    {
      ipi=new InterproceduralInfo(fDec);
      //create "Safe"-Configurations
      ipi->addExitNode(fDec);
      addInterproceduralInformation(ipi);
      ipi->addExitNode(fDec);
      //>addInterproceduralInformation(ipi);
      if (isKnownLibraryFunction(fDec))
      {
        createConnectionsForLibaryFunction(fDec);
      }
      else
      {
      createSafeConfiguration(fDec);
}
      // This is somewhat a waste of memory and a more efficient approach might generate this when needed, but at the momenent everything is created...
    }
    else
    {
      // get the control depenence for this function
      ipi=new InterproceduralInfo(fDec);

      ROSE_ASSERT(ipi != NULL);

      // get control dependence for this function defintion
      cdg = new ControlDependenceGraph(fDec->get_definition(), ipi);
      cdg->computeAdditionalFunctioncallDepencencies();
      cdg->computeInterproceduralInformation(ipi);

      // get the data dependence for this function
      #ifdef NEWDU
      ddg = new DataDependenceGraph(fDec->get_definition(), defUseAnalysis,ipi);
      #else
      ddg = new DataDependenceGraph(fDec->get_definition(), ipi);
      #endif
      cdg->computeAdditionalFunctioncallDepencencies();
      ddg->computeInterproceduralInformation(ipi);

      addFunction(cdg,ddg);
      addInterproceduralInformation(ipi);
    }
    // else if (fD->get_definition() == NULL)

  }
  performInterproceduralAnalysis();
  std::set<SgNode*> preserveSet;
  preserveSet.insert(getMainFunction());
  cleanUp(preserveSet); 

}



void SystemDependenceGraph::cleanUp(std::set<SgNode*> preserve)
{
  
  // go over all formal out and remove them, if there is no incoming data edge..
  for(Rose_STL_Container<InterproceduralInfo *>::iterator i=interproceduralInformationList.begin();i!=interproceduralInformationList.end();i++)
  {
    bool erase=true;
    InterproceduralInfo * ii = *i;
    if (getExistingNode(DependenceNode::ENTRY,ii->getFunctionEntry())==NULL) continue;
    // if either the declaration or the definition is in the from set, this function is a source for the reachabilitity
    if (!preserve.count(ii->getFunctionDefinition()) || preserve.count(ii->getFunctionDeclaration()))
    {
      // ok, now switch erase to true, if this function is not called
      
      // get all incmoing edges for the entry
      DependenceNode * entry=getExistingNode(DependenceNode::ENTRY,ii->getFunctionEntry());
      // get the prececessor set
      std::set<SimpleDirectedGraphNode *> pred=entry->getPredecessors();
      for (std::set<SimpleDirectedGraphNode *>::iterator k=pred.begin();k!=pred.end();k++)
      {
        DependenceNode * source=dynamic_cast<DependenceNode*>(*k);
    //get edgetpye for this ...?
    if (edgeExists(source,entry,CALL))
    {
      // we are finished
      erase=false;
      break;
    }
      }
    }
    else
    {
      erase=false;
    }
    if (erase)
    {
// if (

    // check the number of incoming edges..
    for (int j=0;j<ii->getFormalCount();j++)
    {
      DependenceNode* currentFormal=getNode(DependenceNode::FORMALOUT,ii->getFormal(j));
      if (currentFormal->numPredecessors()<2) // only control edge....
      {
#ifdef VERBOSE_DEBUG
          cout <<"pruning FORMALOUT "; currentFormal->writeOut(cout);cout <<endl;
#endif
        std::set<SimpleDirectedGraphNode *> succs=currentFormal->getSuccessors();
        deleteNode(currentFormal);
        for (std::set<SimpleDirectedGraphNode *>::iterator k=succs.begin();k!=succs.end();k++)
        {
          // get rid of the children...
          deleteNode(dynamic_cast<DependenceNode*>(*k));
//          free(*k);
        }
      }
      // check if the formal in has any edges coming in, if not, get rid of the whole stuff
      currentFormal=getNode(DependenceNode::FORMALIN,ii->getFormal(j));
      if (currentFormal->numPredecessors()<1)
      {
        deleteNode(currentFormal);
      }
            
    }
    if (ii->getEllipse())
    {
      DependenceNode* currentFormal=getNode(DependenceNode::FORMALIN, ii->getEllipse());
      if (currentFormal->numPredecessors()<2) // only control edge....
      {
#ifdef VERBOSE_DEBUG
        cout <<"pruning FORMALOUT "; currentFormal->writeOut(cout);cout <<endl;
#endif
        std::set<SimpleDirectedGraphNode *> succs=currentFormal->getSuccessors();
        deleteNode(currentFormal);
        for (std::set<SimpleDirectedGraphNode *>::iterator k=succs.begin();k!=succs.end();k++)
        {
          // get rid of the children...
          deleteNode(dynamic_cast<DependenceNode*>(*k));
//          free(*k);
        }
      }
      // check if the formal in has any edges coming in, if not, get rid of the whole stuff
      currentFormal=getNode(DependenceNode::FORMALIN,ii->getEllipse());
      if (currentFormal->numPredecessors()<1)
      {
        deleteNode(currentFormal);
      }
            
    }
    // last check the formal return if it has incoming/outgoing  edges if none, clean up
    DependenceNode *returnNode=getExistingNode(DependenceNode::FORMALRETURN,ii->getFormalReturn());
    if (returnNode!=NULL && returnNode->numPredecessors()==0 &&returnNode->numSuccessors()==0)
    {
        //TODO: reimplet this so that there is no Segfault
        deleteNode(returnNode);
        ii->setFormalReturn(NULL);
    }
    // and at last, remove the functioncallNode
    DependenceNode * entry=getExistingNode(DependenceNode::ENTRY,ii->getFunctionEntry());
    if (returnNode!=NULL && returnNode->numPredecessors()==0 &&returnNode->numSuccessors()==0 && entry!=NULL)
        {
                //TODO: reimplet this so that there is no Segfault
                        deleteNode(entry);
                        }
  }}
}

void SystemDependenceGraph::computeSummaryEdges()
{
  std::set<Edge> pathEdge,summaryEdgel;
  std::vector<Edge> workList;
  // for all formaloutvertices...
  int count=0;
  for(Rose_STL_Container<InterproceduralInfo *>::iterator i=interproceduralInformationList.begin();i!=interproceduralInformationList.end();i++)
  {
    DependenceNode * w;
#ifdef VERBOSE_DEBUG
    cout <<"processing iterprocedural #"<<count<<endl;
#endif
    count++;
    InterproceduralInfo * ii = *i;
    for (int j=0;j<ii->getFormalCount();j++)
    {
//      cout <<"adding formalout to worklist"<<endl;
      w=getNode(DependenceNode::FORMALOUT,ii->getFormal(j));
      pathEdge.insert(Edge(w,w));
      workList.push_back(Edge(w,w));
    }
    if (ii->getEllipse())
    {
//      cout <<"adding formalout to worklist"<<endl;
      w=getNode(DependenceNode::FORMALOUT,ii->getEllipse());
      pathEdge.insert(Edge(w,w));
      workList.push_back(Edge(w,w));
    }
    // add the return
    w=getNode(DependenceNode::FORMALRETURN,ii->getFormalReturn());
    pathEdge.insert(Edge(w,w));
    workList.push_back(Edge(w,w));
  }
  
  while(workList.size())
  {
#ifdef VERBOSE_DEBUG
    cout <<"------------------------------------------------------------------"<<endl;
#endif
    DependenceNode *v,*w,*x,*y,*a;
    v=workList.back().first;
    w=workList.back().second;
    workList.pop_back();
#ifdef VERBOSE_DEBUG
    cout <<"v = ";v->writeOut(cout);cout<<" of type"<<v->getType()<<endl;
    cout <<"w = ";w->writeOut(cout);cout<<" of type"<<w->getType()<<endl;
#endif
    switch(v->getType())
    {
      case DependenceNode::ACTUALOUT:
        {
#ifdef VERBOSE_DEBUG
          cout <<"actualout found :"<<v->getSgNode()->unparseToString()<<endl;
#endif
          std::set<SimpleDirectedGraphNode *> preds=v->getPredecessors();
#ifdef VERBOSE_DEBUG
          cout <<"\t"<<preds.size()<<" predecessors found"<<endl;
#endif
          for (std::set<SimpleDirectedGraphNode *>::iterator sdgnx=preds.begin();sdgnx!=preds.end();sdgnx++)
          {
            x=dynamic_cast<DependenceNode*>(*sdgnx);
            if (edgeType(x,v).count(SUMMARY) || edgeType(x,v).count(CONTROL))
            {
              if (!pathEdge.count(Edge(x,w)))
              {
                pathEdge.insert(Edge(x,w));
                workList.push_back(Edge(x,w));
              }
            }
          }
        }
        break;
      case DependenceNode::FORMALIN:
        {
          //v is the formal in and w is the formal out
#ifdef VERBOSE_DEBUG
          cout <<"formalin found"<<endl;
#endif
          //insert the summary edge at all call sites, best done by fololowing the parameter_in edge and connect that node with the out node
          std::set<SimpleDirectedGraphNode *> preds=v->getPredecessors();
          for (std::set<SimpleDirectedGraphNode *>::iterator sdgnx=preds.begin();sdgnx!=preds.end();sdgnx++)
          {
            // x is the actual in dep node
            x=dynamic_cast<DependenceNode*>(*sdgnx);
            if (edgeType(x,v).count(PARAMETER_IN))
            {
#ifdef VERBOSE_DEBUG
              cout <<"adding summary edge"<<endl;
#endif
              // if the formal out is the return find the return of the call-site and connect
              if (w->isFormalReturn())
              {
                // absuing hte actual in to get teh calling sg-node
                SgNode *var=x->getSgNode();
                SgNode *parent=var->get_parent();
//                while(!IsImportantForSliceSgFilter(parent)) 
                while(!isSgFunctionCallExp(parent))
                  parent=parent->get_parent();
                // parent is now the calling node
                y=getNode(DependenceNode::ACTUALRETURN,parent);
              }
              else
                y=getNode(DependenceNode::ACTUALOUT,x->getSgNode());
              establishEdge(x,y,SUMMARY);
              std::set<SimpleDirectedGraphNode *>  succs=y->getSuccessors();
              for (std::set<SimpleDirectedGraphNode *>::iterator  sdgna=succs.begin();sdgna!=succs.end();sdgna++)
              {
                a=dynamic_cast<DependenceNode*>(*sdgna);
                if (!pathEdge.count(Edge(x,a)))
                {
                  pathEdge.insert(Edge(x,a));
                  workList.push_back(Edge(x,a));
                }
              }
            }
          }
          // store the summary edge, for all callers...
                  
//        std::vector<InterproceduralInfo*> callers=getPossibleFuncs(
        }
        break;
      default:
        {
#ifdef VERBOSE_DEBUG
          cout <<"default case: "<<v->getType()<<endl;
#endif
          std::set<SimpleDirectedGraphNode *> preds=v->getPredecessors();
#ifdef VERBOSE_DEBUG
          cout <<"\t"<<preds.size()<<" predecessors found"<<endl;
#endif
          for (std::set<SimpleDirectedGraphNode *>::iterator sdgnx=preds.begin();sdgnx!=preds.end();sdgnx++)
          {
            
            x=dynamic_cast<DependenceNode*>(*sdgnx);
      //      cout <<"x = ";x->writeOut(cout);cout<<" of type "<<x->getType()<<endl;
            if (edgeType(x,v).count(CONTROL)|| edgeType(x,v).count(DATA)/* ||edgeType(x,v).count(PARAMETER_OUT)*/)
            {
              if (!pathEdge.count(Edge(x,w)))
              {
                VERBOSE_DEBUG_BLOCK(cout <<"\tadding x to workList"<<endl;)
                pathEdge.insert(Edge(x,w));
                workList.push_back(Edge(x,w));
              }
VERBOSE_DEBUG_STMT(else
                cout <<"\talready visited that edge..."<<endl;)
            }
VERBOSE_DEBUG_STMT(
            else
              cout <<"\tedge not of type (control,data or return)"<<endl;)
          }
        }
        break;
    } 
  }   
}

// DQ (10/5/2007): Change suggested by Jeremiah.
void SystemDependenceGraph::performInterproceduralAnalysis()
   {
  // Note that the size of interproceduralInformationList is increased
  // during this loop, so size() needs to be called every time
     for(unsigned int i=0;i!=interproceduralInformationList.size();i++)
        {
       // for every interprocedural-information-set
          InterproceduralInfo * ii = interproceduralInformationList[i];
       // first: if it is a libary-call
       // if (ii->isUndefined())
       //      createFunctionStub(ii);

       // create InterproceduralConnections
          doInterproceduralConnections(ii);
        }
     computeSummaryEdges();
   }

std::vector<InterproceduralInfo*> SystemDependenceGraph::getPossibleFuncs(SgFunctionCallExp * funcCall)
{
  std::vector<InterproceduralInfo*> retVal;
  SgFunctionSymbol *fsym = NULL; 
  // check if there is the function declaration available and return the ipi for that decl
  SgFunctionRefExp *fref = isSgFunctionRefExp(funcCall->get_function());
  // Liao 11/15/2011
  // It could be a call to a class member function
  SgArrowExp* arrow_exp = isSgArrowExp (funcCall->get_function());
  SgDotExp* dot_exp = isSgDotExp (funcCall->get_function());
  if (arrow_exp|| dot_exp)
  {
    SgBinaryOp * bop = isSgBinaryOp(funcCall->get_function());
    ROSE_ASSERT (bop != NULL);
    SgMemberFunctionRefExp* mfref = isSgMemberFunctionRefExp (bop->get_rhs_operand_i());
    ROSE_ASSERT (mfref != NULL);
    fsym =  mfref ->get_symbol();
  }
  else if (fref)
  {
    fsym = fref->get_symbol();
  }
  else
  {
    cerr<<"Error: SystemDependenceGraph::getPossibleFuncs() found a unhandled function call type:"<< funcCall->get_function()->class_name()<<endl;
    ROSE_ASSERT (false);
  }
  ROSE_ASSERT (fsym != NULL);
  SgFunctionDeclaration *fD = fsym->get_declaration();
#ifdef VERBOSE_DEBUG
  cout << "Adding function call " << funcCall->unparseToString() << " with funref " << fref->unparseToString() << " and function " << fD->get_name().getString() << endl;
#endif
  // check if that function exists, if not use either known function stubs or create a safe function stub
  if (interproceduralInformation.count(fD))
  {
    // get the interprocedural information container
    retVal.push_back(interproceduralInformation[fD]);
    ROSE_ASSERT (interproceduralInformation[fD]->getFunctionDeclaration() == fD);
  }
  else
  {
    // on the fly construct the correct stub
    InterproceduralInfo * ipi=new InterproceduralInfo(fD);
    ipi->addExitNode(fD);
    addInterproceduralInformation(ipi);
    // if there is a predefined interface description, with flow information...
    if (isKnownLibraryFunction(fD))
    {
      // use it
      createConnectionsForLibaryFunction(fD);
    }
    else
    {
      // else construct a safte assumption
      createSafeConfiguration(fD);
    }
    retVal.push_back(ipi);
  }
  return retVal;
}

void SystemDependenceGraph::addFunction(ControlDependenceGraph * cdg, DataDependenceGraph * ddg){
  mergeGraph(cdg);
  mergeGraph(ddg);
}

void SystemDependenceGraph::createFunctionStub(InterproceduralInfo * ii)
{
  //addFunctionEnry
  DependenceNode *entry=getNode(DependenceNode::ENTRY,ii->getFunctionEntry());
  // add formal_in && out for formal-nodes
  for (int i=0;i<ii->getFormalCount();i++)
  {
    establishEdge(entry,getNode(DependenceNode::FORMALIN,ii->getFormal(i)),CONTROL);
    establishEdge(entry,getNode(DependenceNode::FORMALOUT,ii->getFormal(i)),CONTROL);
  }
  if (ii->getEllipse()!=NULL)
  {
    // setup formal for ellipsis
    establishEdge(entry,getNode(DependenceNode::FORMALIN,ii->getEllipse()),CONTROL);
    establishEdge(entry,getNode(DependenceNode::FORMALOUT,ii->getEllipse()),CONTROL);
  }
  // create the return-value
  if (!isSgTypeVoid(ii->getFunctionDeclaration()->get_type()))
    establishEdge(entry,getNode(DependenceNode::FORMALOUT,ii->getFormalReturn()),CONTROL);
}




void SystemDependenceGraph::doInterproceduralConnections(InterproceduralInfo * ii)
{
  //connect the retunring nodes from the function with the call-site
  SgFunctionCallExp * callSite;
  std::vector<InterproceduralInfo*> functionTargetList;

  // get all callsites within this function
  for (int i=0;i<ii->callSiteCount();i++)
  {
    // get the SgFunctionCallExp node
    callSite=isSgFunctionCallExp(ii->getFunctionCallExpNode(i));

    // cout << "Found funcall: " << callSite->unparseToString() << endl;

    // do analysis of the function call and determine the possible call-targets
    functionTargetList=getPossibleFuncs(callSite);
    // for every possible callable function
    for (unsigned int j=0;j<functionTargetList.size();j++)
    {
      InterproceduralInfo * calledFunction=functionTargetList[j];     
      ROSE_ASSERT(calledFunction!=NULL);
      // cout << "Callee: " << calledFunction->getFunctionDeclaration()->get_name().getString() << endl;
      // insert the function_formal_in and formal_out nodes, just to be safe
//CMI modified 19082007      establishEdge(getNode(DependenceNode::ENTRY,calledFunction->getFunctionEntry()),getNode(DependenceNode::FORMALRETURN,calledFunction->getFormalReturn()),CONTROL);
      // and the formal out for every parameter
      for (int k=0;k<calledFunction->getFormalCount();k++)
      {
        establishEdge(getNode(DependenceNode::ENTRY,calledFunction->getFunctionEntry()),getNode(DependenceNode::FORMALIN,calledFunction->getFormal(k)),CONTROL);
      }
      if (calledFunction->getEllipse())
      {
        establishEdge(getNode(DependenceNode::ENTRY,calledFunction->getFunctionEntry()),getNode(DependenceNode::FORMALIN,calledFunction->getEllipse()),CONTROL);
      }

      // create the call edge that connects the call with the callee
      establishEdge(getNode(callSite),getNode(DependenceNode::ENTRY,calledFunction->getFunctionEntry()),CALL);
      // connect the FORMAL_OUT(return) from the function to the ACTUAL_OUT of the call site
      //CMI modified 19082007            
      establishEdge(getNode(DependenceNode::FORMALRETURN,calledFunction->getFormalReturn()),getNode(DependenceNode::ACTUALRETURN,ii->getActualReturn(i)),PARAMETER_OUT);

      // connect actual in to formal in and actual out to formal out

      // if there are not enough parameters s.th is terribly wrong
      if (ii->getActualInCount(i)<calledFunction->getFormalCount())
      {
        cerr <<"expecting "<<calledFunction->getFormalCount()<<"arguments at minimum, "<<ii->getActualInCount(i)<<" received"<<endl;
        ROSE_ASSERT(false);
      }
      else if (!calledFunction->getEllipse() && ii->getActualInCount(i)>calledFunction->getFormalCount())
      {
            // if there are less parameters specified than actual parameters available, there must be an elipsis
            cerr<<"WARNING: functioncall " << calledFunction->getFunctionDeclaration()->get_name().getString() << ": " << ii->getFunctionCallExpNode(i)->unparseToString() << " has more parameters than it exprects arguments: ... used?"<<endl;
      }
      // for each paramters
      for (int k=0;k<calledFunction->getFormalCount();k++)
                //      for (int k=0;k<ii->getActualInCount(i);k++)
     {
       establishEdge(getNode(DependenceNode::ACTUALIN,ii->getActualIn(i,k)),getNode(DependenceNode::FORMALIN,calledFunction->getFormal(k)),PARAMETER_IN);

       establishEdge(getNode(DependenceNode::FORMALOUT,calledFunction->getFormal(k)),getNode(DependenceNode::ACTUALOUT,ii->getActualIn(i,k)),PARAMETER_OUT);
     }
      // connect all surplus arguments to the ellipse
      if (calledFunction->getEllipse()!=NULL)
      {
        VERBOSE_DEBUG_BLOCK(        cout <<"\t\tcalled function has ellipse!"<<endl;)
        SgNode * ellipse=calledFunction->getEllipse();
        for (int k=calledFunction->getFormalCount();k<ii->getActualInCount(i);k++)
        {
          establishEdge(getNode(DependenceNode::ACTUALIN,ii->getActualIn(i,k)),getNode(DependenceNode::FORMALIN,ellipse),PARAMETER_IN);
          establishEdge(getNode(DependenceNode::FORMALOUT,ellipse),getNode(DependenceNode::ACTUALOUT,ii->getActualIn(i,k)),PARAMETER_OUT);
          establishEdge(getNode(DependenceNode::ACTUALIN,ii->getActualIn(i,k)),getNode(DependenceNode::ACTUALOUT,ii->getActualIn(i,k)),SUMMARY);

        }
      }
    }
  }
}

void SystemDependenceGraph::addFunction(FunctionDependenceGraph * pdg)
{

  ROSE_ASSERT(pdg->getInterprocedural() != NULL);
  SgFunctionDeclaration *func = pdg->getInterprocedural()->getFunctionDeclaration();

  ROSE_ASSERT(func != NULL);
  _funcs_map[func] = pdg;
  //CI (01/12/2007)
  functionToInterfunctionalMap[func] = NULL;// pdg->getInterprocedural();

  _mergeGraph(pdg);
}

void SystemDependenceGraph::process()
{
  map < SgFunctionDeclaration *, FunctionDependenceGraph * >::iterator func_iterator;
  for (func_iterator = _funcs_map.begin(); func_iterator != _funcs_map.end(); func_iterator++)
  {
    //CI (01112007): inserted check for declared only functions
    if (func_iterator->second!=NULL)
    {
      FunctionDependenceGraph *pdg = func_iterator->second;

      _processFunction(pdg);
    }
  }
}

void SystemDependenceGraph::_processFunction(FunctionDependenceGraph * pdg)
{
#if 0
//TODO
    // go through call sites, link up actual_in to formal_in and actual_out to 
    // formal_out nodes
    // and link call sites to call entry nodes
    InterproceduralInfo *info = pdg->getInterprocedural();

    map < SgFunctionCallExp *, InterproceduralInfo::CallSiteStructure >::iterator callsite_iter;
    for (callsite_iter = info->callsite_map.begin();
         callsite_iter != info->callsite_map.end(); callsite_iter++)
    {
        InterproceduralInfo::CallSiteStructure callsite = callsite_iter->second;
        SgFunctionCallExp *funcCall = callsite_iter->first;

        // This only works with easily resolvable function calls
        list < SgFunctionDeclaration * >possibleFuncs = _getPossibleFuncs(funcCall);
        for (list < SgFunctionDeclaration * >::iterator i =
             possibleFuncs.begin(); i != possibleFuncs.end(); i++)
        {

            SgFunctionDeclaration *fD = *i;
            //CI (01/11/2007): Check if there is a definition to go with the declaration...
            if (fD->get_definition()==NULL) continue;

            // Now get the interproc info for the function we are calling
            InterproceduralInfo::ProcedureEntryStructure procEntry =
                _funcs_map[fD]->getInterprocedural()->procedureEntry;

            // Link up the call site with the procedure entry
            establishEdge(createNode(callsite.callsite), createNode(procEntry.entry), CALL);

            // Link up the formal_out return and the actual_out return
            establishEdge(createNode(procEntry.formal_return),
                          createNode(callsite.actual_return), RETURN);

            // Go through the actual_in nodes from callsite and link them to
            // the formal_in nodes in procEntry
            // and link the formal_out nodes from procEntry to the actual_out
            // nodes from callsite
            list < SgExpression * >::iterator actual_iterator = callsite.expr_order.begin();
            list < SgInitializedName * >::iterator formal_iterator =
                procEntry.arg_order.begin();
            while (actual_iterator != callsite.expr_order.end())
            {
                ROSE_ASSERT(formal_iterator != procEntry.arg_order.end());

                DependenceNode *actual_in = createNode(callsite.actual_in[*actual_iterator]);
                DependenceNode *formal_in = createNode(procEntry.formal_in[*formal_iterator]);

                establishEdge(actual_in, formal_in, CALL);

                DependenceNode *actual_out = createNode(callsite.actual_out[*actual_iterator]);
                DependenceNode *formal_out = createNode(procEntry.formal_out[*formal_iterator]);

                establishEdge(formal_out, actual_out, RETURN);

                actual_iterator++;
                formal_iterator++;
            }
        }
    }
    #endif
}

Rose_STL_Container<SgFunctionDeclaration *>
SystemDependenceGraph::_getPossibleFuncs(SgFunctionCallExp * funcCall)
{

    // This function currently assumes that the function call is resolved
    // statically, and returns a single function declaration (which we
    // can obtain directly from the AST). When a better callgraph
    // analysis is used, we can instead return all possible function
    // declarations, and the SDG will still be produced correctly: Each
    // call site will link up to multiple potential functions.

    Rose_STL_Container< SgFunctionDeclaration * >retval;

    SgFunctionRefExp *fref = isSgFunctionRefExp(funcCall->get_function());
    SgFunctionSymbol *fsym = fref->get_symbol();
    SgFunctionDeclaration *fD = fsym->get_declaration();

    retval.push_back(fD);
    return retval;
}

// Jim 2009-03-04: Does a 2 stage slice as described in Horwitz, et al.
// Honestly, a 2 stage-slice is overkill because we still don't have
// Horwitz's linkage grammer yet.
set < DependenceNode * >SystemDependenceGraph::getSlice(DependenceNode * node)
{

  int edgeTypes1 = CONTROL | DATA | SUMMARY | PARAMETER_OUT;
  
  int edgeTypes2 = CONTROL | DATA | SUMMARY | CALL | PARAMETER_IN;

  set < DependenceNode * >start;
  start.insert(node);
  
  set < DependenceNode * >phase1nodes = _getReachable(start, edgeTypes1);
  
  set < DependenceNode * >phase2nodes = _getReachable(phase1nodes, edgeTypes2);
  
  return phase2nodes;

}

set < FunctionDependenceGraph * >SystemDependenceGraph::getPDGs()
{
    set < FunctionDependenceGraph * >retval;
    map < SgFunctionDeclaration *, FunctionDependenceGraph * >::iterator i;
    for (i = _funcs_map.begin(); i != _funcs_map.end(); i++)
    {
        retval.insert(i->second);
    }
    return retval;
}

SgNode *SystemDependenceGraph::getMainFunction()
{
  SgNode*main=NULL;
  for(Rose_STL_Container<InterproceduralInfo *>::iterator i=interproceduralInformationList.begin();i!=interproceduralInformationList.end();i++)
  {
    if ((*i)->getFunctionDeclaration()->get_name().getString ().compare(std::string("main"))) return (*i)->getFunctionDeclaration();
  }
  return main;
}
