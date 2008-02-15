
#ifndef LOOP_TREE_LOCALITY
#define LOOP_TREE_LOCALITY

#include <LoopTreeDepComp.h>

struct DepCompAstRef { 
  AstNodePtr orig; 
  LoopTreeNode* stmt; 
  DepCompAstRef(const AstNodePtr& o, LoopTreeNode* s) 
       : orig(o),stmt(s) {}
  std::string ToHandle() const;
  std::string ToString() const;

// DQ (9/25/2007): Added operator< to permit compilation using STL debugging mode!
// bool operator<(const DepCompAstRef & x) const { return false; };
};

// DQ (9/25/2007): This is a seperate function
bool operator < (const DepCompAstRef& n1, const DepCompAstRef& n2);

class AstRefToString {
public:
static std::string ToString( const DepCompAstRef& r) { return r.ToString(); }
};

typedef GraphNodeTemplate<DepCompAstRef, const DepCompAstRef&,AstRefToString> 
DepCompAstRefGraphNode;

class DepCompAstRefAnal
{
  std::map<LoopTreeNode*, int> stmtmap;
  std::map<AstNodePtr,int> refmap;
  LoopTreeNode* root;
  
    int get_ref_index( const AstNodePtr& ref) const
       { 
            std::map<AstNodePtr, int>:: const_iterator p = refmap.find(ref); 
            assert(p  != refmap.end());
            int res =  (*p).second; 
            return (res > 0)? res : -res;
       }
    void Append( LoopTransformInterface& ai, LoopTreeNode* root);
  public:
    DepCompAstRefAnal(LoopTransformInterface& ai,  LoopTreeNode* _root) 
         : root(_root) { Append(ai, root); }

    LoopTreeNode* get_tree_root() const { return root; }
    int get_stmt_size() const { return stmtmap.size(); }
    bool has_stmt(LoopTreeNode* s) const { return stmtmap.find(s) != stmtmap.end(); } 
    bool is_mod_ref( const AstNodePtr& ref) const
       {
            std::map<AstNodePtr, int>:: const_iterator p = refmap.find(ref); 
            assert(p  != refmap.end());
            return  (*p).second < 0; 
       }

    int CompareStmt( LoopTreeNode* s1, LoopTreeNode* s2)  const
      { 
         std::map<LoopTreeNode*, int>:: const_iterator p1 = stmtmap.find(s1), p2 = stmtmap.find(s2); 
         assert(p1  != stmtmap.end() && p2 != stmtmap.end());
         int i1 = (*p1).second, i2 = (*p2).second; 
         return (i1 == i2)? 0 : ( (i1 < i2)? -1 : 1);
      }
    int CompareAstRef( const DepCompAstRef& r1, const DepCompAstRef& r2) const
      {
         int res = CompareStmt(r1.stmt,r2.stmt);
         if (res == 0 && r1.orig != r2.orig) {
             res = (get_ref_index(r1.orig) < get_ref_index(r2.orig))? -1 : 1;
         }
         return res;
      }
};

class LoopTreeLocalityAnal 
{
  LoopTreeDepCompCreate& comp;
  DepInfoAnal& anal;
  LoopTreeDepGraphCreate inputCreate;
  LoopTransformInterface &fa;

  void ComputeInputDep( LoopTreeDepGraphNodeIterator iter, DepCompAstRefAnal& stmtorder);
  void ComputeInputDep( LoopTreeDepGraphNodeIterator src,
                        LoopTreeDepGraphNodeIterator snk, DepCompAstRefAnal& stmtorder);
  void ComputeInputDep( LoopTreeDepGraphNode *n1, LoopTreeDepGraphNode *n2, 
                        DepCompAstRefAnal& stmtorder);
 public:
  typedef std::set<AstNodePtr> AstNodeSet;
  LoopTreeLocalityAnal( LoopTransformInterface& _fa, LoopTreeDepCompCreate& c);
  ~LoopTreeLocalityAnal();

  LoopTreeDepGraph* GetInputGraph() { return &inputCreate; }
  LoopTreeDepComp& GetDepComp() { return comp; }

  float SelfSpatialReuses( LoopTreeNode *n, int loop, int linesize);
  int TemporaryReuseRefs(LoopTreeNode *s1, int loop1, LoopTreeNode *s2, int loop2,
                   AstNodeSet &refSet, int reuseDist);
   LoopTransformInterface& GetLoopTransformInterface() { return fa; }
};

class DepCompAstRefGraphCreate: public DepInfoGraphCreate<DepCompAstRefGraphNode> 
{
  typedef std::map<DepCompAstRef, DepCompAstRefGraphNode*> AstRefNodeMap;

// DQ (9/25/2007): This does not appear to be used, since DepCompAstRef has no operator<().  
// It is used in LoopTreeLocality.C (function DepCompAstRefGraphCreate::CreateNode()).
   AstRefNodeMap refNodeMap;

 public:
  DepCompAstRefGraphNode* 
        CreateNode(const AstNodePtr& r, LoopTreeNode* stmt);
  DepInfoEdge* 
      CreateEdge( DepCompAstRefGraphNode* src,  DepCompAstRefGraphNode* snk, const DepInfo& dep);
  void Build(LoopTransformInterface& la, LoopTreeLocalityAnal& tc, LoopTreeNode* root) ;

//Boolean SelfReuseLevel( const DepCompAstRefGraphNode* n, int level) const;
  int SelfReuseLevel( const DepCompAstRefGraphNode* n, int level) const;
};

#endif
