#include "sage3basic.h"
#include "AstNodeInfo.h"

using namespace CodeThorn;

namespace CodeThorn {

  AstNodeInfo::AstNodeInfo():label(0),initialLabel(0) {
  }

  std::string AstNodeInfo::toString() { std::stringstream ss;
    if(labelInfoAvailable) {
      ss<<"\\n lab:"<<label<<" ";
      ss<<"init:"<<initialLabel<<" ";
      ss<<"final:"<<finalLabelsSet.toString();
    }
    if(varName!=""||varType!="") {
      ss<<"\\n"<<varType<<"\\n"<<varName;
    }
    return ss.str(); 
  }

  std::string AstNodeInfo::attribute_class_name() const {
    return "AstNodeInfo";
  }

  AstNodeInfo* AstNodeInfo::copy() {
    AstNodeInfo* newNodeInfo=new AstNodeInfo();
    newNodeInfo->label=this->label;
    newNodeInfo->initialLabel=this->initialLabel;
    newNodeInfo->finalLabelsSet=this->finalLabelsSet;
    newNodeInfo->labelInfoAvailable=this->labelInfoAvailable;
    newNodeInfo->varName=this->varName;
    newNodeInfo->varType=this->varType;
    return newNodeInfo;
  }

  AstNodeInfo::OwnershipPolicy
  AstNodeInfo::getOwnershipPolicy() const {
    return CONTAINER_OWNERSHIP;
  }


  void AstNodeInfo::setLabel(CodeThorn::Label l) { 
    labelInfoAvailable=true;
    label=l;
  }

  void AstNodeInfo::setInitialLabel(CodeThorn::Label l) {
    labelInfoAvailable=true;
    initialLabel=l;
  }

  void AstNodeInfo::setFinalLabels(CodeThorn::LabelSet lset) {
    labelInfoAvailable=true;
    finalLabelsSet=lset;
  }

  void AstNodeInfo::setVarName(std::string name) {
    varName=name;
  }

  void AstNodeInfo::setVarType(std::string type) {
    varType=type;
  }

}
