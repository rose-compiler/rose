#ifndef AST_NODE_INFO_H
#define AST_NODE_INFO_H

#include "Labeler.h"

namespace CodeThorn {
  
  class AstNodeInfo : public AstAttribute {
  public:

    AstNodeInfo();
    virtual std::string attribute_class_name() const;
    virtual AstNodeInfo* copy();
    AstNodeInfo::OwnershipPolicy getOwnershipPolicy() const ROSE_OVERRIDE;
    std::string toString();
    void setLabel(CodeThorn::Label l);
    void setInitialLabel(CodeThorn::Label l);
    void setFinalLabels(CodeThorn::LabelSet lset);

  private:
    CodeThorn::Label label;
    CodeThorn::Label initialLabel;
    CodeThorn::LabelSet finalLabelsSet;
  };

} // end of namespace CodeThorn
  
#endif
