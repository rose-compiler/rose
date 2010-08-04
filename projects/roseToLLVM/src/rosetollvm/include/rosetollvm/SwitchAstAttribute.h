#ifndef SWITCH_AST_ATTRIBUTE
#define SWITCH_AST_ATTRIBUTE

#include <rose.h>
#include <rosetollvm/RootAstAttribute.h>
#include <rosetollvm/CaseAstAttribute.h>

class SwitchAstAttribute : public RootAstAttribute { 
public: 
    SwitchAstAttribute(std::string entry_label_, std::string end_label_) : entry_label(entry_label_),
                                                                           end_label(end_label_),
                                                                           default_stmt(NULL)
    {} 
  /*
    SwitchAstAttribute(SwitchAstAttribute *a) : entry_label(a -> entry_label),
                                                end_label(a -> end_label),
                                                default_stmt(a -> default_stmt),
                                                case_attributes(a -> case_attributes)
    {}
  */
    const std::string getEntryLabel() { return entry_label; }
    int numCaseAttributes() { return case_attributes.size(); }
    void addCaseAttribute(CaseAstAttribute *case_attribute) { case_attributes.push_back(case_attribute); }
    CaseAstAttribute *getCaseAttribute(int i) { return case_attributes[i]; }
    const std::string getEndLabel() { return end_label; }

    void setDefaultStmt(SgDefaultOptionStmt *default_stmt_) { default_stmt = default_stmt_; }
    SgDefaultOptionStmt *getDefaultStmt() { return default_stmt; }

private:

    std::string entry_label;
    std::vector<CaseAstAttribute *> case_attributes;
    std::string end_label;

    SgDefaultOptionStmt *default_stmt;
};

#endif
