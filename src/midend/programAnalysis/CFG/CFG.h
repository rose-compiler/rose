#ifndef CFG_ROSE_H
#define CFG_ROSE_H

#include <AstInterface.h>
#include <PtrMap.h>
#include <ProcessAstTree.h>
#include <CommandOptions.h>

// A path in the class hierarchy: 
// CFGConfig->BuildCFGConfig->CFGImplTemplate->DataFlowAnalysis->ReachingDefinitionAnalysis
class CFGConfig {
 public:
  // Edge types for control flow graphs: executed when condition is true, false, or always executed
  typedef enum {COND_TRUE, COND_FALSE, ALWAYS} EdgeType;
  static std::string EdgeType2String( EdgeType e);
};

//Abstract interface of building a CFG using Node type, such as data flow info. node ReachingDefNode
template <class Node>
class BuildCFGConfig  : public CFGConfig
{
 public:
  virtual Node* CreateNode() = 0;
  virtual void CreateEdge( Node *n1, Node *n2, EdgeType condval) = 0;
  //Associating a CFG node to its corresponding AST node
  virtual void AddNodeStmt(Node* n, const AstNodePtr& s) = 0;
  virtual ~BuildCFGConfig() {}
};

namespace ROSE_Analysis {
  template <class Node>
  void BuildCFG ( AstInterface& fa, const AstNodePtr& head, BuildCFGConfig<Node>& g);

  template <class Node>
  void BuildCFG ( AstInterface& fa, const AstInterface::AstNodeList& head, BuildCFGConfig<Node>& g);
};


namespace OpenAnalysis {
class CFG;
class ROSE_CFG_Wrap {
  CFG *oaCfg; 
 public:
  ROSE_CFG_Wrap( const AstNodePtr& head );
  ~ROSE_CFG_Wrap();

  CFG& get_OA_CFG() const { return *oaCfg; }
};

template <class Node>
void OA2ROSE_CFG_Translate ( ROSE_CFG_Wrap& wrap, BuildCFGConfig<Node>& ng);

template <class Node>
void BuildCFG ( AstInterface& fa, const AstNodePtr& head, BuildCFGConfig<Node>& g);
}; // end namespace OpenAnalysis

bool debug_cfg();

inline std::string CFGConfig::EdgeType2String( EdgeType e)
{
  switch (e) {
  case COND_TRUE: return "true";
  case COND_FALSE: return "false";
  case ALWAYS: return "always";
  default:
          { assert(false); /* Avoid MSVC warning */ return "error"; }
  }
}

template <class Node>
class BuildCFGTraverse : public ProcessAstTree
{
 public:
  BuildCFGTraverse( BuildCFGConfig<Node>& g) : graph(g), node(0), stmtnum(0) {}
  void operator()(  AstInterface& fa, const AstNodePtr& head )
     { 
       ProcessAstTree::operator()(fa, head);
     }
  void operator() ( AstInterface& fa, const AstInterface::AstNodeList& stmts)
     {
       for (AstInterface::AstNodeList::const_iterator p = stmts.begin();
            p != stmts.end(); ++p) {
          operator()(fa, *p);
       }
     }


 private:
  BuildCFGConfig<Node>& graph;
  Node* node; 
  unsigned stmtnum;


  // map AstNode pointers to graph node pointers: 
  //   each compound statement (eg, loop, if, basic-block) maps to the last graph node containing its statements  
  // startMap  and exitMap map stmt to the start graph node and to the exit (next)graph node respectively
  PtrMapWrap<void, Node> startMap, exitMap;

  typedef enum {START, EXIT} MapType;

  bool IsBodyEmpty( AstInterface&fa, const AstNodePtr& s)
  {
    if (s == AST_NULL)
        return true;
    AstNodePtr first;
    if (fa.IsBlock(s) && (first = fa.GetBlockFirstStmt(s)) == AST_NULL) {
      if (debug_cfg())
         std::cerr << "block " << AstToString(s) << " is empty " << std::endl;
      return true;
    }
    else {
      if (debug_cfg()) {
         std::cerr << "block " << AstToString(s) << " is not empty " ;
         if (first != AST_NULL) 
            std::cerr << "first statement: " << AstToString(first) << std::endl;
         else
            std::cerr << std::endl;
      }
    }
    return false; 
  }

