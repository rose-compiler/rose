#ifndef CFGIMPL_H
#define CFGIMPL_H

#include <IDGraphCreate.h>
#include <CFG.h>
#include <list>

template <class Node, class Edge>
class CFGImplTemplate 
  : public IDGraphCreateTemplate<Node, Edge>, public BuildCFGConfig<Node,Edge>
{
 protected:
  typedef typename BuildCFGConfig<Node,Edge>::EdgeType EdgeType;
  virtual Node* CreateNode()
        { 
           Node* n = new Node(this);
           CreateBaseNode(n);
           return n;
        }
  virtual Edge* CreateEdge( Node *n1, Node *n2, EdgeType condval) 
        {
           Edge* e = new Edge(condval, this);
           CreateBaseEdge( n1, n2, e);
           return e;
        } 
  virtual void AddNodeStmt(Node* n, const AstNodePtr& s) 
        { n->AddNodeStmt(s); }
 public:
  typedef typename IDGraphCreateTemplate<Node, Edge>::NodeIterator NodeIterator;
  typedef typename IDGraphCreateTemplate<Node, Edge>::EdgeIterator EdgeIterator;

  CFGImplTemplate(  BaseGraphCreate *_impl = 0) : IDGraphCreateTemplate<Node,Edge>(_impl) {}

  NodeIterator GetPredecessors( Node* n)
       { return GraphGetNodePredecessors<CFGImplTemplate<Node,Edge> >()(this, n); }
  NodeIterator GetSuccessors( Node *n)
       { return GraphGetNodeSuccessors<CFGImplTemplate<Node,Edge> >()(this, n); }
};

class CFGNodeImpl : public GraphNode
{
 public:
  CFGNodeImpl(GraphCreate *c) : GraphNode(c) {}
  std::list<AstNodePtr>& GetStmts() { return stmtList; } 
  
  void AddNodeStmt( const AstNodePtr& s) { stmtList.push_back(s); }

  virtual std::string ToString() const
    {
        std::string r;
         for (std::list<AstNodePtr>::const_iterator p = stmtList.begin(); p != stmtList.end(); ++p)
            r = r + AstInterface::AstToString(*p);
        return r;
    }

  virtual void write(std::ostream& out) const
    {
      std::cerr << "Node : " << this << "\n"; 
      for (std::list<AstNodePtr>::const_iterator p = stmtList.begin(); 
           p != stmtList.end(); ++p)
            AstInterface::write(*p, std::cerr);
    }
  virtual void Dump() const
    {
      write(std::cerr);
    }
 private:
  std::list<AstNodePtr> stmtList;
};

class CFGEdgeImpl : public GraphEdge
{
 public:
  typedef CFGConfig::EdgeType EdgeType;
	// HK
  //CFGEdgeImpl( EdgeType val, GraphCreate *c) : t(val), GraphEdge(c) {}
  CFGEdgeImpl( EdgeType val, GraphCreate *c) : GraphEdge(c), t(val) {}
  EdgeType GetEdgeType() const { return t; }
  virtual std::string ToString() const
    {
       switch (t) {
       case CFGConfig::COND_TRUE: return "true"; 
       case CFGConfig::COND_FALSE: return "false"; 
       case CFGConfig::ALWAYS: return "always";
       default:
           assert(false);
       }
    }
  virtual void write(std::ostream& out) const
   { out << ToString() << std::endl; }
  virtual void Dump() const
   { write(std::cerr); }

 private:
  EdgeType t;
};

class DefaultCFGImpl : public CFGImplTemplate<CFGNodeImpl, CFGEdgeImpl> {};

#endif
