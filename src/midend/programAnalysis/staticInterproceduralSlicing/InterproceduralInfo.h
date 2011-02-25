#ifndef __INTERPROCEDURAL_INFO_H__ // added by HK
#define __INTERPROCEDURAL_INFO_H__ // added by HK

#include <AstInterface.h>
#include <StmtInfoCollect.h>
#include <ReachingDefinition.h>
#include <DefUseChain.h>
#include "CallGraph.h"
#include <ostream>
#include <string>
#include <map>
#include <utility>
#include <set>
#include "DominanceFrontier.h"
#include "SimpleDirectedGraph.h"

// #include "rose.h"
// #include "../newImDomImpl/filteredCFG.h"
#include <virtualCFG.h>
#include "filteredCFG.h"
#include "DominatorTree.h"
#include "DominanceFrontier.h"

// #define VERBOSE_DEBUG


/* ! \class InterproceduralInfo

   This class holds information necessary to perform interprocedural slicing.
   As specified in the paper by Horowitz et al, every function in a program
   requires: - An "entry" node - A "formal-in" node for each parameter - A
   "formal-out" node for each return parameter

   Similarly, every function call in a procedure requires: - A "call-site"
   node - An "actual-in" node for every argument to the function - An
   "actual-out" node for every return variable

   There should be one InterproceduralInfo object associated with each
   procedure in a program. It is initialized by passing in a newly created
   object to the constructor of a ControlDependenceGraph. It must then be
   passed to the constructor of a DataDependenceGraph. At this point, it
   contains all the appropriate intraprocedural edges and nodes as specified
   above.

 */

class InterproceduralInfo
{
        public:
        static SgNode * identifyEntryNode(SgFunctionDeclaration * dec)
        {       
                if (dec->get_definition()!=NULL)
                {
                        return dec->get_definition();
                }
                else
                {
                        return dec;
                }
        }
        static  SgNode * identifyEntryNode(SgFunctionDefinition * def)
        {
                return def;
        }
        public:
                // ! the nodes required to fully represent a call site in the PDG
    struct CallSiteStructure
    {
                                //! SgNode containing the functioncall, attention: this is not the SgFunctionCall-Node, this is the first node encountered form the functioncall towards the head of the function
                                SgNode* sliceImportantNode;
                                //! SgNode pointin to the actual SgFunctionCall-node
                                SgNode* sgFunctionCallExpNode;
        SgNode* sgCallInterestingNode;
        // ! the callsite - one per SgFunctionCallExp
        // ! the actual-in nodes - one per argument
                                std::vector<SgExpression*> actual_in;
//                              std::vector<SgExpression*
        // ! the actual-out nodes - one per argument
        // ! a list which records the order of the function arguments
  //      std::list < SgExpression * >expr_order;
        // ! an actual-out node representing the return value of the function
        // call
        SgNode *actual_return;
                };
    void setCallInterestingNode(int id,SgNode * node)
    {
      callSites[id].sgCallInterestingNode=node;
    }
    SgNode * getCallInterestingNode(int id)
    {
      return callSites[id].sgCallInterestingNode;
    }
                SgNode * getActualReturn(int id)
                {
                        return callSites[id].actual_return;                     
                }
                SgNode * getActualIn(int id,int varNr)
                {
                        return callSites[id].actual_in[varNr];
                }
                int getActualInCount(int id)
                {
                        return callSites[id].actual_in.size();
                }
                void addActualIn(int id,SgExpression * node)
                {
                        callSites[id].actual_in.push_back(node);
                }
                void setSliceImportantNode(int id,SgNode * node)
                {
                        callSites[id].sliceImportantNode=node;
                }
                void setActualReturn(int id,SgNode * node)
                {
                        callSites[id].actual_return=node;
                }
                
                //! returns the node for the function call, which contains the function call
                SgNode * getSliceImportantFunctionCallNode(int i)
                {
                        return callSites[i].sliceImportantNode;
                }
                std::set<SgNode *> getExitNodes()
                {
                        return exitNodes;
                }
                void addParameterToFunctionCall(SgNode * functionCall,SgExpression * param)
                {
                }

        int callSiteCount()
        {
                return callSites.size();
        }
        SgNode * getFunctionCallExpNode(int i)
        {
                return callSites[i].sgFunctionCallExpNode;
        }
        SgNode * getFunctionEntry()
        {
                return entry;
        }
        void setEllipse(SgNode * formal)
        {
                ellipseNode=formal;
        }
        SgNode* getEllipse()
        {
                return ellipseNode;
        }
        protected:
  
                SgFunctionDeclaration * decl;
                SgFunctionDefinition * def;
                SgNode * entry;
    
    // ! the nodes required to fully represent a procedure entry in the PDG
        // ! an entry node - one per function declaration
        // ! the formal-in nodes - one per function parameter
        // ! the formal-out nodes - one per function parameter
        // ! a list which records the order of the parameters
          std::list < SgInitializedName * >arg_order;
        // ! a formal out node representing the return value of the function
                                SgNode * formal_return;
                                std::vector<SgNode*> formal;
                                SgNode * ellipseNode;
                                // list containing the nodes from the function that exit...
                                std::set<SgNode *> exitNodes;
    
