#ifndef CFG_ROSE_H
#define CFG_ROSE_H

#include <AstInterface.h>

class CFGConfig {
 public:
  typedef enum {COND_TRUE, COND_FALSE, ALWAYS} EdgeType;
  static std::string EdgeType2String( EdgeType e);
};

template <class Node, class Edge>
class BuildCFGConfig  : public CFGConfig
{
 public:
  virtual Node* CreateNode() = 0;
  virtual Edge* CreateEdge( Node *n1, Node *n2, EdgeType condval) = 0;
  virtual void AddNodeStmt(Node* n, const AstNodePtr& s) = 0;
};

namespace ROSE_Analysis {
template <class Node, class Edge>
void BuildCFG ( AstInterface& fa, const AstNodePtr& head, BuildCFGConfig<Node,Edge>& g);
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

template <class Node, class Edge>
void OA2ROSE_CFG_Translate ( ROSE_CFG_Wrap& wrap, BuildCFGConfig<Node,Edge>& ng);

template <class Node, class Edge>
void BuildCFG ( AstInterface& fa, const AstNodePtr& head, BuildCFGConfig<Node,Edge>& g);
};

#endif
