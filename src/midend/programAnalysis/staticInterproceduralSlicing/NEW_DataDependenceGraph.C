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


#ifdef NEWDU
DataDependenceGraph::DataDependenceGraph(SgNode * head,EDefUse * du, InterproceduralInfo * ii):_head(isSgFunctionDefinition
                                                                                                     (head))
{
  defuse=du;
#else
  DataDependenceGraph::DataDependenceGraph(SgNode * head, InterproceduralInfo * ii):_head(isSgFunctionDefinition
                                                                                          (head))
    {
      _buildDefUseChains(_head);
#endif
        _interprocedural = ii;
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
    










    //construct the data dependence graph
    void DataDependenceGraph::buildDDG()
    {

         //for debugging purposes, print out the def and use maps
         //defuse->printDefUse();


      //iterate over all variable references in the function
      DependenceNode *defDepNode=NULL;
      DependenceNode *useDepNode=NULL;

      // this graph is intersted of all ueses of variables, up so far NO POINTER analysis is done...
      Rose_STL_Container< SgNode * > varUse=NodeQuery::querySubTree(_head,V_SgVarRefExp);

      for (Rose_STL_Container< SgNode * >::iterator i = varUse.begin();i!=varUse.end();i++) {
        // if the variable is a global, special treatmen
    
        SgVarRefExp * varRef = isSgVarRefExp(*i);
        SgNode * varDef;
        SgNode * interestingUseParent,
          * interestingDefParent;
        
        
        // get the correct interesting node for the use
        // uw-mist TODO: rewrite the getnextparent
        interestingUseParent=getNextParentInterstingNode(varRef);


        //Now, grab the proper use node in the system dependence graph, or create it if it doesn't exist
        //uw-mist replaced the call
        //if (isSgFunctionCallExp(getNextParentInterstingNode(interestingUseParent->get_parent()))) 
        if (isSgFunctionCallExp(interestingUseParent)) {
          // check if the variable is used as argument in a function call (special case in system dependence graph)
          useDepNode=getNode(DependenceNode::ACTUALIN,varRef);

        } else if (isSgFunctionDefinition(interestingUseParent)) {
          continue;
        } else {
          useDepNode=getNode(interestingUseParent);
        }


        // the current node is defined, next get the defining nodes
        
        //Jim: What's the point of jumping straight to the decleration here?
        //We should want the def. from the defuse chains...
        std::vector< SgNode* > defPlaces,usePlaces;
        SgInitializedName *initName=varRef->get_symbol()->get_declaration();
        //VERBOSE_DEBUG_STMT(std::string varName=initName->get_name().getString();)
        

        // handle global variable definition
        if (defuse->isNodeGlobalVariable(initName)) {
          //            initName->get_declaration (); 
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
        


        //Main Case: Variable Use
        //    if the current use of the variable is a use
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
            // make sure it is the interesting def parent
            interestingDefParent=getNextParentInterstingNode(varDef);



            
            defDepNode=NULL;

            // if the next interesting node is a function call, this definition comes from a ACTUAL_OUT
            if (isSgFunctionCallExp(getNextParentInterstingNode(interestingDefParent->get_parent())) ||
                isSgFunctionCallExp(getNextParentInterstingNode(interestingDefParent)) ||
                isSgFunctionCallExp(interestingDefParent) ||
                isSgFunctionCallExp(interestingDefParent->get_parent()) ||
                isSgFunctionCallExp(interestingDefParent->get_parent()->get_parent()) ) {

                    //establish edge from function call to actual out
                    defDepNode=getNode(DependenceNode::ACTUALOUT,interestingDefParent);
                    establishEdge(defDepNode,useDepNode,DATA);                            
            // if the next node is a function definition
            } else if (isSgFunctionDefinition(interestingDefParent))
            {
                    //connect edge from function definition to formal in
                    defDepNode=getNode(DependenceNode::FORMALIN,interestingDefParent);
                    establishEdge(defDepNode,useDepNode,DATA);
            }
            // if the next node is a function parameter arg list
            else if (isSgFunctionParameterList(interestingDefParent->get_parent())) {
                    

                    //connect the function args to the formal in nodes
                    defDepNode=getNode(DependenceNode::FORMALIN,interestingDefParent);
                    establishEdge(defDepNode,useDepNode,DATA);
            }

            else {
                //assume a general data edge
                defDepNode=getNode(interestingDefParent);
                establishEdge(defDepNode,useDepNode,DATA);
            }
          }
        }






        //NOW SWITCH from wanting Uses to Defs:

        //Additional Dependence Graph Initialization:
        //connect formal in defs to variable uses
        if (DUVariableAnalysisExt::isIDef(varRef)) {
          //            cout <<"\tIUSE"<<endl;
          //            cout <<"\tIDEF"<<endl;
          defPlaces=defuse->getDefFor(varRef,initName);
          for (unsigned int j=0;j<defPlaces.size();j++) {
            //                cout<<"\tdef "<<j<<": "<<defPlaces[j]->unparseToString()<<endl;
            //                cout<<"\tclass:"<<defPlaces[j]->class_name()<<endl;
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
        
      } //END OF MAIN VARIABLE LOOP


        //FINAL DEPENDENCE GRAPH STEPS:


       //uw-mist
       //begin by connecting all ActualOut nodes to the function args
       //start by getting all callsites representing a function
      for (int i=0;i<_interprocedural->callSiteCount();i++)
      {  
        //for this individual function, visit each arg
        for (int j=0;j<_interprocedural->getActualInCount(i);j++)
        {
            SgNode * actOut = _interprocedural->getActualIn(i,j);

        //if it is not a reference expression, search to find the lower reference expressions in the complex arg
        //uw-mist TODO: (If neccessary) for now, just skip
            if(!isSgVarRefExp(actOut))
                continue;

        //search the def/use map
            SgInitializedName *actOutInitName=isSgVarRefExp(actOut)->get_symbol()->get_declaration();
        DependenceNode * actOutDep = getNode(DependenceNode::ACTUALOUT,actOut);

        //connect to all potential definitions (not as exact as possible, but should work for first pass
        //uw-mist TODO: connect directly to all potential use paths instead
            std::vector< SgNode* > defPlaces=defuse->getDefFor(actOut,actOutInitName);
            for (unsigned int k=0;k<defPlaces.size();k++) {


                    establishEdge(actOutDep,getNode(getNextParentInterstingNode(defPlaces[k])),DATA);
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
        

        
        //Now connect return statements:

        //first, build up list of return statements
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
          if (isSgReturnStmt(stmt) /*&& isSgReturnStmt(stmt)->get_expression ()!=NULL*/) {
            // cout <<"is return,creating out edge"<<endl;
            // check if the sub-expression of the return contains any function calls for wich we have to create data-dependencys .. later

            Rose_STL_Container< SgNode * > returnRefs=NodeQuery::querySubTree(stmt,V_SgVarRefExp); 
                for (Rose_STL_Container<SgNode*>::iterator refIt=returnRefs.begin();refIt!=returnRefs.end();refIt++)
            {
                            establishEdge( getNode(getNextParentInterstingNode(*refIt)), getNode(DependenceNode::FORMALRETURN,_head->get_declaration()), DATA);
            // if this varaible is not from the initial name list
              }
    
            //SgVarRefExp * varRef=isSgVarRefExp(*i);

           }


        }


        // since not all functionparameters may be requried for this function, but the stack has to be valid, force a data depenancy form the fucntion enrty to the formal in
        std::set<SgInitializedName*> functionParameterSet(_head->get_declaration()->get_args().begin(),_head->get_declaration()->get_args().end());
        for (std::set<SgInitializedName*>::iterator param=functionParameterSet.begin();param!=functionParameterSet.end();param++) {
          //        cout <<"adding backedge for slicing purpose"<<endl;
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


