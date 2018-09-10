#include "TFHandles.h"
#include "CppStdUtilities.h"
#include "abstract_handle.h"

using namespace std;
using namespace AbstractHandle;

string TFHandles::getAbstractHandle(SgNode* node){
 abstract_node* anode = buildroseNode(node);
 abstract_handle ahandle(anode);
 return ahandle.toString();
}

SgNode*     TFHandles::getNodeFromHandle(SgProject* project, string handle){
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

vector<string> TFHandles::getHandleVector(set<SgNode*>  nodeSet){
  vector<string> handleVector;
  for(auto i = nodeSet.begin(); i != nodeSet.end(); ++i){
    handleVector.push_back(TFHandles::getAbstractHandle(*i));
  }
  return handleVector;
}

vector<string> TFHandles::getHandleVector(vector<SgNode*> nodeVector){
  vector<string> handleVector;
  for(auto i = nodeVector.begin(); i != nodeVector.end(); ++i){
    handleVector.push_back(TFHandles::getAbstractHandle(*i));
  }
  return handleVector;
}

vector<SgNode*>     TFHandles::getNodeVector(SgProject* project, vector<string> handleVector){
  vector<SgNode*> nodeVector;
  for(auto i = handleVector.begin(); i != handleVector.end(); ++i){
    SgNode* node = TFHandles::getNodeFromHandle(project, *i);
    if(node != nullptr) nodeVector.push_back(node);
  }
  return nodeVector;
}

string            TFHandles::getStringFromVector(vector<string> stringVector){
  string returnString = "";
  for(auto i = stringVector.begin(); i != stringVector.end(); ++i){
    if(returnString != "") returnString = returnString + "==";
    returnString = returnString + *i;
  }
  return returnString;
}

vector<string> TFHandles::getVectorFromString(string stringVector){
  return CppStdUtilities::splitByRegex(stringVector,"==");
}

string        TFHandles::getHandleVectorString(set<SgNode*>  nodeSet){
  vector<string> handleVector = TFHandles::getHandleVector(nodeSet);
  return TFHandles::getStringFromVector(handleVector);
}

string        TFHandles::getHandleVectorString(vector<SgNode*> nodeVector){
  vector<string> handleVector = TFHandles::getHandleVector(nodeVector);
  return TFHandles::getStringFromVector(handleVector);
}

vector<SgNode*> TFHandles::getNodeVectorFromString(SgProject* project, string stringVector){
  vector<string> handleVector = TFHandles::getVectorFromString(stringVector); 
  return TFHandles::getNodeVector(project, handleVector);
}