                // ! The entry node for a procedure
  //  ProcedureEntryStructure procedureEntry;
        std::vector<CallSiteStructure> callSites;
        
        std::map<SgNode *, int> callSitesMap;   
                

  public:
                bool isUndefined()
                {
                        if (def==NULL) return true;
                        else return false;
                }
                int getFormalCount()
                {
                        return formal.size();
                }
                SgNode * getFormal(int nr)
                {
                        if (formal.size()>(unsigned int)nr && nr>=0)
                        return formal[nr];
                        ROSE_ASSERT(false);

         // DQ (12/1/2009): avoid MSVC warning of non-void function without return stmt.
                        return NULL;
                }
        
                void setFormalReturn(SgNode *  node)
                {
                        formal_return=node;
                }
                SgNode * getFormalReturn()
                {
                        return formal_return;
                }
                // add this DependenceNode to the list of nodes which lead to exiting this function
                void addExitNode(SgNode * node)
                {                       
                        exitNodes.insert(node);
                }/*
                void createSafeConfiguration()
                {
                        DependenceNode * formalIn,*formalOut,*entry,*formalReturn;
                        entry=getNode(DependenceNode::ENTRY,getFunctionEntry());
                        formalReturn=getNode(DependenceNode::FORMALOUT,getFormalReturn());
                        
                        // this is done for "unknown" functions
                        for (int i=0;i<getFormalCount();i++)
                        {
                                formalIn=getNode(DependenceNode::FORMALIN,getFormal(i));
                                formalOut=getNode(DependenceNode::FORMALOUT,getFormal(i));
                                // create connection formalin->return
                                establishEdge(formalIn,entry,DATA);                     
                                establishEdge(formalIn,fornalOut,DATA);                 
                                establishEdge(formalIn,formalReturn,DATA);
                        }
                }
                */
                InterproceduralInfo(SgFunctionDeclaration* functionDeclaration)
                {       
#ifdef VERBOSE_DEBUG
                        std::cout << "creating interprocedural info for " << functionDeclaration->get_name().getString() << "\n"; 
#endif
                        decl=functionDeclaration;
                        def=functionDeclaration->get_definition();
                        if (def==NULL) entry=decl;
                        else entry=def;
                        // create formal stuff
                        formal_return=functionDeclaration;/*->get_type()->get_return_type();*/
                        ellipseNode=NULL;
                        Rose_STL_Container<SgInitializedName*> argList=functionDeclaration->get_args();
                        for (Rose_STL_Container<SgInitializedName*>::iterator i=argList.begin();i!=argList.end();i++)
                        {
                                if (isSgTypeEllipse((*i)->get_type()))
                                {
                                        ellipseNode=*i;
                                }
                                else
                                {
#ifdef VERBOSE_DEBUG
                                        std::cout << "\tadding formal in "<<*i<<"\n";
#endif
                                        formal.push_back(*i);
                                }
                        }
                        
                }
    
                /* ! \brief Gets the function declaration that the InterproceduralInfo object is for.
                   Returns: The SgFunctionDeclaration node that is associated with this object */
                SgFunctionDeclaration * foo(){return decl;}
                SgFunctionDefinition * getFunctionDefinition()
                {       return def;}
    SgFunctionDeclaration * getFunctionDeclaration()
    {
                        return decl;
    }
    
                // adds an function call to the tracking list, this will alter on be used to analyse the calls site in the ...
        //      void addFunctionCall(SgNode * sliceImportantNode,SgNode * functionCall,SgNode * actualReturnIdentifier=NULL)
                int addFunctionCall(SgNode * functionCall)
                {
                        CallSiteStructure cs;
                        cs.sliceImportantNode=NULL;//sliceImportantNode;
                        cs.sgFunctionCallExpNode=functionCall;
//                      cs.callSiteDepNode=NULL;
                        cs.actual_return=NULL;
                        callSites.push_back(cs);
                        callSitesMap[functionCall]=callSites.size()-1;
                        return callSites.size()-1;
                }

    /*! \brief Extracs all the function calls from an AST subtree which may contain function calls
       Params: - SgNode * node: The root of the subtree we want to analyze.
       Return: a list of all SgFunctionCallExp nodes that are in the subtree. */
/*    static std::list < SgFunctionCallExp * >extractFunctionCalls(SgNode * node)
    {
        std::list < SgFunctionCallExp * >retval;
        std::list < SgNode * >calls = NodeQuery::querySubTree(node, V_SgFunctionCallExp);
        for (std::list < SgNode * >::iterator i = calls.begin(); i != calls.end(); i++)
        {
            SgFunctionCallExp *fce = isSgFunctionCallExp(*i);
              ROSE_ASSERT(fce != NULL);
              retval.push_back(fce);
        }

        return retval;
    }*/
    // ! maps function calls to the call site structure that represents them
//  std::map < SgFunctionCallExp *, CallSiteStructure > callsite_map;
};

#endif // #ifndef __INTERPROCEDURAL_INFO_H__  // added by HK
