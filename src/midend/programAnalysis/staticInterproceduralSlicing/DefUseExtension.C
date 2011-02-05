// tps : Switching from rose.h to sage3 changed size from 19,6 MB to 9,3MB
#include "sage3basic.h"

#include <AstInterface.h>
#include <StmtInfoCollect.h>
#include <ReachingDefinition.h>
#include <DefUseChain.h>
#include <DirectedGraph.h>
#include "DependenceGraph.h"

#include "SlicingInfo.h"
#include "CreateSlice.h"
#include "ControlFlowGraph.h"
#include "DominatorTree.h"


#include <list>
#include <set>
#include <iostream>
#include "DFAnalysis.h"
#include "DefUseExtension.h"

#define DEBUG 1


using namespace DominatorTreesAndDominanceFrontiers;
using namespace std;
namespace DUVariableAnalysisExt
{
        bool isAssignmentExpr(SgNode*node)
        {
                if (isSgAssignOp(node)) return true;
                else if (isSgPlusAssignOp(node)) return true;
                else if (isSgMinusAssignOp(node)) return true;
                else if (isSgMultAssignOp(node)) return true;
                else if (isSgDivAssignOp(node)) return true;
                else if (isSgModAssignOp(node)) return true;
                else if (isSgAndAssignOp(node)) return true;
                else if (isSgXorAssignOp(node)) return true;
                else if (isSgIorAssignOp(node)) return true;
                else if (isSgRshiftAssignOp(node)) return true;
                else if (isSgLshiftAssignOp(node)) return true;
                return false;
        }
        bool isFunctionParameter(SgNode*node)
        {
                if (isSgExprListExp(node->get_parent()) && isSgFunctionCallExp(node->get_parent()->get_parent())) return true;
                return false;
        }
        bool isPointerType(SgVarRefExp * ref)
        {
                SgType * varType=ref->get_type();
                if (isSgArrayType(varType)|| isSgPointerType(varType))
                {
                        return true;
                }
                else
                {
                        return false;
                }
        }
        
        // is it a struct
        bool isComposedType(SgVarRefExp * ref)
        {
                SgType * varType=ref->get_type();
                 if (isSgClassType(varType))
                 {
                        return true;
                 }
                else
                {
                        return false;
                }               
        }
        
        bool isMemberVar(SgVarRefExp * ref)
        {
                SgNode * parent=ref->get_parent();
                return isSgDotExp(parent)||isSgArrowExp(parent);
        }
        
  SgNode * getNextParentInterstingNode(SgNode* node)
  {
    SgNode * tmp= NULL;
    for(tmp = node; tmp != NULL; tmp=tmp->get_parent()) {
      
      // Variable references themselves are not interesting.
      if(isSgVarRefExp(tmp)) {
        continue;
      }

      // interesting are any parts that have to be in the CFG
      if (IsImportantForSliceSgFilter(tmp)) 
          break;
      // function calls are importat
      if (isSgFunctionCallExp(tmp)) 
          break;
      // function arguments
      if (tmp->get_parent() && 
          isSgExprListExp(tmp->get_parent()) && 
          isSgFunctionCallExp(tmp->get_parent()->get_parent())
          ) 
          break;
      // parameters
      if (isSgInitializedName(tmp)&& isSgFunctionParameterList(tmp->get_parent()))
          break;
      if (isSgFunctionDeclaration(tmp)) break;
      if (isSgFunctionDefinition(tmp))
        break;
/*    while(!IsImportantForSliceSgFilter(tmp) && // get the filtered nodes from the CFG
        !isSgFunctionCallExp(tmp) && // get function calls
        !isSgFunctionDefinition(tmp) && // keep function definitions
        !tmp->get_parent() && !isSgExprListExp(tmp->get_parent()) &&!isSgFunctionCallExp(tmp->get_parent()->get_parent())&&// next interesting: actula in & out
        !isSgInitializedName(tmp)&&tmp->get_parent()&&isSgFunctionDeclaration(tmp->get_parent()))
        )

      //ddwhile(!IsImportantForSliceSgFilter(tmp) && !isSgFunctionCallExp(tmp) &&!isSgFunctionDefinition(tmp))        
    {*/
     }
      return tmp;
  }
#if 0   
        // this function returns true if the variable defined by SgNode is defined
        bool isDef(SgNode * node,bool treadFunctionCallAsDef)
        {
                bool assignCand=false;
                // Straight assignments of a variable
                //      if (isAssignInitializer(node->get_parent()))   assignCand=true;;
                if (isSgAssignOp(node->get_parent()))           assignCand=true;;
                // BINARY-OPERATORS: combined assignment and use
                if (isSgPlusAssignOp(node->get_parent()))   assignCand=true;;
                if (isSgAndAssignOp(node->get_parent()))   assignCand=true;;
                if (isSgDivAssignOp(node->get_parent()))   assignCand=true;;
                if (isSgIorAssignOp(node->get_parent()))   assignCand=true;;
                if (isSgLshiftAssignOp(node->get_parent()))   assignCand=true;;
                if (isSgMinusAssignOp(node->get_parent()))   assignCand=true;;
                if (isSgModAssignOp(node->get_parent()))   assignCand=true;;
                if (isSgMultAssignOp(node->get_parent()))   assignCand=true;;
                if (isSgRshiftAssignOp(node->get_parent()))   assignCand=true;;
                if (isSgXorAssignOp(node->get_parent()))   assignCand=true;;
                if (assignCand==true)
                {
                        if (isSgBinaryOp(node->get_parent())->get_lhs_operand()==node)
                                return true;
                }
                // this variable is a function call parameter
                // this is only works for C since C++ supports calls by reference
                SgNode* currentParent=node->get_parent();
                //  there is only 1 way for a function to change the value of a variable -> addressOf
                if (isSgAddressOfOp(currentParent))
                {                                               
                        if (isSgExprListExp(currentParent->get_parent()) && isSgFunctionCallExp(currentParent->get_parent()->get_parent())) return true;
//              if (treadFunctionCallAsDef &&isSgExprListExp(node->get_parent()) && isSgFunctionCallExp(node->get_parent()->get_parent())) return true;
                }
                
                if (isSgPlusPlusOp(node->get_parent()) || isSgMinusMinusOp(node->get_parent())) 
                        return true;
                return false;
        }
        
