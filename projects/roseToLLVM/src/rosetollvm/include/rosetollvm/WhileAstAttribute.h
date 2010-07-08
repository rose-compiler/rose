#ifndef WHILE_AST_ATTRIBUTE
#define WHILE_AST_ATTRIBUTE

#include <rose.h>
#include <rosetollvm/RootAstAttribute.h>

class WhileAstAttribute : public RootAstAttribute { 
public: 
    WhileAstAttribute(std::string c, std::string b, std::string e) : condition(c),
                                                                     body(b),
                                                                     end(e)
    {} 

    WhileAstAttribute(WhileAstAttribute *a) : condition(a -> condition),
                                              body(a -> body),
                                              end(a -> end)
    {} 

    const std::string getConditionLabel() { return condition; }
    const std::string getBodyLabel() { return body; }
    const std::string getEndLabel() { return end; }

private:

    std::string condition, 
                body,
                end;
};

#endif
