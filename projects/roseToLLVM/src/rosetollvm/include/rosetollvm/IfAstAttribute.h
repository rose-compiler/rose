#ifndef IF_AST_ATTRIBUTE
#define IF_AST_ATTRIBUTE

#include <rose.h>
#include <rosetollvm/RootAstAttribute.h>

class IfAstAttribute : public RootAstAttribute { 
public: 
    IfAstAttribute(std::string t, std::string f, std::string e) : trueLabel(t),
                                                                  falseLabel(f),
                                                                  end(e)
    {} 

    IfAstAttribute(IfAstAttribute *a) : trueLabel(a -> trueLabel),
                                        falseLabel(a -> falseLabel),
                                        end(a -> end)
    {} 

    const std::string getTrueLabel() { return trueLabel; }
    const std::string getFalseLabel() { return falseLabel; }
    const std::string getEndLabel() { return end; }

protected:

    std::string trueLabel, 
                falseLabel,
                end;
};

#endif
