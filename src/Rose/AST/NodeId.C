
#include "sage3basic.h"

#include "SgNodeHelper.h"
#include "Rose/AST/NodeId.h"
#include <boost/tokenizer.hpp>

namespace Rose {
namespace AST 
{
  //! \brief Get the Node ID for a particular SgNode*
    NodeId NodeId::getId(SgNode *node) {
        //ROSETTA code required to generate this for every AST
        //subclass...
      std::string stringId = SgNode::getNodeIdString(node);

      //Then we parse out the string to fill in the data on this NodeId
      std::size_t pos = stringId.find("_");
      std::string poolStr = stringId.substr(0, pos);
      std::string nodeStr = stringId.substr(pos+1);
      size_t poolId = stoull(poolStr, NULL);
      size_t nodeId = stoull(nodeStr, NULL);
      
      return NodeId(poolId, nodeId, node);
  };

/**
 * function NodeId::getId
 *
 * \brief Makes a nodeId from a nodeId String
 *
 * The string returned from ROSE core isn't always exactly the same as what we want
 * We optionally stick on the ROSE VERSION as a safety check.
 **/
    NodeId NodeId::getId(const std::string& nodeIdString) {
        boost::char_separator<char> sep("_");
        boost::tokenizer<boost::char_separator<char>> tok(nodeIdString, sep);
        boost::tokenizer<boost::char_separator<char>>::iterator token = tok.begin();
        VariantT variantT = static_cast<VariantT>(stoull(*token, NULL));
        ++token;
        size_t poolIndex = stoull(*token, NULL);
        ++token;
        size_t nodeIndex = stoull(*token, NULL);
#if NODEID_INCLUDE_ROSE_VERSION
        ++token;
        size_t rose_version = stoull(*token, NULL);
#endif //NODEID_INCLUDE_ROSE_VERSION
        ROSE_ASSERT(rose_version == ROSE_VERSION);
        SgNode* sgnode = SgNode::getNodeByNodeId(variantT, poolIndex, nodeIndex); 
        ROSE_ASSERT(sgnode->variantT() == variantT);  
        //Range is checked in getId(size_t)
        return NodeId(poolIndex, nodeIndex, sgnode);
    };

/**
 * function NodeId::toString
 *
 * \brief Makes a string to represent this NodeId
 *
 * The string returned from ROSE core isn't always exactly the same as what we want
 * We optionally stick on the ROSE VERSION as a safety check.
 **/
    std::string NodeId::toString() const {
        ROSE_ASSERT(poolIndex != std::numeric_limits<size_t>::max());
        std::stringstream ss;
        ss << node->variantT();
        ss << "_" << poolIndex; 
        ss << "_" << nodeIndex;  
#if NODEID_INCLUDE_ROSE_VERSION
        ss << "_" << ROSE_VERSION;        //Optional check on ROSE_VERSION
#endif //NODEID_INCLUDE_ROSE_VERSION
        return ss.str();
  };



} 
}
