#include <general.h>

#include <CFGImpl.h>
#include <PtrMap.h>
#include <ProcessAstTree.h>
#include <CommandOptions.h>
#include <iostream>
#include "rose.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/8/2006): Since this is not used in a heade file it is OK here!
#define Boolean int

bool debug_cfg();

#ifndef TEMPLATE_ONLY
bool debug_cfg()
{
  static int r = 0;
  if (r == 0) {
    if ( CmdOptions::GetInstance()->HasOption("-debugcfg"))
        r = 1;
    else
        r = -1;
  }
  return r == 1;
}
#endif

#ifndef TEMPLATE_ONLY
string CFGConfig::EdgeType2String( EdgeType e)
{
  switch (e) {
  case COND_TRUE: return "true";
  case COND_FALSE: return "false";
  case ALWAYS: return "always";
  default:
     assert(false);
  }
}
#endif

template <class Node, class Edge>
class BuildCFGTraverse : public ProcessAstTree
{
 public:
  BuildCFGTraverse( BuildCFGConfig<Node,Edge>& g) : graph(g), node(0), stmtnum(0) {}
  void operator()(  AstInterface& fa, const AstNodePtr& head )
     { 
       ProcessAstTree::operator()(fa, head);
     }
// DQ (1/28/2006): Removed const function parameter 
// (suggested by Michelle Strout as part of compilation on 64 bit Opteron processor)
// void operator() ( AstInterface& fa, const AstInterface::AstNodeList& stmts)
   void operator() ( AstInterface& fa, AstInterface::AstNodeList& stmts)
     {
       for (AstInterface::AstNodeListIterator p = fa.GetAstNodeListIterator(stmts);
            !p.ReachEnd(); ++p) {
          operator()(fa, *p);
       }
     }


 private:
  BuildCFGConfig<Node,Edge>& graph;
  Node* node; 
  unsigned stmtnum;


  // map AstNode pointers to graph node pointers: 
  //   each compound statement (eg, loop, if, basic-block) maps to the last graph node containing its statements  
  // startMap  and exitMap map stmt to the start graph node and to the exit (next)graph node respectively
  PtrMapWrap<void, Node> startMap, exitMap;

  typedef enum {START, EXIT} MapType;

  bool IsBodyEmpty( AstInterface&fa, const AstNodePtr& s)
  {
    if (s == 0)
        return true;
    AstNodePtr first = 0;
    if (fa.IsBasicBlock(s) && (first = fa.GetBasicBlockFirstStmt(s)) == 0) {
      if (debug_cfg())
         cerr << "block " << fa.AstToString(s) << " is empty " << endl;
      return true;
    }
    else {
      if (debug_cfg()) {
         cerr << "block " << fa.AstToString(s) << " is not empty " ;
         if (first != 0) 
            cerr << "first statement: " << fa.AstToString(first) << endl;
         else
            cerr << endl;
      }
    }
    return false; 
  }

  Node* GetStmtNode( const AstNodePtr& s, MapType t ) 
   {
      switch (t) {
      case EXIT:
       return exitMap.Map(s);
      case START:
        return startMap.Map(s);
      } 
      assert(false);
      return 0;
   }
  void SetStmtNode( const AstNodePtr& s, Node* n, MapType t)
   {
      if (debug_cfg()) {
         cerr << "mapping stmt " << ((t == START)? "START" : "EXIT");
         cerr << AstInterface::AstToString(s) << " to " << n << endl;
      }
      switch (t) {
      case EXIT:
         exitMap.InsertMapping(s, n); break;
         break;
      case START:
         startMap.InsertMapping(s, n);
         break;
      default:
         assert(false);
      };
   }
      // map s to a CFG node: create a new one if not already mapped 
  Node* MapStmt( const AstNodePtr& s, MapType t, bool add = false) 
   {
     // printf("In MapStmt %p (%s) on map %s add=%s\n", (SgNode*)s, s->class_name().c_str(), (t == START ? "START" : "END"), (add ? "true" : "false"));
     assert(s != 0);
     Node *n = GetStmtNode(s,t);
     if (n != 0) 
         return n;
         
      n = graph.CreateNode();
      if (debug_cfg()) {
         cerr << " create node : " << n << endl; 
      }
      if (add) {
          assert ( t==START);
           AddStmt(n, s);
          SetStmtNode( s, n, START);
       }
       else
         SetStmtNode(s,n,t);
     return n;
   }
  Node* AddStmt( Node* n, const AstNodePtr& s) // add s to the node n
    { 
       assert(s != 0 && n != 0);

       // printf ("In AddStmt(%s) \n",s->class_name().c_str());
       if (AstInterface::IsBasicBlock(s)) {

       // printf ("s->class_name() = %s \n",s->class_name().c_str());

          AstInterface::AstNodeList l = AstInterface::GetBasicBlockStmtList(s);

       // DQ (3/16/2006): If the SgForStatment has an empty SgForInitStatement then "graph.AddNodeStmt(n,*p);" is never called
          ROSE_ASSERT(l.size() > 0);

          for (AstInterface::AstNodeListIterator p = AstInterface::GetAstNodeListIterator(l);
               !p.ReachEnd(); ++p) {
              graph.AddNodeStmt(n,*p);
          }
       }
       else
          graph.AddNodeStmt(n, s); 

       if (debug_cfg()) {
          cerr << " add stmt " << AstInterface::AstToString(s);
          cerr << " to node " << n << endl;
       }

       if (node == n)
          ++stmtnum;

       SetStmtNode(s,n,START);

       return n;
    }