  Node* GetStmtNode( const AstNodePtr& s, MapType t ) 
   {
      switch (t) {
      case EXIT:
       return exitMap.Map(s.get_ptr());
      case START:
        return startMap.Map(s.get_ptr());
      } 
      assert(false);
      return 0;
   }
  void SetStmtNode( const AstNodePtr& s, Node* n, MapType t)
   {
      if (debug_cfg()) {
         std::cerr << "mapping stmt " << ((t == START)? "START" : "EXIT");
         std::cerr << AstToString(s) << " to " << n << std::endl;
      }
      switch (t) {
      case EXIT:
         exitMap.InsertMapping(s.get_ptr(), n); break;
         break;
      case START:
         startMap.InsertMapping(s.get_ptr(), n);
         break;
      default:
         assert(false);
      };
   }
  // map s to a CFG node: create a new one if not already mapped 
  Node* MapStmt( AstInterface& fa, const AstNodePtr& s, MapType t, bool add = false) 
   {
     assert(s != AST_NULL);
     Node *n = GetStmtNode(s,t);
     if (n != 0) 
         return n;
         
      n = graph.CreateNode();
      if (debug_cfg()) {
         std::cerr << " create node : " << n << std::endl; 
      }
      if (add) {
          assert ( t==START);
           AddStmt(fa, n, s);
          SetStmtNode( s, n, START);
       }
       else
         SetStmtNode(s,n,t);
     return n;
   }
  Node* AddStmt( AstInterface& fa, Node* n, const AstNodePtr& s) // add s to the node n
    { 
       assert(s != AST_NULL && n != 0);
       if (fa.IsBlock(s)) {
          AstInterface::AstNodeList l = fa.GetBlockStmtList(s);
          for (AstInterface::AstNodeList::const_iterator p = l.begin();
               p != l.end(); ++p) {
              graph.AddNodeStmt(n,*p);
          }
       }
       else
          graph.AddNodeStmt(n, s); 
       if (debug_cfg()) {
          std::cerr << " add stmt " << AstToString(s);
          std::cerr << " to node " << n << std::endl;
       }
       if (node == n)
          ++stmtnum;
       SetStmtNode(s,n,START);
       return n;
    }
  void AddBranch( Node *cur, Node* dest, CFGConfig::EdgeType val)
    { 
       assert(cur != 0 && dest != 0);
       if (cur == dest && val == CFGConfig::ALWAYS)
          return;
       graph.CreateEdge(cur, dest, val);
       if (debug_cfg()) 
             std::cerr << "add " << CFGConfig::EdgeType2String(val) << " edge from " << cur << " to " << dest << std::endl;
    }


  bool IsCurNodeEmpty() const { return !stmtnum; }
  Node* GetCurNode( bool createnew = true)  // return the current CFGConfig::node 
   {
      if (node == 0 && createnew) {
        node = graph.CreateNode();
        if (debug_cfg()) 
           std::cerr << "create node in GetCurNode " << node << std::endl;
        stmtnum = 0;
      }
      if (debug_cfg()) 
        std::cerr << "current node : " << node << std::endl;
      return node;
   }
  void SetCurNode( Node *n)
   {
     if (debug_cfg()) {
        if (n == 0)
           std::cerr << "resetting current node to 0 \n"; 
     }
     node = n;
     stmtnum = 0;
   }

  virtual bool ProcessDecls(AstInterface &fa, const AstNodePtr& s)
                                { return ProcessStmt(fa, s); }

