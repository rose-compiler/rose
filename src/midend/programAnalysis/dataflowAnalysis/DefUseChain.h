#ifndef DEF_USE_CHAIN_H
#define DEF_USE_CHAIN_H

#include "ReachingDefinition.h"
#include "StmtInfoCollect.h"
// A node in def-use chain: indicating if a reference in a statement is a definition or use.
class DefUseChainNode : public MultiGraphElem
{
  bool isdef;
  AstNodePtr ref, stmt;
 public:
  DefUseChainNode( MultiGraphCreate* c, const AstNodePtr& _ref, 
                   const AstNodePtr& _stmt, bool def)
    : MultiGraphElem(c), isdef(def), ref(_ref), stmt(_stmt) {}
  bool is_definition() const { return isdef; }
  AstNodePtr get_ref() const { return ref; }
  AstNodePtr get_stmt() const { return stmt; }
  void Dump() const;
  std::string toString() const;
};

  
template <class Node>
class DefUseChain 
  : public VirtualGraphCreateTemplate<Node, MultiGraphElem>
{
 public:
  typedef MultiGraphElem Edge;
  typedef typename VirtualGraphCreateTemplate<Node, Edge>::NodeIterator NodeIterator;
  typedef typename VirtualGraphCreateTemplate<Node, Edge>::EdgeIterator EdgeIterator;
  
  DefUseChain( BaseGraphCreate* c) 
    : VirtualGraphCreateTemplate<Node, Edge>(c) {}

  virtual Node* CreateNode( AstInterface& fa, const AstNodePtr& ref, 
                            const AstNodePtr& stmt, bool def)
    {
      Node* n = new Node(this, ref, stmt, def);
      AddNode(n);

      return n;
    }
  Edge* CreateEdge( Node* n1, Node* n2)
    {
      Edge* e = new Edge(this);
      AddEdge( n1, n2, e);
      return e;
    }
  
  void build( AstInterface& fa,
              ReachingDefinitionAnalysis& r, 
              AliasAnalysisInterface& alias,
              FunctionSideEffectInterface* f = 0);
  //Build ReachingDefinition internally       
  void build(AstInterface& fa, AstNodePtr root,  
             AliasAnalysisInterface* alias = 0, 
             FunctionSideEffectInterface* f = 0);
};

template<class Node>
class UpdateDefUseChainNode {
 public:
  virtual void init(CollectObject<Node*>& newnodes ) = 0;
  virtual bool update_def_node( Node* def, const Node* use, 
                                CollectObject<Node*>& newnodes) = 0;
  virtual bool update_use_node( Node* use, const Node* def,
                                CollectObject<Node*>& newnodes) = 0;
  
  virtual ~UpdateDefUseChainNode() {}
};

class DefaultDUchain : public DefUseChain<DefUseChainNode>
{
 public:
   DefaultDUchain( BaseGraphCreate* c = 0)
    : DefUseChain<DefUseChainNode>(c) {}
};

template<class Node>
void PropagateDefUseChainUpdate( DefUseChain<Node> *graph, 
                                 UpdateDefUseChainNode<Node>& update);










#define TEMPLATE_ONLY
// tps: 16Dec2008: copied the code from DefUseChain.C into here
// this is because the file could not be found when ROSE is installed.
//#include <DefUseChain.C>

#include "DefUseChain.h"
#include "StmtInfoCollect.h"
#include "SinglyLinkedList.h"
#include "CommandOptions.h"
#include "GraphUtils.h"
#include "GraphIO.h"

#include <vector>


bool DebugDefUseChain();

template <class Node>
class BuildDefUseChain 
{
 protected:
  DefUseChain<Node>* graph;
  std::vector<Node*>& defvec;

  const ReachingDefinitionGenerator *g;
  AliasAnalysisInterface& alias;
  AstInterface& fa;

