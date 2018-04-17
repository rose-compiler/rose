#ifndef CASE_AST_ATTRIBUTE
#define CASE_AST_ATTRIBUTE

#include <string>
#include <rose.h>
#include <rosetollvm/RootAstAttribute.h>

class CaseAstAttribute : public RootAstAttribute { 
public: 
    CaseAstAttribute(std::string k, std::string l, bool e, bool r) : key(k),
                                                                     case_label(l),
                                                                     empty_body(e),
                                                                     reused_label(r)
    {} 

    bool emptyBody() { return empty_body; }
    bool reusedLabel() { return reused_label; }
    std::string getKey() { return key; }
    const std::string getCaseLabel() { return case_label; }

private:

    bool empty_body,
         reused_label;
    std::string key;
    std::string case_label;
};

#endif