  virtual bool ProcessFunctionDefinition( AstInterface &fa, const AstNodePtr& s,
                                           const AstNodePtr& body,
                                     AstInterface::TraversalVisitType t)
   {
     if (debug_cfg()) {
        std::cerr << "processing function def " ; 
        std::cerr << ((t == AstInterface::PreVisit)? " previsit " : " postvisit ") << std::endl;
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

  virtual bool ProcessLoop(AstInterface &fa, const AstNodePtr& s, const AstNodePtr& body,
                               AstInterface::TraversalVisitType t)
     {
        Node *lastNode = GetCurNode();
        SetStmtNode(s, lastNode, START);
        Node *exitNode = MapStmt(fa, s, EXIT);

     if (debug_cfg()) {
        std::cerr << "processing loop ";
        std::cerr << ((t == AstInterface::PreVisit)? " previsit " : " postvisit ") << std::endl;
     }
       AstNodePtr init, cond, incr;
       if (!fa.IsLoop( s, &init, &cond,&incr))
            assert(false);

        if (t == AstInterface::PreVisit) {
           if (init != AST_NULL) 
              AddStmt(fa, lastNode, init);

           bool isPostLoop = fa.IsPostTestLoop(s);
           Node* bodyNode = (isPostLoop && IsCurNodeEmpty())? SetStmtNode(body, lastNode, START), lastNode 
                                                        : MapStmt(fa, body, START);
           Node* testNode = 0;
           if (cond != AST_NULL) {
              testNode = (!isPostLoop && IsCurNodeEmpty())? AddStmt(fa, lastNode, cond), lastNode : MapStmt(fa, cond, START, true); 

              if (!isPostLoop) { 
                  AddBranch( lastNode, testNode, BuildCFGConfig<Node>::ALWAYS); 
              }
              else {
                  AddBranch( lastNode, bodyNode, BuildCFGConfig<Node>::ALWAYS);
              }
              AddBranch( testNode, exitNode, BuildCFGConfig<Node>::COND_FALSE);
              AddBranch( testNode, bodyNode, BuildCFGConfig<Node>::COND_TRUE); 
           }
           else {
              AddBranch( lastNode, bodyNode, BuildCFGConfig<Node>::ALWAYS);
           }
           if (incr != AST_NULL) {
              Node* incrNode = MapStmt(fa, incr, START, true);
              SetStmtNode(s, incrNode, START);
              SetStmtNode( body, incrNode, EXIT);
           }
           else if (cond != AST_NULL) {
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
           if (incr != AST_NULL) {
              Node* incrNode = GetStmtNode(incr, START);
              assert(incrNode != 0);
              AddBranch(lastNode, incrNode, CFGConfig::ALWAYS);
              SetCurNode(incrNode);
              lastNode = incrNode;
           }
           if (cond != AST_NULL) {
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
  virtual bool ProcessIf( AstInterface &fa, const AstNodePtr& s,
                             const AstNodePtr& cond, const AstNodePtr& trueBody,
                             const AstNodePtr& falseBody,
                               AstInterface::TraversalVisitType t)
   {
     if (debug_cfg()) {
        std::cerr << "processing if ";
        std::cerr << ((t == AstInterface::PreVisit)? " previsit " : " postvisit ") << std::endl;
      }
        Node *exitNode = MapStmt(fa, s, EXIT);


        if (t == AstInterface::PreVisit) {
            Node *lastNode = GetCurNode();
            assert( cond != AST_NULL);
            //Process test condition expression
            Node *testNode = IsCurNodeEmpty()? AddStmt(fa, lastNode, cond) : MapStmt(fa, cond, START, true);
            AddBranch( lastNode, testNode, CFGConfig::ALWAYS);
            // Process the false body first
            Node *falseNode = IsBodyEmpty(fa,falseBody)? 0 : MapStmt(fa, falseBody, START);
            if (falseNode != 0)  {
                 AddBranch(testNode, falseNode, CFGConfig::COND_FALSE);
                 SetStmtNode( falseBody, exitNode, EXIT);
            }
            // Process the true body later since the preorder traversal will
            // use it's CFG node as the current CFG node
            Node *trueNode = IsBodyEmpty(fa,trueBody)? 0 :MapStmt(fa, trueBody, START);
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
        return  ProcessAstTree::ProcessIf(fa, s, cond, trueBody, falseBody, t);
     }
  virtual bool ProcessBlock( AstInterface &fa, const AstNodePtr& s,
                                    AstInterface::TraversalVisitType t)
    {
     if (debug_cfg()) {
        std::cerr << "processing basic block " ; 
        std::cerr << ((t == AstInterface::PreVisit)? " previsit " : " postvisit ") << std::endl;
     }

        Node* lastNode = GetCurNode(false);
        if (t == AstInterface::PreVisit) {
            Node *n = GetStmtNode(s,START);
            if ( n != 0 && n != lastNode) 
               SetCurNode(n); 
            Node *n1 = GetStmtNode(s,EXIT); //MapStmt(fa, s, EXIT); 
            if (n1 != 0) {
               AstNodePtr lastStmt = fa.GetBlockLastStmt(s);
               if (lastStmt != AST_NULL)  
                  SetStmtNode(lastStmt, n1, EXIT);
            }
        } 
        else if (lastNode != 0) {  // use exitMap to remember the last node of this block
           SetStmtNode( s, lastNode, EXIT);  
        }
        else if ( (lastNode = GetStmtNode(s, EXIT)) != 0)
           SetCurNode(lastNode);
        return  ProcessAstTree::ProcessBlock(fa, s, t);
    }

  virtual bool ProcessGoto( AstInterface &fa, const AstNodePtr& s, 
                               const AstNodePtr& _dest)
     {
     if (debug_cfg()) {
        std::cerr << "processing go to  " ; 
     }
        AstNodePtr dest = _dest;
        Node *lastNode = GetCurNode( false );
     // Liao, 11/18/2008. bug fix
     // Both true and false body of if-stmt may contain a naked (without a Basic block) goto(return etc.)
     // the first ProcessGoto() of the true body will reset the current node to 0.
     // So we have to restore it to the right one when the second ProcessGoto() is processing the false body
     // for a naked false body: 
     // current CFG node ==false body's START CFG node== return's START CFG node)
     if (lastNode==0)
      {
        lastNode = GetStmtNode(s,START);
         if (debug_cfg())
          std::cerr << "processing go to: GetCurNode() return 0, restoring it to:"<<lastNode <<std::endl; ;
      }
        assert (lastNode != 0);
        AddStmt(fa, lastNode, s); 
         // If it is a goto jumping to the position after the destination
         // return; break;
        if (fa.IsGotoAfter(s)) {
            Node* destNode = MapStmt(fa, dest, EXIT);
            AddBranch( lastNode, destNode, CFGConfig::ALWAYS);
            while (fa.IsBlock(dest)) {
              dest = fa.GetBlockLastStmt(dest);
              SetStmtNode(dest, destNode, EXIT); 
            }
        }
        else { // IsGotoBefore(s): goto, continue
            Node* destNode =  MapStmt(fa, dest, START);
            AddBranch( lastNode, destNode, CFGConfig::ALWAYS);
            AstNodePtr prev = fa.GetPrevStmt(dest);
            if (prev != AST_NULL)
               SetStmtNode( prev, destNode, EXIT);
        }
        SetCurNode(0);

        return  ProcessAstTree::ProcessGoto(fa, s, dest);
     }

  virtual bool ProcessStmt(AstInterface &fa, const AstNodePtr& s)
     {
       if (debug_cfg()) 
          std::cerr << "processing stmt " << AstToString(s) << std::endl;

         Node *lastNode = GetCurNode();
         Node *n = GetStmtNode(s, START);
         if (n == 0 && !fa.IsLabelStatement(s))
            AddStmt(fa, lastNode, s); 
         else if (n == 0 && IsCurNodeEmpty()) {
            AddStmt(fa, lastNode, s);
            SetStmtNode(s, lastNode, START); 
         }
         else if (n != 0) { 
            if (n == 0) {
               n = MapStmt(fa,  s, START, true);
            }
            else
               AddStmt(fa, n, s);
            AddBranch( lastNode, n, CFGConfig::ALWAYS);
            SetCurNode(n);
            SetStmtNode(s, n, START);
         }
        return  ProcessAstTree::ProcessStmt(fa, s);
     }
}; //end class BuildCFGTraverse..

template <class Node>
void ROSE_Analysis::BuildCFG( AstInterface& fa, const AstNodePtr& head, BuildCFGConfig<Node>& g)
{
  BuildCFGTraverse<Node> op(g);
  op(fa, head);
}

template <class Node>
void ROSE_Analysis::BuildCFG( AstInterface& fa, const AstInterface::AstNodeList& stmts, BuildCFGConfig<Node>& g)
{
  BuildCFGTraverse<Node> op(g);
  for (AstInterface::AstNodeList::const_iterator p = stmts.begin();
       p != stmts.end(); ++p) {
     op(fa, *p);
  }
}

#endif