        bool isDef(SgNode * node)
        {
                return isDef(node,true);
        }
                
        
        // standard return value is true, find all instances where it will be false
        bool isUse(SgNode * node)
        {
                bool isBinary=false;
                // Straight assignments of a variable
                if (isSgAssignOp(node->get_parent())&& isSgBinaryOp(node->get_parent())->get_lhs_operand()==node) return false;
                /*
                // BINARY-OPERATORS: combined assignment and use
                if (isSgPlusAssignOp(node->get_parent()))   isBinary=true;
                if (isSgAndAssignOp(node->get_parent()))   isBinary=true;
                if (isSgDivAssignOp(node->get_parent()))   isBinary=true;
                if (isSgIorAssignOp(node->get_parent()))   isBinary=true;
                if (isSgLshiftAssignOp(node->get_parent()))   isBinary=true;
                if (isSgMinusAssignOp(node->get_parent()))   isBinary=true;
                if (isSgModAssignOp(node->get_parent()))   isBinary=true;
                if (isSgMultAssignOp(node->get_parent()))   isBinary=true;
                if (isSgRshiftAssignOp(node->get_parent()))   isBinary=true;
                if (isSgXorAssignOp(node->get_parent()))   isBinary=true;
                if (isBinary)
                {
                        // for a binary operator this is a use in anycase
                        return true;                    
                }*//*
                if (isSgPlusPlusOp(node->get_parent()) || isSgMinusMinusOp(node->get_parent())) 
                {
                        return true;
                }*//*
                // if the paren is an addressOfOperator check if the parent of that is an function call
                if (isSgAddressOfOp(node->get_parent()) && isSgExprListExp(node->get_parent()->get_parent()) && isSgFunctionCallExp(node->get_parent()->get_parent()->get_parent()))
                {
                        // foor(&node)
                        return true;
                }*/
                SgNode * parent=node->get_parent();
                // what is left.. the variable itself is used as an statement
        /*      if (isSgStatement(parent))
                        return true;*/
                if (isSgDotExp(parent)||isSgArrowExp(parent)||isSgPntrArrRefExp(parent))
                {
                        return false;
                }
                return true;    
        }