  void AddBranch( Node *cur, Node* dest, CFGConfig::EdgeType val)
    { 
       assert(cur != 0 && dest != 0);
       if (cur == dest) {
          assert(val == CFGConfig::ALWAYS);
       }
       else {
          graph.CreateEdge(cur, dest, val);
          if (debug_cfg()) 
             cerr << "add " << CFGConfig::EdgeType2String(val) << " edge from " << cur << " to " << dest << endl;
       }
    }


  bool IsCurNodeEmpty() const { return !stmtnum; }

  Node* GetCurNode( bool createnew = true)  // return the current CFGConfig::node 
   {
      if (node == 0 && createnew) {
        node = graph.CreateNode();
        if (debug_cfg()) 
           cerr << "create node in GetCurNode " << node << endl;
        stmtnum = 0;
      }
      if (debug_cfg()) 
        cerr << "current node : " << node << endl;
      return node;
   }
  void SetCurNode( Node *n)
   {
     if (debug_cfg()) {
        if (n == 0)
           cerr << "resetting current node to 0 \n"; 
     }
     node = n;
     stmtnum = 0;
   }

  virtual Boolean ProcessDecls(AstInterface &fa, const AstNodePtr& s)
                                { ProcessStmt(fa, s); return true;}

  virtual Boolean ProcessFunctionDefinition( AstInterface &fa, const AstNodePtr& s,
                                           const AstNodePtr& body,
                                     AstInterface::TraversalVisitType t)
   {
     if (debug_cfg()) {
        cerr << "processing function def " ; 
        cerr << ((t == AstInterface::PreVisit)? " previsit " : " postvisit ") << endl;
     }
     if (t == AstInterface::PreVisit) {
            Node *exit = GetCurNode(true);
            SetStmtNode(s, exit, EXIT);
            SetStmtNode(body, exit, EXIT);
            SetCurNode(0);
            Node* entry = GetCurNode(true);
            SetStmtNode(s, entry, START);
     }
     else {
           Node* lastNode = GetCurNode(false);
           if (lastNode != 0)
              AddBranch(lastNode, GetStmtNode(s, EXIT), CFGConfig::ALWAYS); 
     }
     return  ProcessAstTree::ProcessFunctionDefinition(fa, s, body, t);
   }

