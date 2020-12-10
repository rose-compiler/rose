#include <PtrAnal.h>
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */


class PtrAnalBuildCFG
  : public BuildCFGConfig<std::list<PtrAnal::Stmt> >
{
  typedef std::list<PtrAnal::Stmt> Node;
  typedef std::list<std::pair<Node*,EdgeType> > EdgeInfo;

  std::list<Node> nodes;
  std::map<Node*, EdgeInfo> pending_in, pending_out;
  PtrAnal& m;
  PtrAnal::Stmt defn;
  std::string fname;
  AstInterface& fa;
 public:
  PtrAnalBuildCFG(AstInterface& _fa, PtrAnal& _m, const std::string& _fname, PtrAnal::Stmt _d)
     : m(_m), defn(_d), fname(_fname), fa(_fa) {}

  void solve_pending() {
     std::cerr << "solving pending \n";
    for (std::map<Node*, EdgeInfo>::const_iterator p_out = pending_out.begin();
        p_out != pending_out.end(); ++p_out) {
       Node* n1 = (*p_out).first;
       EdgeInfo edgeOut = (*p_out).second;
       if (n1->size()) {
         std::cerr << "source is no longer empty\n";
         PtrAnal::Stmt stmt = n1->back();
         for (EdgeInfo::const_iterator p_edge = edgeOut.begin();
              p_edge != edgeOut.end(); ++p_edge) {
             Node* n2 = (*p_edge).first;
             assert(n2->size());
             m.contrl_flow( stmt, n2->front(), (*p_edge).second);
         }
       }
       else {
         std::cerr << "source is still empty\n";
         std::map<Node*, EdgeInfo>::iterator p_in = pending_in.find(n1);
         assert(p_in != pending_in.end());
         EdgeInfo edgeIn = (*p_in).second;
         for (EdgeInfo::const_iterator p_edgeIn = edgeIn.begin();
                 p_edgeIn != edgeIn.end(); ++p_edgeIn) {
            n1 = (*p_edgeIn).first;
            assert(n1->size());
            PtrAnal::Stmt stmt1 = n1->back();
            EdgeType t = (*p_edgeIn).second;
            for (EdgeInfo::const_iterator p_edgeOut = edgeOut.begin();
                 p_edgeOut != edgeOut.end(); ++p_edgeOut) {
                EdgeType t2 = (*p_edgeOut).second;
                if (t != ALWAYS) {
                    if (t2 == ALWAYS)
                        t2 = t;
                    else {
                       std::cerr << "Error at edge type\n"; // assert(t2 == t);
                       t2 = t;
                    }
                }
                Node* n2 = (*p_edgeOut).first;
                assert(n2->size());
                 m.contrl_flow( stmt1, n2->front(), t2);
            }
          }
         pending_in.erase(p_in);
       }
    }
    pending_out.clear();
    if (pending_in.size()) {
       PtrAnal::Stmt r = m.funcexit_x(fname);
       for (std::map<Node*, EdgeInfo>::iterator p_in = pending_in.begin();
            p_in != pending_in.end(); ++p_in) {
         EdgeInfo edgeIn = (*p_in).second;
         for (EdgeInfo::const_iterator p_edgeIn = edgeIn.begin();
                 p_edgeIn != edgeIn.end(); ++p_edgeIn) {
            PtrAnal::Stmt stmt1 = (*p_edgeIn).first->back();
            EdgeType t = (*p_edgeIn).second;
            m.contrl_flow( stmt1, r, t);
         }
       }
    }
    pending_in.clear();
  }

  virtual Node* CreateNode()
    {
      Node n;
      nodes.push_back(n);
      return &nodes.back();
    }
  virtual void CreateEdge( Node *n1, Node *n2, EdgeType condval)
    {
      if (!n2->size()) {
          pending_in[n2].push_back(std::pair<Node*,EdgeType>(n1,condval));
          return;
      }
      if (!n1->size()) {
          pending_out[n1].push_back(std::pair<Node*,EdgeType>(n2,condval));
          return;
      }
      PtrAnal::Stmt s1 = n1->back(), s2 = n2->front();
      m.contrl_flow(s1, s2, condval);
    }
  virtual void AddNodeStmt(Node* n, const AstNodePtr& s)
    {
      PtrAnal::StmtRef s_stmts = m.translate_stmt(s);
      if (s_stmts.size() == 0) {
         if (fa.IsStatement(s))
            return;
         PtrAnal::Stmt s_exp = m.translate_exp(s).stmt;
         assert(s_exp != 0);
         s_stmts.push_back(s_exp);
      }
      if (n->size()) {
         m.contrl_flow(n->back(), s_stmts.front(), CFGConfig::ALWAYS);
      }
      else if (n == &nodes.front())
         m.contrl_flow(defn, s_stmts.front(), CFGConfig::ALWAYS);
      PtrAnal::StmtRef::const_iterator p = s_stmts.begin();
      n->push_back(*p);
      for (++p;  p != s_stmts.end(); ++p) {
         PtrAnal::Stmt cur = *p;
         m.contrl_flow(n->back(), cur, CFGConfig::ALWAYS);
         n->push_back(cur);
      }
      std::map<Node*, EdgeInfo>::iterator p_pending = pending_in.find(n);
      if (p_pending != pending_in.end()) {
         EdgeInfo& cur = (*p_pending).second;
         for (std::list<std::pair<Node*,EdgeType> >::const_iterator
               p_edge=cur.begin(); p_edge != cur.end(); ++p_edge) {
            Node* n1 = (*p_edge).first;
            if (n1->size())
                m.contrl_flow( n1->back(), n->front(), (*p_edge).second);
            else
               pending_out[n1].push_back(std::pair<Node*,EdgeType>(n, (*p_edge).second));
         }
         pending_in.erase(p_pending);
      }
    }
};

void PtrAnal::ControlFlowAnalysis(AstInterface& fa, const AstNodePtr& head, Stmt defn)
{
  if (stmts.size()) {
     PtrAnalBuildCFG op(fa, *this, fdefined.back(), defn);
     ROSE_Analysis::BuildCFG(fa, head, op);
     op.solve_pending();
  }
}


