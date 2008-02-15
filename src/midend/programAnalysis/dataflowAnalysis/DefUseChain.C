
#include <DefUseChain.h>
#include <StmtInfoCollect.h>
#include <SinglyLinkedList.h>
#include <CommandOptions.h>
#include <GraphUtils.h>
#include <GraphIO.h>

#include <vector>

#ifdef TEMPLATE_ONLY

bool DebugDefUseChain();

template <class Node>
class BuildDefUseChain 
{
 protected:
  DefUseChain<Node>* graph;
  STD vector<Node*>& defvec;

  const ReachingDefinitionGenerator *g;
  AliasAnalysisInterface& alias;
  AstInterface& fa;

 public:
  void CreateEdges( Node* cur, const AstNodePtr& ref, const ReachingDefinitions& in)
  {
    ReachingDefinitions known = g->get_empty_set();
    ReachingDefinitions unknown = in;
    STD string varname;
    AstNodePtr scope;
    if (fa.IsVarRef(ref, 0, &varname, &scope)) {
      unknown = known = g->get_def_set(varname, scope);
      unknown.complement();
      known &= in;
      unknown &= in;
    }
    for (int i = 0; i < defvec.size(); ++i) {
        Node* def = defvec[i];
        assert (def != 0);
        if (known.has_member(i) ||
            (unknown.has_member(i) && alias.may_alias( fa, ref, def->get_ref()) )) {
          if (DebugDefUseChain()) 
             STD cerr << " creating edge from " << def->toString() << STD endl;
          graph->CreateEdge(def, cur);
        }
        else if (DebugDefUseChain())  {
          if (!unknown.has_member(i)) 
             STD cerr << "not in reaching definition: " << def->toString();
          else if (!alias.may_alias( fa, ref, def->get_ref())) 
             STD cerr << "not aliased: " << def->toString(); 
        }
    }
  }
  BuildDefUseChain(  DefUseChain<Node>* _graph, STD vector<Node*>& _defvec,
                     const ReachingDefinitionGenerator *_g,
                     AliasAnalysisInterface& _alias,
                     AstInterface& _fa)
    : graph(_graph), defvec(_defvec), g(_g), alias(_alias), fa(_fa) {}
};

template <class Node>
class ProcessGenInfo 
  : public CollectObject<STD pair<AstNodePtr, AstNodePtr> >,
    public BuildDefUseChain<Node>
{
  ReachingDefinitions& in;
  STD map<AstNodePtr, Node*>& defmap;

  bool operator()( const STD pair<AstNodePtr, AstNodePtr>& mod)
  {
    STD string varname;
    AstNodePtr scope;
    if (DebugDefUseChain()) {
      STD cerr << "processing gen mod info : " << AstToString(mod.first) << " : " << AstToString(mod.second) << STD endl;
      DumpDefSet(BuildDefUseChain<Node>::defvec,in);
    }

    typename STD map<AstNodePtr,Node*>::const_iterator p = defmap.find( mod.first);
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
      STD cerr << "finish processing gen mod info : " << AstToString(mod.first) << " : " << AstToString(mod.second) << STD endl;
      DumpDefSet(BuildDefUseChain<Node>::defvec,in);
    }
    return true;
  }
public:
  ProcessGenInfo( DefUseChain<Node>* _graph, STD vector<Node*>& _defvec,
                     const ReachingDefinitionGenerator *_g,
                     AliasAnalysisInterface& _alias,
                     AstInterface& _fa, STD map<AstNodePtr, Node*>& dm, ReachingDefinitions& _in)
    : BuildDefUseChain<Node>(_graph, _defvec, _g, _alias, _fa), in(_in), defmap(dm) {}
};

template <class Node>
class ProcessKillInfo
  : public CollectObject<STD pair<AstNodePtr, AstNodePtr> >,
    public BuildDefUseChain<Node>
{
  ReachingDefinitions& in;
  STD map<AstNodePtr, Node*>& defmap;

  bool operator()( const STD pair<AstNodePtr, AstNodePtr>& mod)
  {
    STD string varname;
    AstNodePtr scope;
    if (DebugDefUseChain()) {
      STD cerr << "processing kill mod info : " << AstToString(mod.first) << " : " << AstToString(mod.second) << STD endl;
      DumpDefSet(BuildDefUseChain<Node>::defvec,in);
    }
    if (BuildDefUseChain<Node>::fa.IsVarRef(mod.first, 0, &varname, &scope)) {
      ReachingDefinitions kill = BuildDefUseChain<Node>::g->get_def_set(varname, scope);
      kill.complement();
      in &= kill;
    }
    if (DebugDefUseChain()) {
      STD cerr << "finish processing kill mod info : " << AstToString(mod.first) << " : " << AstToString(mod.second) << STD endl;
      DumpDefSet(BuildDefUseChain<Node>::defvec,in);
    }
    return true;
  }
public:
  ProcessKillInfo( DefUseChain<Node>* _graph, STD vector<Node*>& _defvec,
                     const ReachingDefinitionGenerator *_g,
                     AliasAnalysisInterface& _alias,
                     AstInterface& _fa, STD map<AstNodePtr, Node*>& dm, ReachingDefinitions& _in)
    : BuildDefUseChain<Node>(_graph, _defvec, _g, _alias, _fa), in(_in), defmap(dm) {}
};



