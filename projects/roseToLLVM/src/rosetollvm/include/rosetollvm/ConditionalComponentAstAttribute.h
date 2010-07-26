#ifndef CONDITIONAL_COMPONENT_AST_ATTRIBUTE
#define CONDITIONAL_COMPONENT_AST_ATTRIBUTE

class ConditionalComponentAstAttribute : public IfComponentAstAttribute { 
public: 
    ConditionalComponentAstAttribute(std::string l, std::string e) : IfComponentAstAttribute(l, e)

    {} 

    void setLastLabel(const std::string last_label_) { last_label = last_label_; }
    const std::string getLastLabel() { return last_label; }

private:

    std::string last_label;
};

#endif
