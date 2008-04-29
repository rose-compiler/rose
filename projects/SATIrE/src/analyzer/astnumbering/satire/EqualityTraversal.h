#ifndef EQUALITY_TRAVERSAL_H
#define EQUALITY_TRAVERSAL_H

#include <rose.h>
#include <map>
#include <ext/hash_map>

typedef unsigned long EqualityId;
//typedef SgNode* NodeInfo;
typedef std::pair<SgNode*, std::vector<EqualityId> > NodeInfo;

// hashmap support functions
class NodeHash {
public:
  size_t operator()(NodeInfo node) const;
};
class NodeEqual {
public:
  bool operator()(NodeInfo s1, NodeInfo s2) const;
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
  EqualityTraversal() {}

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

protected:
  EqualityId
  evaluateSynthesizedAttribute(SgNode* astNode,
			       SynthesizedAttributesList synList);
  EqualityId
  defaultSynthesizedAttribute();

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

};


#endif