 public:
  void CreateEdges( Node* cur, const AstNodePtr& ref, const ReachingDefinitions& in)
  {
    ReachingDefinitions known = g->get_empty_set();
    ReachingDefinitions unknown = in;
    std::string varname;
    AstNodePtr scope;
    if (fa.IsVarRef(ref, 0, &varname, &scope)) {
      unknown = known = g->get_def_set(varname, scope);
      unknown.complement();
      known &= in;
      unknown &= in;
    }
    for (size_t i = 0; i < defvec.size(); ++i) {
        Node* def = defvec[i];
        assert (def != 0);
        if (known.has_member(i) ||
            (unknown.has_member(i) && alias.may_alias( fa, ref, def->get_ref()) )) {
          if (DebugDefUseChain()) 
             std::cerr << " creating edge from " << def->toString() << std::endl;
          graph->CreateEdge(def, cur);
        }
        else if (DebugDefUseChain())  {
          if (!unknown.has_member(i)) 
             std::cerr << "not in reaching definition: " << def->toString();
          else if (!alias.may_alias( fa, ref, def->get_ref())) 
             std::cerr << "not aliased: " << def->toString(); 
        }
    }
  }
  BuildDefUseChain(  DefUseChain<Node>* _graph, std::vector<Node*>& _defvec,
                     const ReachingDefinitionGenerator *_g,
                     AliasAnalysisInterface& _alias,
                     AstInterface& _fa)
    : graph(_graph), defvec(_defvec), g(_g), alias(_alias), fa(_fa) {}
};

template <class Node>
class ProcessGenInfo 
  : public CollectObject<std::pair<AstNodePtr, AstNodePtr> >,
    public BuildDefUseChain<Node>
{
  ReachingDefinitions& in;
  std::map<AstNodePtr, Node*>& defmap;

  bool operator()( const std::pair<AstNodePtr, AstNodePtr>& mod)
  {
    std::string varname;
    AstNodePtr scope;
    if (DebugDefUseChain()) {
      std::cerr << "processing gen mod info : " << AstToString(mod.first) << " : " << AstToString(mod.second) << std::endl;
      DumpDefSet(BuildDefUseChain<Node>::defvec,in);
    }

    typename std::map<AstNodePtr,Node*>::const_iterator p = defmap.find( mod.first);
    assert( p != defmap.end());
    Node* cur = (*p).second;
    CreateEdges( cur, mod.first, in);
    if (BuildDefUseChain<Node>::fa.IsVarRef(mod.first, 0, &varname, &scope)) {
      BuildDefUseChain<Node>::g->add_def( in, varname, scope, mod);
    }
    else {
      BuildDefUseChain<Node>::g->add_unknown_def( in, mod);
    }
    if (DebugDefUseChain()) {
      std::cerr << "finish processing gen mod info : " << AstToString(mod.first) << " : " << AstToString(mod.second) << std::endl;
      DumpDefSet(BuildDefUseChain<Node>::defvec,in);
    }
    return true;
  }
public:
  ProcessGenInfo( DefUseChain<Node>* _graph, std::vector<Node*>& _defvec,
                     const ReachingDefinitionGenerator *_g,
                     AliasAnalysisInterface& _alias,
                     AstInterface& _fa, std::map<AstNodePtr, Node*>& dm, ReachingDefinitions& _in)
    : BuildDefUseChain<Node>(_graph, _defvec, _g, _alias, _fa), in(_in), defmap(dm) {}
};

