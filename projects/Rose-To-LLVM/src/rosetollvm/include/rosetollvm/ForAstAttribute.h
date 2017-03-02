#ifndef FOR_AST_ATTRIBUTE
#define FOR_AST_ATTRIBUTE

#include <rose.h>
#include <rosetollvm/RootAstAttribute.h>

class ForAstAttribute : public RootAstAttribute { 
public: 
    ForAstAttribute(std::string c, std::string b, std::string i, std::string e) : condition(c),
                                                                                  body(b),
                                                                                  increment(i),
                                                                                  end(e),
                                                                                  saved_label("?")
    {} 

    ForAstAttribute(ForAstAttribute *a) : condition(a -> condition),
                                          body(a -> body),
                                          increment(a -> increment),
                                          end(a -> end)
    {} 

    const std::string getConditionLabel() { return condition; }
    const std::string getBodyLabel() { return body; }
    const std::string getIncrementLabel() { return increment; }
    const std::string getEndLabel() { return end; }

    void saveEndOfBodyLabel(std::string label) { saved_label = label; }
    const std::string getEndOfBodyLabel() { return saved_label; }


private:

    std::string condition, 
                body,
                increment,
                end,

                saved_label;
};

#endif
