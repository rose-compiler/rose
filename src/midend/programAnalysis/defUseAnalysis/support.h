/******************************************
 * Category: DFA
 * DefUse Analysis Declaration
 * created by tps in Feb 2007
 *****************************************/

#ifndef __DFAnalysis_support_HXX_LOADED__
#define __DFAnalysis_support_HXX_LOADED__
//#include "rose.h"
#include <string>

// A set of utility functions
class Support {
 public:

  /**********************************************************
   *  Convert anything to a string
   *********************************************************/
  template<typename T>
    std::string ToString(T t) {
    std::ostringstream myStream; //creates an ostringstream object
    myStream << t << std::flush;
    return(myStream.str()); //returns the string form of the stringstream object
  }

  /**********************************************************
   *  Resolve Boolean Value to String
   *********************************************************/
  std::string resBool(bool val) {
    if (val)
      return "true";
    return "false";
  }

  /**********************************************************
   *  Check if an element is contained in a vector
   *********************************************************/
  template <typename T>
    bool isContainedinVector(T filterNode,   std::vector<T> worklist) {
    bool contained = false;
    for (typename std::vector<T >::const_iterator l = worklist.begin(); l != worklist.end(); ++l) {
      T aNode = *l;
      if (aNode == filterNode)
        contained = true;
    }
    return contained;
  }



  /* *****************************************
   * retrieve a sepcific name for functionNodes
   * must be the same for all retrievals, so that
   * analyses work.
   * *****************************************/
  ::std::string getAppName(SgFunctionDeclaration* functionDeclaration) {
    std::string nodeNameApp = "";
    std::vector<SgNode*> children = functionDeclaration->get_parameterList()->get_traversalSuccessorContainer();
    for (unsigned int i=0; i< children.size(); i++) {
      SgInitializedName* initName = (SgInitializedName*)children[i];
      nodeNameApp = nodeNameApp + ""+initName->get_type()->unparseToString();
      if (i!=(children.size()-1))
        nodeNameApp = nodeNameApp + ", ";
      // yed can not handle & signs.. replacing
      while (nodeNameApp.find("&")!=std::string::npos) {
        int pos = nodeNameApp.find("&");
        nodeNameApp.replace(pos,1,"?");
      }
    }
    std::string retVal = "("+nodeNameApp+")"; //+"-"+NodeToString(functionDeclaration);
    return retVal;
  }


  std::string getFileNameString(std::string src) {
    return src;
  }

  std::string getFullName(SgFunctionDefinition* functionDef) {
    SgFunctionDeclaration* functionDeclaration = functionDef->get_declaration();
    ::std::string fullName = functionDeclaration->get_qualified_name().str();

    if ((fullName.find("std::") != std::string::npos) ||
        (fullName.find("__") != std::string::npos) ||
        (fullName.find("operator") != std::string::npos)
        ) return ""; // Explicitly ignore all nodes in the ::std namespace

    std::string filename = getFileNameString(functionDeclaration->get_file_info()->get_filename());
    if ((filename.find("/usr/") != std::string::npos)
        ) return ""; // Explicitly ignore all nodes in the ::std namespace
    fullName = fullName+getAppName(functionDeclaration);
    return fullName;
  }

};

#endif