template <class Node> 
class ProcessUseInfo
  : public CollectObject< STD pair<AstNodePtr, AstNodePtr> >,
    public BuildDefUseChain<Node>
{
  ReachingDefinitions& in;

  bool operator()( const STD pair<AstNodePtr, AstNodePtr>& read)
  {
    if (DebugDefUseChain())  {
       STD cerr << "processind read info : " << AstToString(read.first) << " : " << AstToString(read.second) << STD endl;
         DumpDefSet(BuildDefUseChain<Node>::defvec,in);
    }
    Node* cur = BuildDefUseChain<Node>::graph->CreateNode( BuildDefUseChain<Node>::fa, read.first, read.second, false);
    if (cur == 0) {
       if (DebugDefUseChain()) 
           STD cerr << "do not create node in def-use chain \n";
        return false;
    }
    CreateEdges( cur, read.first, in);
    return true;
  }
public:
  ProcessUseInfo( DefUseChain<Node>* _graph, STD vector<Node*>& _defvec,
		     const ReachingDefinitionGenerator *_g,
		     AliasAnalysisInterface& _alias,
		     AstInterface& _fa, ReachingDefinitions& _in)
    :  BuildDefUseChain<Node>(_graph, _defvec, _g, _alias, _fa), in(_in)  {}
};

template<class Node>
void DumpDefSet( const STD vector<Node*>& defvec, const ReachingDefinitions& in)
{
        for (int i = 0; i < defvec.size(); ++i) {
          if (in.has_member(i)) {
             Node* def = defvec[i];
             assert (def != 0);
             STD cerr << def->toString();
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
  STD vector <Node*> defvec;
  const ReachingDefinitionGenerator* g = r.get_generator();
  StmtSideEffectCollect collect(f);

  STD map<AstNodePtr, Node*> defmap;
  const ReachingDefinitionBase& base = g->get_base();
  for (ReachingDefinitionBase::iterator p = base.begin(); p != base.end(); ++p) {
    STD pair<AstNodePtr,AstNodePtr> cur = base.get_ref(p);
    if (DebugDefUseChain()) 
       STD cerr << "creating def node : " << AstToString(cur.first) << " : " << AstToString(cur.second) << STD endl;
    Node* n = CreateNode( fa, cur.first, cur.second, true);
    assert(n != 0);
    defvec.push_back(n);
    defmap[cur.first] = n;
  }
  for (ReachingDefinitionAnalysis::NodeIterator p = r.GetNodeIterator(); 
       !p.ReachEnd(); ++p) {
      ReachingDefNode* cur = *p;
      if (DebugDefUseChain())  {
         STD cerr << "processing CFG node : ";
         cur->write(STD cerr);
      }
      ReachingDefinitions in = cur->get_entry_defs();
      if (DebugDefUseChain()) { 
         STD cerr << "Reaching definitions: \n";
         DumpDefSet(defvec,in);
         STD cerr << STD endl;
      }

      ProcessUseInfo<Node> opread( this, defvec, g, alias,fa, in);
      ProcessGenInfo<Node> opgen( this, defvec, g, alias, fa, defmap, in);
      ProcessKillInfo<Node> opkill( this, defvec, g, alias, fa, defmap, in);
      STD list <AstNodePtr>& stmts = cur->GetStmts();
      for (STD list<AstNodePtr>::iterator p = stmts.begin(); p != stmts.end();
	   ++p) {
	AstNodePtr cur = *p;
        if (DebugDefUseChain())  
            STD cerr << "processing stmt : " << AstToString(cur) << STD endl;
	collect(fa, cur, &opgen, &opread, &opkill); 
      }
  }

  if (DebugDefUseChain()) {
     STD cerr << "\nfinished building def-use chain:\n";
     STD cerr << GraphToString(*this);
  }
}

template <class Node>
class AppendWorkListWrap : public CollectObject<Node*>
{
  STD set<Node*>& worklist;
 public:
  AppendWorkListWrap( STD set<Node*> &w) : worklist(w) {}
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
  STD set<Node*> worklist;
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
             STD cerr << "Error: use of reference with no definition: ";
             cur->Dump();
             STD cerr << STD endl;
          }
      }
      else {
        Node* def = *defp;
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

#else

void DefUseChainNode::Dump() const
{
  if (isdef) 
    STD cerr << "definition: ";
  else
    STD cerr << "use: ";
  STD cerr << AstToString(ref) << " : " << AstToString(stmt) << STD endl;
}

STD string DefUseChainNode::toString() const
{
  STD string result;
  if (isdef)
     result = "definition:";
  else
    result = "use:";
  result = result + AstToString(ref) + ":" + AstToString(stmt) + " ";
  return result;
}


bool DebugDefUseChain()
{
  static int r =  0;
  if (r == 0) {
     if (CmdOptions::GetInstance()->HasOption("-debugdefusechain"))
         r = 1;
     else
         r = -1;
   }
  return r == 1;
}

#endif
