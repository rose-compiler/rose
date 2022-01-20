#ifndef AST_NODE_INFO_H
#define AST_NODE_INFO_H

#include "Labeler.h"

namespace CodeThorn {
  
  class AstNodeInfo : public AstAttribute {
  public:

    AstNodeInfo();
    virtual std::string attribute_class_name() const;
    virtual AstNodeInfo* copy();
    AstNodeInfo::OwnershipPolicy getOwnershipPolicy() const override;
    std::string toString();
    void setLabel(CodeThorn::Label l);
    void setInitialLabel(CodeThorn::Label l);
    void setFinalLabels(CodeThorn::LabelSet lset);
    void setVarName(std::string name);
    void setVarType(std::string type);

  private:
    CodeThorn::Label label;
    CodeThorn::Label initialLabel;
    CodeThorn::LabelSet finalLabelsSet;
    bool labelInfoAvailable=false;
    std::string varName;
    std::string varType;
  };

} // end of namespace CodeThorn
  
#endif
