
#include "sage3basic.h"
#include "sageInterface.h"

#include "SgNodeHelper.h"
#include "Combinatorics.h"  //Rose hashers
#include "Rose/AST/NodeId.h"
#include <boost/tokenizer.hpp>

namespace Rose {
  namespace AST 
  {
    //define the static hash
    std::string NodeId::run_hash; 

    NodeId::NodeId(SgNode *inNode) : node(inNode) {
      //ROSETTA code required to generate this for every AST
      //subclass...
      std::string stringId = SgNode::getNodeIdString(node);
    
      //Then we parse out the string to fill in the data on this NodeId
      std::size_t pos = stringId.find("_");
      std::string poolStr = stringId.substr(0, pos);
      std::string nodeStr = stringId.substr(pos+1);
      poolIndex = stoull(poolStr, NULL);
      nodeIndex = stoull(nodeStr, NULL);

      if(run_hash.size() == 0) {
        initRunHash();
      }
    
    }

    /**
     *  \brief Initialize the run_hash variable for error checking.
     *
     *  No reason to call publically, all paths to usage of run_hash
     *  should call this.
     *
     *  Assumes there is only one SgProject node
     **/
    void NodeId::initRunHash() 
    {
      if(run_hash.size() == 0) {
        Rose::Combinatorics::HasherFnv fnv;
        //This assumes we cannot have more than one SgProject node 2023/03/13
        SgProject* root = SageInterface::getProject();
        ROSE_ASSERT(root != NULL);
      
        fnv.insert(std::to_string(ROSE_VERSION));
        auto fileList = root->get_fileList_ptr()->get_listOfFiles();
        for(auto file : fileList) {
          fnv.insert(file->getFileName());
        }
        run_hash = fnv.toString();
      }    
    }
  

    //! \brief Get the Node ID for a particular SgNode*
    NodeId NodeId::getId(SgNode *node) {
      return NodeId(node);
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
      if(run_hash.size() == 0) {
        initRunHash();
      }
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
      std::string in_run_hash = *token;
#endif //NODEID_INCLUDE_ROSE_VERSION
      ROSE_ASSERT(run_hash == in_run_hash);
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
      if(run_hash.size() == 0) {
        initRunHash();
      }
      ROSE_ASSERT(poolIndex != std::numeric_limits<size_t>::max());
      std::stringstream ss;
      ss << node->variantT();
      ss << "_" << poolIndex; 
      ss << "_" << nodeIndex;  
#if NODEID_INCLUDE_ROSE_VERSION
      ss << "_" << run_hash;        //Optional check on ROSE_VERSION
#endif //NODEID_INCLUDE_ROSE_VERSION
      return ss.str();
    };



  } 
}
