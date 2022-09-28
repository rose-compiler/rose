// ################################################################
// #                           Header Files                       #
// ################################################################

#include "ROSETTA_macros.h"
#include "grammar.h"
#include "AstNodeClass.h"
#include "grammarString.h"
#include <sstream>

using namespace std;

/**
 * \Function buildMemoryPoolBasedNodeId
 *
 * This function generated code for looking up nodes by NodeId, and
 * generating NodeId from an SgNode*.  There is further support for
 * this functionality in SgNode and in NodeId.h/C
 **/
string
Grammar::buildMemoryPoolBasedNodeId()
{
    stringstream ss;
    
    /**
     * \function getNodeIdItemIndex
     *
     * Generates code to find an SgNode in a particular pool. So given a
     * pool and an SgNode*, it finds the index of the correct node, which
     * becomes part of its NodeId
     **/
    
    ss << "\n\nstd::string SgNode::getNodeIdString(SgNode* sgNode)\n   {\n";
    ss << "  VariantT thisVariantT = sgNode->variantT();\n\n";
    ss << "  switch(thisVariantT) {\n";
    
    /* Now we have a giant case statement of every possible AST Node Types */ 
    for (unsigned int i=0; i < terminalList.size(); i++)
    {
        string name = terminalList[i]->name;
        ss << "  case V_" << name << ":\n";
        ss << "  {\n";
        ss << "    return " << name << "::getNodeIdStringInternal(reinterpret_cast<" << name << "*>(sgNode));\n";
        ss << "    }\n";
    }
    
    ss << "  default:\n";
    ss << "    ROSE_ABORT();\n";
    ss << "  } //end switch(thisVariantT)\n\n";
    
    ss << "  ROSE_ABORT();\n";
       
    ss <<"}\n";
    
    
/*    ss << "  default:\n";
    ss << "    ROSE_ABORT();\n";
    ss << "  } //end switch(thisVariantT)\n";
    ss << "  //Code below should be dead\n";
    ss << "  return std::numeric_limits<size_t>::max();\n";
    ss << "}\n\n";
*/
    //buildGetNodeIdString();       
    // Generates code to generate a NodeId for a given SgNode*.  This
    // function contains the code to find the correct pool, then call
    // NodeIdItemIndex on it to get the item index.
/*
    ss << "\n\nstd::string SgNode::getNodeIdString(SgNode* node) {\n";
    ss << "  //Values picked to always return one past the intended pool.\n";
    ss << "  //lower_bound will usually return one past, but not if node is the\n";
    ss << "  //first node in the pool.  Consistency is better.\n";
    ss << "  std::tuple<unsigned char*, unsigned, VariantT> fakeitem(reinterpret_cast<unsigned char*>(node), std::numeric_limits<size_t>::max(), V_SgNumVariants);\n";
    ss << "  typename std::vector<std::tuple<unsigned char*, unsigned, VariantT>>::iterator poolIt;\n";
    ss << "  poolIt = std::upper_bound(all_pools.begin(), all_pools.end(), fakeitem);\n";
    ss << "  ROSE_ASSERT(poolIt != all_pools.begin() && poolIt <= all_pools.end());\n";
    ss << "  poolIt--;  //We always want the pool before the \"first that compares greater\".\n";
    ss << "  size_t poolIdx = std::distance(all_pools.begin(), poolIt);  //Check this to make index from iterator\n";
    ss << "  size_t itemIdx = getNodeIdItemIndex(*poolIt, node);\n\n";
    ss << "  std::stringstream ss;\n";
    ss << "  ss << poolIdx << \"_\" << itemIdx;\n";
    ss << "  return ss.str();\n";Internal
    ss << "}\n";
*/

// getNodeByNodeId();           
// Generates code to lookup an SgNode by it's NodeId expressed as two ints
    ss << "\n\nSgNode*  SgNode::getNodeByNodeId(VariantT thisVariantT, size_t poolIdx, size_t itemIdx)\n    {\n";
    ss << "  switch(thisVariantT) {\n";
    /* Now we have a giant case statement of every possible AST Node Type */
    for (unsigned int i=0; i < terminalList.size(); i++)
    {
        string name = terminalList[i]->name;
        ss << "  case V_" << name << ":\n";
        ss << "  {\n";
        ss << "    return " << name << "::getNodeByNodeIdInternal(poolIdx, itemIdx);\n";
        ss << "  }\n";
    }
    ss << "  default:\n";
    ss << "    ROSE_ABORT();\n";
    ss << "  } //end switch(thisVariantT)\n\n";

    ss << "  //This should never happen\n";
    ss << "  return nullptr;\n";
       
    ss <<"}\n";
       
    return ss.str();
};

/*    
        {
            
ss << "  VariantT thisVariantT =  std::get<2>(pool);\n\n";
    ss << "  switch(thisVariantT) {\n";
    for (unsigned int i=0; i < terminalList.size(); i++)
    {
        string name = terminalList[i]->name;
        ss << "  case V_" << name << ":\n";
        ss << "  {\n";
        ss << "    " << name << "* poolStart = reinterpret_cast<" << name << "*>(std::get<0>(pool));\n"; 
        ss << "    ROSE_ASSERT(itemIdx * sizeof(" << name << ") < poolSizeInBytes);\n";
        ss << "    return poolStart+itemIdx;\n";
        ss << "  }\n";
    }
    ss << "  default:\n";
    ss << "    ROSE_ABORT();\n";
    ss << "  } //end switch(thisVariantT)\n\n";

    ss << "  //This should never happen\n";
    ss << "  return nullptr;\n";
       
    ss <<"}\n";
       
    return ss.str();

*/
