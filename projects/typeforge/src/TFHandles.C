#include "TFHandles.h"
#include "CppStdUtilities.h"
#include "abstract_handle.h"

namespace Typeforge {

using namespace std;
using namespace AbstractHandle;

namespace TFHandles {

string getAbstractHandle(SgNode* node){
//if (isSgTemplateInstantiationFunctionDecl(node) != nullptr) return "";
 abstract_node* anode = buildroseNode(node);
 abstract_handle ahandle(anode);
 return ahandle.toString();
}

SgNode* getNodeFromHandle(SgProject* project, string handle){
  abstract_handle* ahandle = nullptr;
  try{
    abstract_node* rootNode = buildroseNode(project);
    abstract_handle* rootHandle = new abstract_handle(rootNode);
    ahandle = new abstract_handle(rootHandle,handle);
  }catch(...){}
  if(ahandle != nullptr){
    if(abstract_node* anode = ahandle->getNode()){
      return (SgNode*) anode->getNode();
    }
  }
  return nullptr;
}

vector<string> getHandleVector(set<SgNode*>  nodeSet){
  vector<string> handleVector;
  for(auto i = nodeSet.begin(); i != nodeSet.end(); ++i){
    handleVector.push_back(getAbstractHandle(*i));
  }
  return handleVector;
}

vector<string> getHandleVector(vector<SgNode*> nodeVector){
  vector<string> handleVector;
  for(auto i = nodeVector.begin(); i != nodeVector.end(); ++i){
    handleVector.push_back(getAbstractHandle(*i));
  }
  return handleVector;
}

vector<SgNode*> getNodeVector(SgProject* project, vector<string> handleVector){
  vector<SgNode*> nodeVector;
  for(auto i = handleVector.begin(); i != handleVector.end(); ++i){
    SgNode* node = getNodeFromHandle(project, *i);
    if(node != nullptr) nodeVector.push_back(node);
  }
  return nodeVector;
}

string getStringFromVector(vector<string> stringVector){
  string returnString = "";
  for(auto i = stringVector.begin(); i != stringVector.end(); ++i){
    if(returnString != "") returnString = returnString + "==";
    returnString = returnString + *i;
  }
  return returnString;
}

vector<string> getVectorFromString(string stringVector){
  return CppStdUtilities::splitByRegex(stringVector,"==");
}

string getHandleVectorString(set<SgNode*>  nodeSet){
  vector<string> handleVector = getHandleVector(nodeSet);
  return getStringFromVector(handleVector);
}

string getHandleVectorString(vector<SgNode*> nodeVector){
  vector<string> handleVector = getHandleVector(nodeVector);
  return getStringFromVector(handleVector);
}

vector<SgNode*> getNodeVectorFromString(SgProject* project, string stringVector){
  vector<string> handleVector = getVectorFromString(stringVector); 
  return getNodeVector(project, handleVector);
}

}

}