        bool isIDef(SgNode * node)
        {
                if (!isSgVarRefExp(node))
                {
                        cerr<<"this is not a SgVarRefExpr"<<endl;
                        return false;
                }
                if (isDef(node,false)) return false;
                bool lValue,undecided;
                undecided=true;
                // assum this identifyer is a lvalue
                lValue=true;
                int depth=0;
                SgNode * parent=node->get_parent(),
                                         * current=node;
                while(undecided)
                {
//                      cout <<"\t"<<depth<<": "<<parent->unparseToString()<<endl;

                        if (isAssignmentExpr(parent))
                        {       
                                // if the paren is an assignment
                                assert(depth!=0);
                                // and the current node is on the left hand side of the assignment, then it is a lvalue
                                if (isSgBinaryOp(parent)->get_lhs_operand()==current)
                                {
                                        // is an lvalue and on the lhs
                                        return true;
                                }
                                else
                                {
                                        // is on the right hand side
                                        return false;
                                }
                        }
                        //CI (03/27/2007): I am not that shure about that anymore
                        else if (isFunctionParameter(parent))
                        {
                                assert(depth!=0);
                                return true;
                        }
                        // if the paren is a statement, then there is no assignmetn
                        else if (isSgStatement(parent))
                        {
                                return false;
                        }
                        else if (isSgDotExp(parent)||isSgArrowExp(parent))
                        {
                                if (!(isSgBinaryOp(parent)->get_lhs_operand()==current))
                                {
                                        return false;
                                }
                        }
                        
                        current=parent;
                        parent=parent->get_parent();
                        depth++;

                }
                return false;
        }
        bool isIUse(SgNode* node)
        {
                if (!isSgVarRefExp(node))
                {
                        cerr<<"this is not a SgVarRefExpr"<<endl;
                        return false;
                }
                if (isDef(node,false)) return false;
                bool undecided;
                undecided=true;
                // assum this identifyer is a lvalue
                bool isIndirect=false;
                bool isUse=true,isDef=true,isAddressOf=false;
                int depth=0;
                SgNode * parent=node->get_parent(),
                                         * current=node;
                                         
                while(undecided)
                {
                        
                        cout <<"\t"<<depth<<": "<<parent->unparseToString()<<endl;

                        if (isAssignmentExpr(parent))
                        {       
                                // and the current node is on the left hand side of the assignment, then it is a lvalue
                                if (isSgBinaryOp(parent)->get_lhs_operand()==current)
                                {
                                        // this is a assignment
                                        undecided=false;
                                        isUse=false;
                                        break;
                                }
                                else
                                {
                                        // we are on the right hand side
                                        undecided=false;
                                        isDef=false;
                                        break;
                                }
                        }
                        else if (isSgAddressOfOp(parent))
                        {
                                isAddressOf=true;
                                
                        }
                        //CI (03/27/2007): I am not that shure about that anymore
                        else if (isSgExprListExp(parent) && isSgFunctionCallExp(parent->get_parent()))
                        {

                                if (isAddressOf)
                                {
                                        
                                }
                                // we are in a parameter list ...
                                // if current is an address 
                                
                                return true;
                        }
                        // if the paren is a statement, then there is no assignmetn
                        else if (isSgStatement(parent))
                        {
                                undecided=false;
                                break;
                        }
                        else if (isSgPntrArrRefExp(parent))
                        {
                                // is is used inside of []
                                // ..[node] is a use
                                if (isSgBinaryOp(parent)->get_rhs_operand()==current)
                                {
                                        isDef=false;
                                        undecided=false;
                                        break;
                                }
                                else
                                {
                                        // node[?]
                                        isIndirect=true;
                                }
                        }
                        else if (isSgDotExp(parent)||isSgArrowExp(parent))
                        {
                                // lhs->node and lhs.node is no use -> false
                                // if we are on the lhs and we are 
                                if (!(isSgBinaryOp(parent)->get_lhs_operand()==current))
                                {
                                        // we are on the rhs... if we are still an lvalue return true
                                        undecided=false;
                                        isDef=false;
                                        isIndirect=false;
                                        break;
                                }
                                else
                                {
                                        // lhs of -> or .
                                        isIndirect=true;
                                        isDef=true;
                                }                               
                        }
                        
                        current=parent;
                        parent=parent->get_parent();
                        depth++;

                }
                if (isDef)
                        return false;
                else
                if (isIndirect)
                        return true;
                return false;           
        }
#endif

