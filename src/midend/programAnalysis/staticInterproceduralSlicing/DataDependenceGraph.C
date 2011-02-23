// tps : Switching from rose.h to sage3 changed size from 22 MB to 12,4MB
#include "sage3basic.h"

#include <AstInterface_ROSE.h>

#include "DependenceGraph.h"

#include "DefUseAnalysis.h"
#include "DefUseExtension.h"
#include "EDefUse.h"

#include <list>
using namespace std;
#include <iostream>
#include "DebugTool.h"
#include <assert.h>
using namespace DUVariableAnalysisExt;
// this function returns true if the variable defined by SgNode is defined
/*SgNode * getNextParentInterstingNode(SgNode* node)
  {
  SgNode * tmp=node;
  while(!IsImportantForSliceSgFilter(tmp) && !isSgFunctionCallExp(tmp) &&!isSgFunctionDefinition(tmp))        {
  tmp=tmp->get_parent();   }
  return tmp;

  }
*/


DataDependenceGraph::DataDependenceGraph(SgNode * head,
#ifdef NEWDU
                                         EDefUse * du,
#endif
                                         InterproceduralInfo * ii)
      : _head(isSgFunctionDefinition(head))
{
#ifdef NEWDU
      defuse=du;
#else
      _buildDefUseChains(_head);
#endif

      //  _interprocedural = ii;
      functionDef=isSgFunctionDefinition(head);
      functionDecl=functionDef->get_declaration();
      buildDDG();
    }

    void outputNodeInfo(std::ostream & os,SgNode* defSgNode)
    {
      if (isSgInitializedName(defSgNode)!=NULL) {
          os <<"\""<<isSgInitializedName(defSgNode)->get_qualified_name().getString()<<"\" of class "<<defSgNode->class_name();
      }
      else {
        os <<"\""<<defSgNode->unparseToString()<<"\" of class "<<defSgNode->class_name();
      }
    }
    

    void DataDependenceGraph::buildDDG()
    {
      std::set<SgInitializedName*> functionParameterSet(_head->get_declaration()->get_args().begin(),_head->get_declaration()->get_args().end());
      //SgNode *tmp;

      //      bool varRefIsDef;
      DependenceNode *defDepNode=NULL;
      DependenceNode *useDepNode=NULL;
      // this graph is intersted of all ueses of variables, up so far NO POINTER analysis is done...
      Rose_STL_Container< SgNode * > varUse=NodeQuery::querySubTree(_head,V_SgVarRefExp);
      for (Rose_STL_Container< SgNode * >::iterator i = varUse.begin();i!=varUse.end();i++) {
        // if the variable is a global, special treatmen
        
        // for each SgVarRef
        
        //varRefIsDef=false;
        
        SgVarRefExp * varRef=isSgVarRefExp(*i);
        SgNode * varDef;
        SgNode * interestingUseParent,
          * interestingDefParent;
        
        
        // get the correct interesting node for the use (This is returning varRef for some dumb reason)
        interestingUseParent=getNextParentInterstingNode(varRef);

        //        cout <<"processing: "<<varRef->unparseToString()<<endl;
        //cout <<"\t 1: use interesting parent is of class:"<<interestingUseParent->class_name()<<
        //  "\n\t\t and is "<<interestingUseParent->unparseToString()<<endl;
        
        // check if the variable is used as argument in a function call
        // this is the interesting parent of the interesting parent
        if (isSgFunctionCallExp(getNextParentInterstingNode(interestingUseParent->get_parent()))) {
          // SgVarRefExp->getNextParentInterstingNode->getNextParentInterstingNode
          useDepNode=getNode(DependenceNode::ACTUALIN,interestingUseParent);                    
        } else if (isSgFunctionDefinition(interestingUseParent)) {
          continue;
        } else {
          useDepNode=getNode(interestingUseParent);
        }
        // the current node is defined, next get the defining nodes
        
        //Jim: What's the point of jumping straight to the decleration here?  We should want the def. from the defuse chains...
        std::vector< SgNode* > defPlaces,usePlaces;
        SgInitializedName *initName=varRef->get_symbol()->get_declaration();
        //VERBOSE_DEBUG_STMT(std::string varName=initName->get_name().getString();)
        
        // handle global variable defenitions
        if (defuse->isNodeGlobalVariable(initName)) {
          //                    initName->get_declaration (); 
          // the variable is a global variable, now connect the statement with the global def
          if (DUVariableAnalysisExt::isIDef(varRef) || DUVariableAnalysisExt::isDef(varRef)) {
            // create a ade from the ref to the globalInitialisation
            establishEdge(useDepNode,getNode(initName->get_declaration()),GLOBALVAR_HELPER);                            
          }
          if (DUVariableAnalysisExt::isIUse(varRef) || DUVariableAnalysisExt::isUse(varRef)) {
            // create a ade from the ref to the globalInitialisation
            establishEdge(getNode(initName->get_declaration()),useDepNode,GLOBALVAR_HELPER);
          }
        }
        
        //      if the current use of the variable is a use
        if (DUVariableAnalysisExt::isUse(varRef) || DUVariableAnalysisExt::isIUse(varRef))
        {
          //Jim: Debug lines, just uncommented
          //cout <<"initname: 2: intersetingUseParent for "<< initName->get_name().getString() << 
          //  " is "<<interestingUseParent->unparseToString()<<" and is of type "<<interestingUseParent->class_name()<<endl;
          //VERBOSE_DEBUG_BLOCK(std::cout<<"\tUSE"<<endl;)
          // get definition places for the interesting node
          //Jim: this appears to be dead code
          //defPlaces=defuse->getDefFor(interestingUseParent,initName);
          
          //Does the below not over write the above?  Curious.  Removing the below creates a false self-ref link on x+=5;
          defPlaces=defuse->getDefFor(varRef,initName);
          // for all possible definition places
          for (unsigned int j=0;j<defPlaces.size();j++) {
            // get the def, so we don't have to use the array all the time
            varDef=defPlaces[j];
            // make shure it is the interesting def parent
            interestingDefParent=getNextParentInterstingNode(varDef);
            
            //cout <<"3: intersetingDefParent for "<< initName->get_name ().getString() << " is "<<interestingDefParent->unparseToString()<<" and is of type "<<interestingDefParent->class_name()<<endl;
            //cout <<"4: next inreresting parent :" <<getNextParentInterstingNode(interestingDefParent->get_parent())->unparseToString()<< " and is of type "<<getNextParentInterstingNode(interestingDefParent->get_parent())->class_name()<<endl;
            
            defDepNode=NULL;
            // if the next interesting node is a function call, this definition comes from a ACTUAL_OUT
            if (isSgFunctionCallExp(getNextParentInterstingNode(interestingDefParent->get_parent()))) {
              defDepNode=getNode(DependenceNode::ACTUALOUT,interestingDefParent);
              establishEdge(defDepNode,useDepNode,DATA);                                                        
            } else if (isSgFunctionDeclaration(getNextParentInterstingNode(interestingDefParent->get_parent())))
              //)||                 isSgFunctionDefinition(interestingDefParent))
            {
              defDepNode=getNode(DependenceNode::FORMALIN,interestingDefParent);
              establishEdge(defDepNode,useDepNode,DATA);
            }/*
               else if (isSgFunctionParameterList(interestingDefParent) || isSgFunctionParameterList(interestingDefParent) ||
               isSgFunctionParameterList(defPlaces[j]->get_parent()) && isSgFunctionDeclaration(defPlaces[j]->get_parent()->get_parent()))         
               {
               //                                       cout <<"\t*isIntersting is FunctionParameterList"<<endl;
               //          checkForParameterChange=false;
               defDepNode=getNode(DependenceNode::FORMALIN,defPlaces[j]);
               establishEdge(defDepNode,useDepNode,DATA);
               }*/
            else {
              //                                        cout <<"\t*isIntersting is other use"<<endl;
              defDepNode=getNode(interestingDefParent);
              establishEdge(defDepNode,useDepNode,DATA);
            }
            
            // check if this variable is a variable from the function declaration, and if so mark that variable as dirty
            //
            //        if (checkForParameterChange)
          }
        }
        // we already marked def for iuse, now we have to mark idefs for iuse
        if (DUVariableAnalysisExt::isIUse(varRef))
        {
          //                    cout <<"\tIUSE"<<endl;*/
          usePlaces=defuse->getUseFor(varRef,initName);
          // this is a iUse find all uses before nad determnine if they are iDefs ...
          for (unsigned int j=0;j<usePlaces.size();j++) {
            // get the def, so we don't have to use the array all the time
            SgNode * use=usePlaces[j];
            // make shure it is the interesting def parent
            interestingDefParent=getNextParentInterstingNode(use);
            
            
            //                          cout<<"\tdef "<<j<<": "<<defPlaces[j]->unparseToString()<<endl;                         
            //                          cout<<"\tclass:"<<defPlaces[j]->class_name()<<endl;
            if (varRef==use) {
              //                                        cout <<"use and def in same node, skipping"<<endl;
              continue;
            } else if (DUVariableAnalysisExt::isIDef(use)) {
              // this is an idef, if the interesting paren is a function call get the
              interestingDefParent=getNextParentInterstingNode(use);
              //                                        cout <<"\tnextInteresting: "<<interestingDefParent->unparseToString()<<endl;
              
              if (isSgFunctionCallExp(getNextParentInterstingNode(interestingDefParent->get_parent()))) {
                //                                              cout <<"\t*isIntersting is FunctionCallExp"<<endl;
                // since the use is the sgVarRefexp traverse upwards to find the correct parameter in the functioncall
                defDepNode=getNode(DependenceNode::ACTUALOUT,interestingDefParent);
                establishEdge(defDepNode,useDepNode,DATA);                                                      
              } else {
                // iDefs can only be uses as functionparameters or direct iDefs
                defDepNode=getNode(interestingDefParent);
                establishEdge(defDepNode,useDepNode,DATA);
              }
            }                           
            
          }
        }

        
        if (DUVariableAnalysisExt::isIDef(varRef)) {
          //                    cout <<"\tIUSE"<<endl;
          //                    cout <<"\tIDEF"<<endl;
          defPlaces=defuse->getDefFor(varRef,initName);
          for (int j=0;j<defPlaces.size();j++) {
            //                          cout<<"\tdef "<<j<<": "<<defPlaces[j]->unparseToString()<<endl;
            //                          cout<<"\tclass:"<<defPlaces[j]->class_name()<<endl;
            interestingDefParent=getNextParentInterstingNode(defPlaces[j]);
            if (isSgFunctionParameterList(interestingDefParent) || isSgFunctionParameterList(interestingDefParent) ||
                isSgFunctionParameterList(defPlaces[j]->get_parent()) && isSgFunctionDeclaration(defPlaces[j]->get_parent()->get_parent()))
            {
              defDepNode=getNode(DependenceNode::FORMALIN,defPlaces[j]);
            }
            else
            {
              defDepNode=getNode(interestingDefParent);
            }
            establishEdge(defDepNode,useDepNode,DATA);
          }
        }
        
      }
          
          
      // force a dependence from the ACTUALOUT to the Functionscall
      // iterate over all function-calls to connect the actual in/out edges
      Rose_STL_Container< SgNode * > functionCall=NodeQuery::querySubTree(_head,V_SgFunctionCallExp);
      for (Rose_STL_Container< SgNode * >::iterator call=functionCall.begin(); call != functionCall.end();call++) {
        // track down the parent imporant node to establish a data-dependency
        DependenceNode * callDepNode,*returnNode;
        // actualreturn
          returnNode=getNode(DependenceNode::ACTUALRETURN,*call);
          SgNode* callTmp=(*call)->get_parent();               // if the paren is the interesting node or a call break
          while(!IsImportantForSliceSgFilter(callTmp) && !isSgFunctionCallExp(callTmp)) {
            callTmp=callTmp->get_parent();
          }
          // returnvalue is used in a function call, connect
          if (isSgFunctionCallExp(callTmp)) {
            // get the actual in
            SgNode *paramFindTmp=*call;
            while(!isSgExprListExp(paramFindTmp->get_parent())) {
              paramFindTmp=paramFindTmp->get_parent();
            }                 // paramFindTmp is now the parameter of the function call 
            callDepNode=getNode(DependenceNode::ACTUALIN,paramFindTmp);
          } else {
            callDepNode=getNode(callTmp);
          }
          assert(returnNode!=NULL);
          assert(callDepNode!=NULL);    
          establishEdge(returnNode,callDepNode,DATA);
        }
        
        // now process all functioncalls and their retunr-values
        
        
        //before ending, create return->formal-out edges
        Rose_STL_Container< SgNode * >returnStmts = NodeQuery::querySubTree(_head,V_SgReturnStmt);
        // if the last stmnt is not a return stmt, add that statement to the list, such that any modified formal_in parameters are linked correctly
        if (_head->get_body ()->get_statements ().size() &&
            !isSgReturnStmt(*(_head->get_body ()->get_statements ().rbegin()))) {
          
          // cout <<"last stmt is "<<(*(_head->get_body ()->get_statements ().rbegin()))->unparseToString()<<endl;
          // cout <<"last stmt is "<<_head->get_body ()->unparseToString()<<endl;
          //    cout <<"cfg says: "<<_head->get_declaration()->cfgForEnd ().getNode()->unparseToString()<<" is the end"<<endl;
          //   returnStmts.push_back(_head->get_declaration()->cfgForEnd ().getNode());
          // the last stmt might be a def, take the body as the sink for the CFG
          //    returnStmts.push_back(_head->get_body ());
          returnStmts.push_back(_head);
          //    returnStmts.push_back(*(_head->get_body ()->get_statements ().rbegin()));
        }
        
        // process return statements
        for (Rose_STL_Container< SgNode * >::iterator returnS=returnStmts.begin(); returnS != returnStmts.end();returnS++) {
          SgNode * stmt=*returnS;
          // cout <<"processing end-stantement: "<<stmt->unparseToString()<<endl;
          // if the return statement has actually some return values, connect those to the formal out
          if (isSgReturnStmt(stmt) && isSgReturnStmt(stmt)->get_expression ()!=NULL) {
            // cout <<"is return,creating out edge"<<endl;
            // check if the sub-expression of the return contains any function calls for wich we have to create data-dependencys .. later
            establishEdge(getNode(*returnS),getNode(DependenceNode::FORMALRETURN,_head->get_declaration()),DATA);
          } else {
            // the last stmt potentially modified s.th.
            // perform a complete DD-analysis for that statatement
            Rose_STL_Container< SgNode * > varRefList=NodeQuery::querySubTree(stmt,V_SgVarRefExp); 
            //      for (list<SgNode*>::refIt=varRefList.begin();refIt!=varRefList.end();refIt++)
            //    {
            // if this varaible is not from the initial name list
            //  }
          }
          
          // second step: check if any of the formal in-variables have been changes, if so, create a dependency from the specific cange point to the parameter
          SgInitializedNamePtrList  params=_head->get_declaration()->get_args ();
          // cout <<_head->get_declaration()->get_args ().size()<<" params "<<endl;
          for (SgInitializedNamePtrList::iterator paramIt=params.begin();
               paramIt!=params.end();paramIt++) {
            SgInitializedName * param=isSgInitializedName(*paramIt);
            if (param==NULL)
              exit(-1);
            vector < SgNode* > defList=defuse->getDefFor(stmt,param);
            // cout << "Processing "<<param->get_name ().getString()<<endl;
            // cout <<"  found "<<defList.size()<<" def(s)"<<endl;
            // check if it is the formal if
            for (unsigned int i=0;i<defList.size();i++) {
              if (defList[i]==param) {
                // IGNORE!!!
                // cout <<"def is formal in for formal out"<<endl;
              } else {
                // cout <<"\t"<<getNextParentInterstingNode(defList[i])->unparseToString()<<" is valid"<<endl;
                // connect the next interesting node ..
                establishEdge(getNode(getNextParentInterstingNode(defList[i])),getNode(DependenceNode::FORMALOUT,param),DATA);
                
              }
            }
            vector < SgNode* > useList=defuse->getUseFor(stmt,param);
            // cout <<"  found "<<useList.size()<<" uses"<<endl;
            // check if it is the formal if
            for (unsigned int i=0;i<useList.size();i++) {
              // get the statement
              // cout <<"\tuse use stmt is: "<<useList[i]->unparseToString() <<" next interesting:"<< getNextParentInterstingNode(useList[i])->unparseToString()<<endl;
              if (DUVariableAnalysisExt::isIDef(useList[i])) {
                establishEdge(getNode(getNextParentInterstingNode(useList[i])),getNode(DependenceNode::FORMALOUT,param),DATA);
                // cout <<"\t\tIDEF"<<endl;
                
              }
              //        if (DUVariableAnalysisExt::isIDef())
            }
          }
          //            establishEdge(getNode(*returnS),getNode(DependenceNode::FORMALOUT,_head->get_declaration()),DATA);
        }
        // since not all functionparameters may be requried for this function, but the stack has to be valid, force a data depenancy form the fucntion enrty to the formal in
        for (std::set<SgInitializedName*>::iterator param=functionParameterSet.begin();param!=functionParameterSet.end();param++) {
          //            cout <<"adding backedge for slicing purpose"<<endl;
          establishEdge(
                        getNode(DependenceNode::FORMALIN,*param),
                        getNode(DependenceNode::ENTRY,functionDef),
                        COMPLETENESS_HELPER);
        };
      }
      
    void DataDependenceGraph::computeInterproceduralInformation(InterproceduralInfo * ii)
    {
      // cout <<"since not all functionparameters may be requried for this function, but the stack has to be valid, force a data depenancy form the fucntion enrty to the formal in"<<endl;
    }
    
      void DataDependenceGraph::_buildDefUseChains(SgFunctionDefinition * fD)
      {

        SgBasicBlock *stmts = fD->get_body();
        AstInterfaceImpl astIntImpl(stmts);
        AstInterface astInt(&astIntImpl);
        StmtVarAliasCollect alias;

        alias(astInt, AstNodePtrImpl(fD));
        ReachingDefinitionAnalysis r;

        r(astInt, AstNodePtrImpl(fD));
        _defuse.build(astInt, r, alias);
      }

      //TODO
      void DataDependenceGraph::_processReturns()
      {

        // go through all the nodes of the graph. If any of them are return
        // statements, the formal_out return node should be linked to
        /*  set < SimpleDirectedGraphNode * >::iterator node_iterator;
            for (node_iterator = _nodes.begin(); node_iterator != _nodes.end(); node_iterator++)
            {
            DependenceNode *node = dynamic_cast < DependenceNode * >(*node_iterator);

            ROSE_ASSERT(node != NULL);
            if (node->getType() == DependenceNode::SGNODE)
            {
            SgNode *sgnode = node->getNode();

            if (isSgReturnStmt(sgnode))
            {
            DependenceNode *return_node =
            createNode(_interprocedural->procedureEntry.formal_return);

            establishEdge(node, return_node);
            }
            }
            }*/
      }

      SgFunctionCallExp *DataDependenceGraph::_findArgExprFromRef(SgNode * &funcArg, SgNode * use)
        {

          // move up the chain from funcArg until either the parent is an
          // SgExprListExp (i.e. the argument list) or is == use (i.e. funcArg
          // wasn't part of a function call)
          while (!isSgExprListExp(funcArg->get_parent()) && (funcArg->get_parent() != use))
            {
              funcArg = funcArg->get_parent();
            }

          // if it's == use, then return NULL
          if (funcArg->get_parent() == use)
            return NULL;

          // otherwise figure out which functioncallexp the argument is in
          SgFunctionCallExp *funcCall = isSgFunctionCallExp(funcArg->get_parent()->get_parent());

          ROSE_ASSERT(funcCall != NULL);

          return funcCall;

        }

