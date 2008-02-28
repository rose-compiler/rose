#ifndef CFG_ROSE_H
#define CFG_ROSE_H

#include <AstInterface.h>
// See include of <CFG_ROSE.h> at bottom

class CFGConfig {
 public:
  typedef enum {COND_TRUE, COND_FALSE, ALWAYS} EdgeType;
  static std::string EdgeType2String( EdgeType e);
};

template <class Node>
class BuildCFGConfig  : public CFGConfig
{
 public:
  virtual Node* CreateNode() = 0;
  virtual void CreateEdge( Node *n1, Node *n2, EdgeType condval) = 0;
  virtual void AddNodeStmt(Node* n, const AstNodePtr& s) = 0;
  virtual ~BuildCFGConfig() {}
};

namespace ROSE_Analysis {
template <class Node>
void BuildCFG ( AstInterface& fa, const AstNodePtr& head, BuildCFGConfig<Node>& g);
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
};

// This has to be at the bottom of the file
#include <CFG_ROSE.h>

#endif