        bool calcUseDefs(SgNode* node,bool getDef,bool getIndirect)
        {
                if (!isSgVarRefExp(node))
                {
                        cerr<<"this is not a SgVarRefExpr"<<endl;
                        return false;
                }
                int depth=1;
                SgNode * parent=node->get_parent(),
                                         * current=node;
                bool finished=false;                    
                bool indirect=false,def[2]={false,false},use[2]={false,false};
                bool addressOf=false;
                bool isVariable=true;   
                finished=isSgStatement(parent);
                while(!finished)
                {
                //      cout <<"\t"<<depth<<": "<<parent->unparseToString()<<endl;
                        if (isAssignmentExpr(parent) && isSgBinaryOp(parent)->get_lhs_operand()==current)
                        {
                                // this is a def
                                def[indirect]=true;
                                finished=true;
                                if (!isSgAssignOp(parent)) use[indirect]=true;
                        }
                        // where in the -> or . op are we
                        else if (isSgDotExp(parent) || isSgArrowExp(parent))
                        {
                                if (isSgBinaryOp(node->get_parent())->get_lhs_operand()==current)
                                {
                                        indirect|=true;
                                }
                                else
                                {
                                        isVariable=false;
                                        finished=true;
                                }
                        }
                        // array deref op a[]
                        else if (isSgPntrArrRefExp(parent))
                        {
                                if (isSgBinaryOp(node->get_parent())->get_lhs_operand()==current)
                                {
                                        indirect|=true;
                                }
                                else
                                {
                                        use[indirect]=true;
                                        finished=true;
                                }
                        }
                        // this rule has to be thought over regarding indirect definitions
                        else if (isSgPlusPlusOp(parent)||isSgMinusMinusOp(parent))
                        {                               
                                def[indirect]=true;
                                use[indirect]=true;
                        }
                        else if (isSgExprListExp(parent))
                        {
                                if (isSgFunctionCallExp(parent->get_parent()))
                                {
                                        // the indirect use of this variable is possible, as loing it is one that is capaple of doing so
                                        if (isSgAddressOfOp(current))
                                        {
                                                // the parameter has been passed indirectly
                                                def[0]=true;
                                                use[0]=true;
                                                if (isPointerType(isSgVarRefExp(node)))
                                                {
                                                def[1]=true;
                                                use[1]=true;
                                                }
                                        }
                                        if (isPointerType(isSgVarRefExp(node)))
                                        {
                                        
                                                indirect=true;
                                                def[1]=true;
                                                            use[1]=true;
                                        }
                                        if (isComposedType(isSgVarRefExp(node)))
                                        {
                                                use[0]=use[1]=1;
                                        }
                                        finished=true;
                                }
                        }
                        else if (isSgPointerDerefExp(parent))
                        {
                                indirect=true;
                        }
                        else if (isSgAddressOfOp(parent))
                        {
                                // the return of this is an address, this is not ok unless used for passing parameters to functions
                //              if (!isSgExprListExp(parent->get_parent()))
                        //      {
                                        cerr<<"the & operator is used on a variable"<<endl;
                                        addressOf=true;
                                        // from this point on this may be anything!!!
          use[0]=use[1]=1;
                def[0]=def[1]=1;
//                      cerr<<"the & operator is not impleted for cases other than passing addresses to functions"<<endl;
//                      exit(-1);
                                        
                //              }
                //              else
                //              {
                                        // the addresOfOperator is used like foo(&LValue)
                                        
                                
                        //      }
                        }
                        
                        //do the traversal towards the top
                        current=parent;
                        parent=parent->get_parent();
                        depth++;
//                      cout  << "u " <<use<<" d "<<def[0]<<" id "<<def[1]<<" i "<<indirect<<endl;

                        if (isSgStatement(parent)) finished=true;
                }
                // if there is no def (indirect or direct) set the according use
                if (isVariable &&!def[0] && !def[1])
                        use[indirect]=true;
        /*      
                if (use[0])
                        cout <<"\t*is Use"<<endl;
                if (use[1])
                        cout <<"\t*is iUse"<<endl;
                if (def[0])
                        cout <<"\t*is Def"<<endl;
                if (def[1])
                        cout <<"\t*is iDef"<<endl;
                */
                if (getDef)
                        return def[getIndirect];
                else 
                        return use[getIndirect];
                
                return false;           
        }
        bool isDef(SgNode * n)
        {
                return  calcUseDefs(n,true,false);              
        }
        bool isIDef(SgNode * n)
        {
                return  calcUseDefs(n,true,true);               
        }
        bool isUse(SgNode * n)
        {
                return  calcUseDefs(n,false,false);             
        }
        bool isIUse(SgNode * n)
        {
                return  calcUseDefs(n,false,true);              
        }
        bool functionUsesAddressOf(SgVarRefExp * node,SgFunctionCallExp * call)
        {
                SgNode * parent=node->get_parent(),
                                         * current=node;
                bool finished=false;                    
                finished=isSgStatement(parent);
                while(parent!=call)
                {
                //      cout <<"\t"<<depth<<": "<<parent->unparseToString()<<endl;
                        if (isAssignmentExpr(parent) && isSgBinaryOp(parent)->get_rhs_operand()==current)
                        {
                                // this is a def
                                return false;
                        }
                        // where in the -> or . op are we
                        else if (isSgDotExp(parent) || (isSgArrowExp(parent) && isSgBinaryOp(node->get_parent())->get_rhs_operand()==current))
                        {                       
                                        return false;
                        }
                        // array deref op a[]
                        else if (isSgPntrArrRefExp(parent))
                        {
                                if ((isSgBinaryOp(node->get_parent())->get_rhs_operand()==current))
                                {
                                        return false;
                                }
                        }
                        // this rule has to be thought over regarding indirect definitions
                        else if (isSgExprListExp(parent))
                        {
                                return parent;
                        }
                        else if (isSgAddressOfOp(parent))
                        {
                                return true;
                        }
                        //do the traversal towards the top
                        /**/
                        current=parent;
                        parent=parent->get_parent();
                        if (isSgStatement(current))
                        {
                                cerr<<"another statement reached than spedcified with sgfunctioncallexp"<<endl;
                                // segfault
                                ROSE_ASSERT(false);
                                exit( -1);
                        }
                }
                return false;
        }
}
