
#include "DefUseChain.h"
#include "StmtInfoCollect.h"
#include "SinglyLinkedList.h"
#include "CommandOptions.h"
#include "GraphUtils.h"
#include "GraphIO.h"
#include <ROSE_ASSERT.h>

#include <vector>

#ifdef TEMPLATE_ONLY

bool DebugDefUseChain();

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
  StmtSideEffectCollect<AstNodePtr> collect(fa, f);

  std::map<AstNodePtr, Node*> defmap;
  const ReachingDefinitionBase& base = g->get_base();
  for (ReachingDefinitionBase::iterator p = base.begin(); p != base.end(); ++p) {
    std::pair<AstNodePtr,AstNodePtr> cur = base.get_ref(p);
    if (DebugDefUseChain())
       std::cerr << "creating def node : " << AstInterface::AstToString(cur.first) << " : " << AstInterface::AstToString(cur.second) << std::endl;
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

      auto CreateEdges = [&fa,&g,&alias,this,&in,&defvec] 
              (Node* cur, const AstNodePtr& ref) {
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
               this->CreateEdge(def, cur);
             }
             else if (DebugDefUseChain())  {
               if (!unknown.has_member(i))
                  std::cerr << "not in reaching definition: " << def->toString();
               else if (!alias.may_alias( fa, ref, def->get_ref()))
                  std::cerr << "not aliased: " << def->toString();
             }
         }
      };
      // Collecting read set.
      std::function<bool(AstNodePtr,AstNodePtr)>  opread = 
              [&defvec, this, &fa, &in, &CreateEdges]
            ( AstNodePtr read_first, AstNodePtr read_second) {
        if (DebugDefUseChain())  {
           std::cerr << "processind read info : " << AstInterface::AstToString(read_first) << " : " << AstInterface::AstToString(read_second) << std::endl;
            DumpDefSet(defvec,in);
        }
        Node* cur = this->CreateNode( fa, read_first, read_second, false);
        if (cur == 0) {
           if (DebugDefUseChain())
               std::cerr << "do not create node in def-use chain \n";
            return false;
        }
        CreateEdges( cur, read_first);
        return true;
      };
      // Collecting mod set.
      std::function<bool(AstNodePtr,AstNodePtr)> opgen = 
              [&defvec, &g, &fa, &defmap, &in, &CreateEdges]
        (AstNodePtr mod_first, AstNodePtr mod_second) {
          std::string varname;
          AstNodePtr scope;
          if (DebugDefUseChain()) {
            std::cerr << "processing gen mod info : " << AstInterface::AstToString(mod_first) << " : " << AstInterface::AstToString(mod_second) << std::endl;
            DumpDefSet(defvec,in);
          }
          typename std::map<AstNodePtr,Node*>::const_iterator p = defmap.find( mod_first);
          assert( p != defmap.end());
          Node* cur = (*p).second;
          CreateEdges( cur, mod_first);
          std::pair<AstNodePtr,AstNodePtr> mod(mod_first, mod_second);
          if (fa.IsVarRef(mod_first, 0, &varname, &scope)) {
            g->add_def( in, varname, scope, mod);
          }
          else {
            g->add_unknown_def( in, mod);
          }
          if (DebugDefUseChain()) {
            std::cerr << "finish processing gen mod info : " << AstInterface::AstToString(mod_first) << " : " << AstInterface::AstToString(mod_second) << std::endl;
            DumpDefSet(defvec,in);
          }
          return true;
      };
      std::function<bool(AstNodePtr, AstNodePtr)> opkill = 
                      [&defvec, g, &fa, &in]
       (AstNodePtr mod_first, AstNodePtr mod_second) {
          std::string varname;
          AstNodePtr scope;
          if (DebugDefUseChain()) {
            std::cerr << "processing kill mod info : " << AstInterface::AstToString(mod_first) << " : " << AstInterface::AstToString(mod_second) << std::endl;
            DumpDefSet(defvec,in);
          }
          if (fa.IsVarRef(mod_first, 0, &varname, &scope)) {
             ReachingDefinitions kill = g->get_def_set(varname, scope);
             kill.complement();
             in &= kill;
           }
           if (DebugDefUseChain()) {
             std::cerr << "finish processing kill mod info : " << AstInterface::AstToString(mod_first) << " : " << AstInterface::AstToString(mod_second) << std::endl;
             DumpDefSet(defvec,in);
           }
           return true;
      };
      std::list <AstNodePtr>& stmts = cur->GetStmts();
      for (std::list<AstNodePtr>::iterator p = stmts.begin(); p != stmts.end();
           ++p) {
        AstNodePtr cur = *p;
        if (DebugDefUseChain())
            std::cerr << "processing stmt : " << AstInterface::AstToString(cur) << std::endl;
        collect(cur, &opgen, &opread, &opkill);
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
#ifndef NDEBUG
        Node *tmp = *defp;
#endif
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

#else

void DefUseChainNode::Dump() const
{
  if (isdef)
    std::cerr << "definition: ";
  else
    std::cerr << "use: ";
  std::cerr << AstInterface::AstToString(ref) << " : " << AstInterface::AstToString(stmt) << std::endl;
}

std::string DefUseChainNode::toString() const
{
  std::string result;
  if (isdef)
     result = "definition:";
  else
    result = "use:";
  result = result + AstInterface::AstToString(ref) + ":" + AstInterface::AstToString(stmt) + " ";
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
