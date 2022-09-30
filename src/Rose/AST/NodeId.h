
#ifndef ROSE_AST_NodeId_H
#define ROSE_AST_NodeId_H

#include <string>
#include <limits>

#define NODEID_INCLUDE_ROSE_VERSION 1


class SgNode;

namespace Rose { namespace AST {

/**
 * \class NodeId
 *
 * \brief A mostly static class for creating and storing Unique Node Identifiers
 *
 * This class by be used to uniquely identify a node by a string, or
 * find a particular node from that string.  There is no storage of
 * the IDs on the AST or anywhere else.  This class includes a couple
 * of numbers, but those only exist as the NodeId does.  Node and Ids
 * are found and generated algorithmically.  So using NodeIds does not
 * increase the memory cost of ROSE.
 *
 * It is based on the Node's
 * location in the memory pool, so it is based on the stability of the
 * memory pool.  So it has a few constraints.
 * It will be stable between runs of ROSE iff:
 *  1. The same version of ROSE is used
 *  2. The same command line arguments are given
 *  3. The same files are on the command line in the same order.
 *  4. No transformations occur.
 *
 * Under those constraints the construction of the AST should be
 * deterministic, so the memory pool construction should be as well.
 * One hopes.  There is one optimization that I may have to change if
 * it becomes an issue.  I'm leaving a note here in case it becomes an
 * issue.
 * The memory pools are stored via sorted insert in the pools array of
 * each AST node type.  This makes the lookup slightly faster as the
 * memory pool is sorted.  However, it is possible that as the AST is
 * constructed a memory pool pointer could be allocatted out of order,
 * resulting in a different order of memory pools.  If this happens I
 * suggest we go back to a regular push_back instead of a
 * sorted_insert, and then when an ID is generated for an SgNode, we
 * will have to use linear search.  I haven't seen this in testing. -Jim Leek
 * 
 */
class NodeId {

private:  

  // \brief private internal constructor
    NodeId(size_t poolId, size_t nodeId, SgNode* inNode) : poolIndex(poolId), nodeIndex(nodeId), node(inNode) {} 

    
    //DATA---------------------  
    //I'm not that concerned about data size because these aren't stored for each node, they can be
    //recomputed at will
    // The number of the memory pool this node lives at
    size_t poolIndex;
    // The index of this item in the memory pool 
    size_t nodeIndex;
  
    //! \brief The node this ID applies to
    SgNode* node;

  
public:
  //! \brief default constructor required for containers, but only makes invalid NodeIds 
  NodeId() : poolIndex(std::numeric_limits<size_t>::max()), nodeIndex(std::numeric_limits<size_t>::max()), node(nullptr) {}

  //! \brief copy constructor
  NodeId(const NodeId &rhs) : poolIndex(rhs.poolIndex), nodeIndex(rhs.nodeIndex), node(rhs.node) {}

  //! \brief assignment operator
  NodeId& operator=(const NodeId& rhs) {
    poolIndex = rhs.poolIndex;
    nodeIndex = rhs.nodeIndex;
    node = rhs.node;
    return *this;
  }
  
  //! \brief Get the Node ID for a particular SgNode*
    static const NodeId getId(SgNode *node);

  //! \brief Get the Node ID from a string (e.g. from json)
    static const NodeId getId(const std::string& nodeIdString); 

  //! \brief Get the SgNode from a string (convinience function)
  static SgNode *getNode(const std::string& nodeIdString) {
    return getId(nodeIdString).getNode();
  }

  //! \brief Get the SgNode* contained in this NodeId
  SgNode* getNode() const { return node;};
  
  //! \brief Get this node ID as a string
    std::string toString() const;

  bool operator==(const NodeId& rhs) const {
    if(poolIndex == rhs.poolIndex &&
       nodeIndex == rhs.nodeIndex &&
       node == rhs.node) {
      return true;
    }
    return false;
  }
  
};
    
} }

#endif /* ROSE_AST_NodeId_H */

