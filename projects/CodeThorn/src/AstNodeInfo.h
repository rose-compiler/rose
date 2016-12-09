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
    void setLabel(SPRAY::Label l);
    void setInitialLabel(SPRAY::Label l);
    void setFinalLabels(SPRAY::LabelSet lset);

  private:
    SPRAY::Label label;
    SPRAY::Label initialLabel;
    SPRAY::LabelSet finalLabelsSet;
  };

} // end of namespace CodeThorn
  
#endif