  virtual Boolean ProcessLoop(AstInterface &fa, const AstNodePtr& s, const AstNodePtr& body,
                               AstInterface::TraversalVisitType t)
     {
        Node *lastNode = GetCurNode();
        SetStmtNode(s, lastNode, START);
        Node *exitNode = MapStmt(s, EXIT);

     if (debug_cfg()) {
        cerr << "processing loop ";
        cerr << ((t == AstInterface::PreVisit)? " previsit " : " postvisit ") << endl;
     }
       AstNodePtr init, cond, incr;
       if (!fa.IsLoop( s, &init, &cond,&incr))
            assert(false);

     // DQ (3/16/2006): Added assertion to help debug init being an empty SgForInitStatement
     // ROSE_ASSERT(*init != NULL);
     // ROSE_ASSERT(*cond != NULL);
     // ROSE_ASSERT(*incr != NULL);
#if 0
        init.Dump();
        if (init != NULL)
           {
             printf ("\ninit is valid pointer %p = %s \n",isSgNode(init),init->class_name().c_str());
           }
          else
           {
             printf ("\ninit is NULL pointer %p \n",isSgNode(init));
           }
        printf ("lastNode = %p \n",lastNode);
#endif

        if (t == AstInterface::PreVisit) {

        // DQ (3/18/2006): Only build a basic block where there are statements to support it
        // if (init != 0) 
           if (init != 0 && fa.GetBasicBlockSize(init) > 0) 
              AddStmt(lastNode, init);

           bool isPostLoop = fa.IsPostTestLoop(s);
           Node* bodyNode = (isPostLoop && IsCurNodeEmpty())? SetStmtNode(body, lastNode, START), lastNode 
                                                        : MapStmt(body, START);
           Node* testNode = 0;
           if (cond != 0) {
              testNode = (!isPostLoop && IsCurNodeEmpty())? AddStmt(lastNode, cond), lastNode : MapStmt(cond, START, true); 

              if (!isPostLoop) { 
                  AddBranch( lastNode, testNode, BuildCFGConfig<Node,Edge>::ALWAYS); 
              }
              else {
                  AddBranch( lastNode, bodyNode, BuildCFGConfig<Node,Edge>::ALWAYS);
              }
              AddBranch( testNode, exitNode, BuildCFGConfig<Node,Edge>::COND_FALSE);
              AddBranch( testNode, bodyNode, BuildCFGConfig<Node,Edge>::COND_TRUE); 
           }
           else {
              AddBranch( lastNode, bodyNode, BuildCFGConfig<Node,Edge>::ALWAYS);
           }
           if (incr != 0) {
              Node* incrNode = MapStmt(incr, START, true);
              SetStmtNode(s, incrNode, START);
              SetStmtNode( body, incrNode, EXIT);
           }
           else if (cond != 0) {
              assert(testNode != 0);
              SetStmtNode(s, testNode, START);
              SetStmtNode(body, testNode, EXIT);
           }
           else {
              SetStmtNode(s, bodyNode, START);
              SetStmtNode(body, bodyNode, EXIT);
           }
           SetCurNode(bodyNode);
        }
        else {
           if (incr != 0) {
              Node* incrNode = GetStmtNode(incr, START);
              assert(incrNode != 0);
              AddBranch(lastNode, incrNode, CFGConfig::ALWAYS);
              SetCurNode(incrNode);
              lastNode = incrNode;
           }
           if (cond != 0) {
              Node* testNode = GetStmtNode(cond, START);
              assert(testNode != 0);  
              AddBranch( lastNode, testNode, CFGConfig::ALWAYS);
           }
           else { 
              Node* bodyNode = GetStmtNode(body, START);
              assert(bodyNode != 0);
              AddBranch( lastNode, bodyNode, CFGConfig::ALWAYS); 
           }  
           SetCurNode(exitNode);
        }
        return  ProcessAstTree::ProcessLoop(fa, s, body, t);
     }
  virtual void ProcessIf( AstInterface &fa, const AstNodePtr& s,
                             const AstNodePtr& cond, const AstNodePtr& trueBody,
                             const AstNodePtr& falseBody,
                               AstInterface::TraversalVisitType t)
   {
     if (debug_cfg()) {
        cerr << "processing if ";
        cerr << ((t == AstInterface::PreVisit)? " previsit " : " postvisit ") << endl;
      }
        Node *exitNode = MapStmt(s, EXIT);


        if (t == AstInterface::PreVisit) {
            Node *lastNode = GetCurNode();
            assert( cond != 0);
            Node *testNode = IsCurNodeEmpty()? AddStmt(lastNode, cond) : MapStmt(cond, START, true);
            AddBranch( lastNode, testNode, CFGConfig::ALWAYS);
            Node *falseNode = IsBodyEmpty(fa,falseBody)? 0 : MapStmt(falseBody, START);
            if (falseNode != 0)  {
                 AddBranch(testNode, falseNode, CFGConfig::COND_FALSE);
                 SetStmtNode( falseBody, exitNode, EXIT);
            }
            Node *trueNode = IsBodyEmpty(fa,trueBody)? 0 :MapStmt(trueBody, START);
            if (trueNode != 0) {
               AddBranch(testNode, trueNode, CFGConfig::COND_TRUE);
               SetStmtNode( trueBody, exitNode, EXIT);
               SetCurNode(trueNode);
            }
        }
        else {
            bool empty1 = IsBodyEmpty(fa, trueBody), empty2 = IsBodyEmpty(fa, falseBody);
            Node *trueNode = empty1? 0 : GetStmtNode(trueBody, EXIT);
            Node *falseNode = empty2? 0 : GetStmtNode(falseBody, EXIT);
            Node *testNode  = GetStmtNode(cond, START);
            assert(testNode != 0);
            if (trueNode != 0)
                  AddBranch( trueNode, exitNode, CFGConfig::ALWAYS);
            else if (empty1) {
               AddBranch(testNode, exitNode, CFGConfig::COND_TRUE);
             }
            if (falseNode != 0) 
                  AddBranch( falseNode, exitNode, CFGConfig::ALWAYS);
            else if (empty2) {
                  AddBranch(testNode, exitNode, CFGConfig::COND_FALSE);
            }
           SetCurNode(exitNode);
        }
        ProcessAstTree::ProcessIf(fa, s, cond, trueBody, falseBody, t);
     }
  virtual Boolean ProcessBasicBlock( AstInterface &fa, const AstNodePtr& s,
                                    AstInterface::TraversalVisitType t)
    {
     if (debug_cfg()) {
        cerr << "processing basic block "; // << AstInterface::AstToString(s); 
        cerr << ((t == AstInterface::PreVisit)? " previsit " : " postvisit ") << endl;
     }

        Node* lastNode = GetCurNode(false);
        if (t == AstInterface::PreVisit) {
            Node *n = GetStmtNode(s,START);
            if ( n != 0 && n != lastNode) {
               SetCurNode(n); 
               Node *n1 = MapStmt(s, EXIT); 
               AstNodePtr lastStmt = fa.GetBasicBlockLastStmt(s);
               if (lastStmt != 0) 
                 SetStmtNode(lastStmt, n1, EXIT);
            }
        } 
        else if (lastNode != 0) {  // use exitMap to remember the last node of this block
           SetStmtNode( s, lastNode, EXIT);  
        }
        else if ( (lastNode = GetStmtNode(s, EXIT)) != 0)
           SetCurNode(lastNode);
        return  ProcessAstTree::ProcessBasicBlock(fa, s, t);
    }

