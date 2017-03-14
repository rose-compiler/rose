#ifndef IF_COMPONENT_AST_ATTRIBUTE
#define IF_COMPONENT_AST_ATTRIBUTE

#include <rose.h>
#include <rosetollvm/RootAstAttribute.h>

class IfComponentAstAttribute : public RootAstAttribute { 
public: 
    IfComponentAstAttribute(std::string l, std::string e) : label(l),
                                                            end(e)
    {} 

    const std::string getLabel() { return label; }
    const std::string getEndLabel() { return end; }

protected:

    std::string label, 
                end;
};

#endif