template <class Node>
class ProcessKillInfo
  : public CollectObject<std::pair<AstNodePtr, AstNodePtr> >,
    public BuildDefUseChain<Node>
{
  ReachingDefinitions& in;
  std::map<AstNodePtr, Node*>& defmap;

  bool operator()( const std::pair<AstNodePtr, AstNodePtr>& mod)
  {
    std::string varname;
    AstNodePtr scope;
    if (DebugDefUseChain()) {
      std::cerr << "processing kill mod info : " << AstToString(mod.first) << " : " << AstToString(mod.second) << std::endl;
      DumpDefSet(BuildDefUseChain<Node>::defvec,in);
    }
    if (BuildDefUseChain<Node>::fa.IsVarRef(mod.first, 0, &varname, &scope)) {
      ReachingDefinitions kill = BuildDefUseChain<Node>::g->get_def_set(varname, scope);
      kill.complement();
      in &= kill;
    }
    if (DebugDefUseChain()) {
      std::cerr << "finish processing kill mod info : " << AstToString(mod.first) << " : " << AstToString(mod.second) << std::endl;
      DumpDefSet(BuildDefUseChain<Node>::defvec,in);
    }
    return true;
  }
public:
  ProcessKillInfo( DefUseChain<Node>* _graph, std::vector<Node*>& _defvec,
                     const ReachingDefinitionGenerator *_g,
                     AliasAnalysisInterface& _alias,
                     AstInterface& _fa, std::map<AstNodePtr, Node*>& dm, ReachingDefinitions& _in)
    : BuildDefUseChain<Node>(_graph, _defvec, _g, _alias, _fa), in(_in), defmap(dm) {}
};



template <class Node> 
class ProcessUseInfo
  : public CollectObject< std::pair<AstNodePtr, AstNodePtr> >,
    public BuildDefUseChain<Node>
{
  ReachingDefinitions& in;

  bool operator()( const std::pair<AstNodePtr, AstNodePtr>& read)
  {
    if (DebugDefUseChain())  {
       std::cerr << "processind read info : " << AstToString(read.first) << " : " << AstToString(read.second) << std::endl;
         DumpDefSet(BuildDefUseChain<Node>::defvec,in);
    }
    Node* cur = BuildDefUseChain<Node>::graph->CreateNode( BuildDefUseChain<Node>::fa, read.first, read.second, false);
    if (cur == 0) {
       if (DebugDefUseChain()) 
           std::cerr << "do not create node in def-use chain \n";
        return false;
    }
    CreateEdges( cur, read.first, in);
    return true;
  }
public:
  ProcessUseInfo( DefUseChain<Node>* _graph, std::vector<Node*>& _defvec,
                  const ReachingDefinitionGenerator *_g,
                  AliasAnalysisInterface& _alias,
                  AstInterface& _fa, ReachingDefinitions& _in)
    :  BuildDefUseChain<Node>(_graph, _defvec, _g, _alias, _fa), in(_in)  {}
};

template<class Node>
void DumpDefSet( const std::vector<Node*>& defvec, const ReachingDefinitions& in)
{
        for (size_t i = 0; i < defvec.size(); ++i) {
          if (in.has_member(i)) {
             Node* def = defvec[i];
             assert (def != 0);
             std::cerr << def->toString();
           }
        }
}
template <class Node>
void DefUseChain<Node>::
build( AstInterface& fa, AstNodePtr root, AliasAnalysisInterface* alias, 
       FunctionSideEffectInterface *f)
{
   StmtVarAliasCollect defaultAlias;

   ReachingDefinitionAnalysis reachingDef;
   reachingDef(fa, root);
   if (alias == 0) {
      alias = &defaultAlias;
      AstNodePtr defn = fa.GetFunctionDefinition(root); 
      assert(defn != AST_NULL);    
      defaultAlias(fa, defn);
   }
   build(fa, reachingDef,*alias, f);
}