  virtual Boolean ProcessGoto( AstInterface &fa, const AstNodePtr& s, 
                               const AstNodePtr& _dest)
     {
     if (debug_cfg()) {
        cerr << "processing go to  " ; 
     }
        AstNodePtr dest = _dest;
     // DQ (8/4/2005): Trivial change suggested by Qing
     // Node *lastNode = GetCurNode( false );
        Node *lastNode = GetCurNode( true );
        assert (lastNode != 0);
        AddStmt(lastNode, s); 

        if (fa.IsGotoAfter(s)) {
            Node* destNode = MapStmt(dest, EXIT);
            AddBranch( lastNode, destNode, CFGConfig::ALWAYS);
            while (fa.IsBasicBlock(dest)) {
              dest = fa.GetBasicBlockLastStmt(dest);
              SetStmtNode(dest, destNode, EXIT); 
            }
        }
        else { 
            Node* destNode =  MapStmt(dest, START);
            AddBranch( lastNode, destNode, CFGConfig::ALWAYS);
            AstNodePtr prev = fa.GetPrevStmt(dest);
            if (prev != 0)
               SetStmtNode( prev, destNode, EXIT);
        }
        SetCurNode(0);

        return  ProcessAstTree::ProcessGoto(fa, s, dest);
     }

  virtual void ProcessStmt(AstInterface &fa, const AstNodePtr& s)
     {
       if (debug_cfg()) 
          cerr << "processing stmt " << s->class_name() << ": " << fa.AstToString(s) << endl;

         Node *lastNode = GetCurNode();
         Node *n = GetStmtNode(s, START);
         if (n == 0 && !fa.IsLabelStatement(s))
            AddStmt(lastNode, s); 
         else if (n == 0 && IsCurNodeEmpty()) {
            AddStmt(lastNode, s);
            SetStmtNode(s, lastNode, START); 
         }
         else if (n != 0) { 
            if (n == 0) {
               n = MapStmt( s, START, true);
            }
            else
               AddStmt(n, s);
            AddBranch( lastNode, n, CFGConfig::ALWAYS);
            SetCurNode(n);
            SetStmtNode(s, n, START);
         }
        ProcessAstTree::ProcessStmt(fa, s);
     }
};

template <class Node, class Edge>
void ROSE_Analysis::BuildCFG( AstInterface& fa, const AstNodePtr& head, BuildCFGConfig<Node,Edge>& g)
{
  BuildCFGTraverse<Node,Edge> op(g);
  op(fa, head);
}

/* pmp 08JUN05
       in call to op the argument head is undeclared; 
       Uncertain, if this function is ever used, or if head will be known at instantiation time
       --> commented out for now
       
template <class Node, class Edge>
void ROSE_Analysis::BuildCFG( AstInterface& fa, const AstInterface::AstNodeList& stmts, BuildCFGConfig<Node,Edge>& g)
{
  BuildCFGTraverse<Node,Edge> op(g);
  op(fa, head, stmts);
}
*/
