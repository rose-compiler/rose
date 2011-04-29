#ifndef CFGIMPL_H
#define CFGIMPL_H

#include <VirtualGraphCreate.h>
#include <CFG.h>
#include <sstream>
#include <list>

// A CFG template for arbitrary node and edge types
template <class Node, class Edge>
class CFGImplTemplate 
  : public VirtualGraphCreateTemplate<Node, Edge>, public BuildCFGConfig<Node>
{
 protected:
  typedef typename BuildCFGConfig<Node>::EdgeType EdgeType;
  virtual Node* CreateNode()
        { 
           Node* n = new Node(this);
           AddNode(n);
           return n;
        }
  virtual void CreateEdge( Node *n1, Node *n2, EdgeType condval) 
        {
           Edge* e = new Edge(condval, this);
           AddEdge( n1, n2, e);
        } 
  virtual void AddNodeStmt(Node* n, const AstNodePtr& s) 
        { n->AddNodeStmt(s); }
 public:
  typedef typename VirtualGraphCreateTemplate<Node, Edge>::NodeIterator NodeIterator;
  typedef typename VirtualGraphCreateTemplate<Node, Edge>::EdgeIterator EdgeIterator;

  CFGImplTemplate(  BaseGraphCreate *_impl = 0) : VirtualGraphCreateTemplate<Node,Edge>(_impl) {}

  NodeIterator GetPredecessors( Node* n)
     { return new GraphNodePredecessorIterator<CFGImplTemplate<Node,Edge> >
                   (this, n); }
  NodeIterator GetSuccessors( Node *n)
       { return new GraphNodeSuccessorIterator<CFGImplTemplate<Node,Edge> >
                   (this, n); }
};
// A CFG node may representing several actual source statements
class CFGNodeImpl : public MultiGraphElem
{
 public:
  CFGNodeImpl(MultiGraphCreate *c) : MultiGraphElem(c) {}
  std::list<AstNodePtr>& GetStmts() { return stmtList; } 
  
  void AddNodeStmt( const AstNodePtr& s) { stmtList.push_back(s); }

  virtual std:: string toString() const
    {
        std::stringstream r;
        std:: list<AstNodePtr>::const_iterator p = stmtList.begin(); 
        if (p != stmtList.end()) {
           r <<  AstToString(*p);
           for ( ++p; p != stmtList.end(); ++p) 
               r << "\n" <<  AstToString(*p);
        }
        else
            r << "EMPTY\n";
        return r.str();
    }

  virtual void write(std:: ostream& out) const
    {
      std:: cerr << "Node : " << this << "\n"; 
      for (std:: list<AstNodePtr>::const_iterator p = stmtList.begin(); 
           p != stmtList.end(); ++p)
            std:: cerr << AstToString(*p);
    }
  virtual void Dump() const
    {
      write(std:: cerr);
    }
 private:
  std:: list<AstNodePtr> stmtList;
};
// A CFG edge stores edge type information: exectuted on true condition, false condition, or always executed
class CFGEdgeImpl : public MultiGraphElem
{
 public:
  typedef CFGConfig::EdgeType EdgeType;
  CFGEdgeImpl( EdgeType val, MultiGraphCreate *c) : MultiGraphElem(c), t(val) {}
  EdgeType GetEdgeType() const { return t; }
  virtual std:: string toString() const
    {
       switch (t) {
       case CFGConfig::COND_TRUE: return "true"; 
       case CFGConfig::COND_FALSE: return "false"; 
       case CFGConfig::ALWAYS: return "always";
       default:
                   { assert(false); /* Avoid MSVC warning */ return "error"; }
       }
    }
  virtual void write(std:: ostream& out) const
   { out << toString() << std:: endl; }
  virtual void Dump() const
   { write(std:: cerr); }

 private:
  EdgeType t;
};

class DefaultCFGImpl : public CFGImplTemplate<CFGNodeImpl, CFGEdgeImpl> {};

#endif
