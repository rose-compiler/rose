#include "sage3basic.h"
#include "AstNodeInfo.h"

using namespace CodeThorn;

namespace CodeThorn {

  AstNodeInfo::AstNodeInfo():label(0),initialLabel(0) {
  }

  std::string AstNodeInfo::toString() { std::stringstream ss;
    ss<<"\\n lab:"<<label<<" ";
    ss<<"init:"<<initialLabel<<" ";
    ss<<"final:"<<finalLabelsSet.toString();
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
    return newNodeInfo;
  }

  AstNodeInfo::OwnershipPolicy
  AstNodeInfo::getOwnershipPolicy() const {
    return CONTAINER_OWNERSHIP;
  }


  void AstNodeInfo::setLabel(CodeThorn::Label l) { 
    label=l;
  }

  void AstNodeInfo::setInitialLabel(CodeThorn::Label l) {
    initialLabel=l;
  }

  void AstNodeInfo::setFinalLabels(CodeThorn::LabelSet lset) {
    finalLabelsSet=lset;
  }

}
