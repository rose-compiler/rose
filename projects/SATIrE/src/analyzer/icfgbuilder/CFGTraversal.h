#ifndef H_CFGTRAVERSAL
#define H_CFGTRAVERSAL

#include <config.h>
#include <rose.h>

#include <map>

#include "cfg_support.h"

// this must be used preorder
class CFGTraversal : public AstSimpleProcessing
{
public:
    CFGTraversal(std::deque<Procedure *> *);
    CFG *getCFG();

    void print_map() const;
    std::map<int, SgStatement *> block_stmt_map;

protected:
    void visit(SgNode *);

private:
    CFGTraversal();
    BasicBlock *transform_block(SgBasicBlock *block, BasicBlock *after,
            BasicBlock *break_target, BasicBlock *continue_target);
    BlockList *do_switch_body(SgBasicBlock *block, BasicBlock *after,
            BasicBlock *continue_target);
    int find_procnum(const char *) const;
    BasicBlock *allocate_new_block(BasicBlock *, BasicBlock *);
    BasicBlock *allocate_block_without_successor(BasicBlock *);
    SgStatement *rewrite_statement(const SgStatement *,
            std::vector<SgVariableSymbol *> *);
    void perform_goto_backpatching();
    void number_exprs();
    bool is_destructor_decl(SgFunctionDeclaration *) const;
    BasicBlock *call_base_destructors(Procedure *, BasicBlock *);
    int node_id;
    int procnum;
    CFG *cfg, *real_cfg;
    Procedure *proc;
    int call_num;
    int lognum;
    int expnum;
    SgStatement *current_statement;
};

#endif
