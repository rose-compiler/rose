#ifndef EQUALITY_TRAVERSAL_H
#define EQUALITY_TRAVERSAL_H

#include <satire_rose.h>
#include <satire.h>

#include <map>

// MS: using TR1 we will probably replace this with include<unordered_map>
// but this requires additional work
#include <ext/hash_map>

typedef unsigned long EqualityId;
//typedef SgNode* NodeInfo;
typedef std::pair<SgNode*, std::vector<EqualityId> > NodeInfo;

// hashmap support functions
class NodeHash {
public:
  size_t operator()(const NodeInfo& node) const;

private:
// GB: Supporting hash functions.
  static size_t hashString(const std::string &str);
  static size_t hashInt(int val);
  static size_t hashChildren(SgNode *node);
  template <class C> static size_t hashMangledName(C *c);
  static size_t hashInitializedNamePtrList(SgInitializedNamePtrList &args);
  static size_t hashValueExp(SgValueExp *value);
  static size_t hashVarious(const NodeInfo &node);
  static size_t hashDeclarationStatement(SgDeclarationStatement *decl);
};
class NodeEqual {
public:
  bool operator()(const NodeInfo& s1, const NodeInfo& s2) const;

private:
// GB: Supporting equality functions.
  static bool variantsEqual(const NodeInfo& s1, const NodeInfo& s2);
  static bool childrenEqual(const NodeInfo& s1, const NodeInfo& s2);
  template <class C> static bool compareNamedThings(C *i1, C *i2);
  static bool compareInitializedNamePtrList(SgInitializedNamePtrList &a1,
                                            SgInitializedNamePtrList &a2);
  static bool compareVarious(const NodeInfo& s1, const NodeInfo& s2);
};

class NodeAddressHash {
public:
  size_t operator()(const SgNode* node) const;
};

// the traversal class
typedef __gnu_cxx::hash_map<NodeInfo, EqualityId, NodeHash, NodeEqual> 
SimpleNodeMap;


class EqualityTraversal : public AstBottomUpProcessing<EqualityId> {
public:
  EqualityTraversal(SATIrE::Program *program);

  enum NodeStringRepresentation { FullRepresentation, 
				  IdRepresentation, 
				  TreeRepresentation };
 
  EqualityId get_node_id(SgNode* node);
  const std::vector<SgNode*>& get_nodes_for_id(EqualityId id);
  std::string get_node_repr(EqualityId id, NodeStringRepresentation r);

  void output_tables();

  std::string get_node_specifics(SgNode* id);
  std::string get_id_specifics(EqualityId id);

  //we might or might not need this
  //std::string get_node_repr(SgNode* node, NodeStringRepresentation r);

  void get_all_exprs(std::vector<EqualityId>& ids);
  void get_all_types(std::vector<EqualityId>& ids);

// GB (2008-05-20): Made this public because occasionally we want to call it
// directly on type nodes.
  EqualityId
  evaluateSynthesizedAttribute(SgNode* astNode,
			       SynthesizedAttributesList synList);

protected:
  EqualityId
  defaultSynthesizedAttribute();
  void atTraversalEnd();

private:
  SimpleNodeMap node_map;

  bool equal_child_ids(SgNode* n, SgNode* m);

  bool add_extra_criteria(std::stringstream& out, SgNode* node);
  void get_node_repr_recursive(std::stringstream& out, EqualityId id, 
			       NodeStringRepresentation r);

  // map id -> nodes
  __gnu_cxx::hash_map<EqualityId, std::vector<SgNode*> > node_list_map;
  // map node -> id
  __gnu_cxx::hash_map<SgNode*, EqualityId, NodeAddressHash> node_id_map;
  // map id -> children
  __gnu_cxx::hash_map<EqualityId, std::vector<EqualityId> > id_child_map;

  EqualityId idForNode(const NodeInfo& node);

  SATIrE::Program *program;

  EqualityTraversal();
};


#endif