template <class Node>
void DefUseChain<Node>::
build( AstInterface& fa, ReachingDefinitionAnalysis& r, 
       AliasAnalysisInterface& alias, FunctionSideEffectInterface* f)
{
  std::vector <Node*> defvec;
  const ReachingDefinitionGenerator* g = r.get_generator();
  StmtSideEffectCollect collect(f);

  std::map<AstNodePtr, Node*> defmap;
  const ReachingDefinitionBase& base = g->get_base();
  for (ReachingDefinitionBase::iterator p = base.begin(); p != base.end(); ++p) {
    std::pair<AstNodePtr,AstNodePtr> cur = base.get_ref(p);
    if (DebugDefUseChain()) 
       std::cerr << "creating def node : " << AstToString(cur.first) << " : " << AstToString(cur.second) << std::endl;
    Node* n = CreateNode( fa, cur.first, cur.second, true);
    assert(n != 0);
    defvec.push_back(n);
    defmap[cur.first] = n;
  }
  for (ReachingDefinitionAnalysis::NodeIterator p = r.GetNodeIterator(); 
       !p.ReachEnd(); ++p) {
      ReachingDefNode* cur = *p;
      if (DebugDefUseChain())  {
         std::cerr << "processing CFG node : ";
         cur->write(std::cerr);
      }
      ReachingDefinitions in = cur->get_entry_defs();
      if (DebugDefUseChain()) { 
         std::cerr << "Reaching definitions: \n";
         DumpDefSet(defvec,in);
         std::cerr << std::endl;
      }

      ProcessUseInfo<Node> opread( this, defvec, g, alias,fa, in);
      ProcessGenInfo<Node> opgen( this, defvec, g, alias, fa, defmap, in);
      ProcessKillInfo<Node> opkill( this, defvec, g, alias, fa, defmap, in);
      std::list <AstNodePtr>& stmts = cur->GetStmts();
      for (std::list<AstNodePtr>::iterator p = stmts.begin(); p != stmts.end();
           ++p) {
        AstNodePtr cur = *p;
        if (DebugDefUseChain())  
            std::cerr << "processing stmt : " << AstToString(cur) << std::endl;
        collect(fa, cur, &opgen, &opread, &opkill); 
      }
  }

  if (DebugDefUseChain()) {
     std::cerr << "\nfinished building def-use chain:\n";
     std::cerr << GraphToString(*this);
  }
}

template <class Node>
class AppendWorkListWrap : public CollectObject<Node*>
{
  std::set<Node*>& worklist;
 public:
  AppendWorkListWrap( std::set<Node*> &w) : worklist(w) {}
  bool operator() (Node* const& cur)
   {
     if (worklist.find(cur) == worklist.end())  {
        worklist.insert(cur);
         return true;
     }
     return false;
   }
};

template<class Node>
void PropagateDefUseChainUpdate( DefUseChain<Node> *graph, 
                                 UpdateDefUseChainNode<Node>& update)
{
  std::set<Node*> worklist;
  AppendWorkListWrap<Node> append(worklist);
  update.init(append);
  while (worklist.size()) {
    Node* cur = *worklist.begin();
    worklist.erase(worklist.begin());
    if (cur->is_definition()) {
      for (GraphNodeSuccessorIterator<DefUseChain<Node> > usep(graph,cur);
           !usep.ReachEnd(); ++usep) {
        Node* use = *usep;
        if (use->is_definition())
          continue;
        GraphNodePredecessorIterator<DefUseChain<Node> > defp(graph,use);
        Node *tmp = *defp;
        ++defp;
        if (defp.ReachEnd()) {
          assert( tmp == cur);
          update.update_use_node(use, cur, append);
        }
      }
    }
    else {
      GraphNodePredecessorIterator<DefUseChain<Node> > defp(graph,cur);
      if (defp.ReachEnd()) {
        if (DebugDefUseChain())  {
          std::cerr << "Error: use of reference with no definition: ";
          cur->Dump();
          std::cerr << std::endl;
        }
      }
      else {
        // Node* def = *defp;
        if (!defp.ReachEnd()) {
          Node* def = *defp;
          ++defp;
          if (defp.ReachEnd()) {
            if (update.update_def_node(def, cur, append)) 
              append(def);
          }
        }
      }
    }
  }
}



#undef TEMPLATE_ONLY

#endif


