// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: cfg_funcs.C,v 1.3 2007-07-15 02:02:27 markus Exp $

#include "CFGTraversal.h"
#include "iface.h"

extern "C" KFG kfg_create(KFG p)
{
    /*
     * Initialize the CFG.
     * Since it is built before the PAG functions are invoked, there
     * is nothing left to do.
     */
    return p;
}

extern "C" int kfg_num_nodes(KFG cfg)
{
    /*
     * Returns the number of nodes in the CFG.
     */
    return ((CFG *) cfg)->nodes.size();
}

extern "C" KFG_NODE_TYPE kfg_node_type(KFG, KFG_NODE node)
{
    /*
     * Returns the type of the node.
     */
    if (node == NULL)
        std::cout << "null node" << std::endl;
    return ((BasicBlock *) node)->node_type;
}

extern "C" KFG_NODE_ID kfg_get_id(KFG, KFG_NODE node)
{
    /*
     * Returns the identifier of the node.
     */
    return ((BasicBlock *) node)->id;
}

extern "C" KFG_NODE kfg_get_node(KFG cfg, KFG_NODE_ID n)
{
    /*
     * Returns the node with the given identifier.
     */
    return ((CFG *) cfg)->nodes[n];
}

extern "C" int kfg_get_bbsize(KFG, KFG_NODE node)
{
    /*
     * Returns the number of instructions in the node.
     */
    return ((BasicBlock *) node)->statements.size();
}

extern "C" void *kfg_get_bbelem(KFG, KFG_NODE node, int n)
{
    /*
     * Returns the n-th instruction of the node, starting with 0.
     */
    if (((BasicBlock *) node)->statements.size() == 0)
        return NULL;
    else
        return ((BasicBlock *) node)->statements[n];
}

extern "C" void kfg_node_infolabel_print_fp(FILE *file, KFG kfg,
        KFG_NODE node, int n)
{
    /*
     * Writes a textual description of the n-th instruction in the
     * node to the file. This is used for visualisation.
     */
    SgStatement *stmt = (SgStatement *) kfg_get_bbelem(kfg, node, n);
    if (stmt == NULL)
    {
        fprintf(file, "empty BB");
        return;
    }
    DeclareStmt *declare_stmt = dynamic_cast<DeclareStmt *>(stmt);
    UndeclareStmt *undeclare_stmt = dynamic_cast<UndeclareStmt *>(stmt);
    CallStmt *call_stmt = dynamic_cast<CallStmt *>(stmt);
    ExternalCall *external_call = dynamic_cast<ExternalCall *>(stmt);
    ConstructorCall *constructor_call = dynamic_cast<ConstructorCall *>(stmt);
    DestructorCall *destructor_call = dynamic_cast<DestructorCall *>(stmt);
    ArgumentAssignment *argument_assignment
        = dynamic_cast<ArgumentAssignment *>(stmt);
    ParamAssignment *param_assignment
      = dynamic_cast<ParamAssignment *>(stmt);
    ReturnAssignment *return_assignment
        = dynamic_cast<ReturnAssignment *>(stmt);
    LogicalIf *logical_if
        = dynamic_cast<LogicalIf *>(stmt);
    IfJoin *if_join = dynamic_cast<IfJoin *>(stmt);
    WhileJoin *while_join = dynamic_cast<WhileJoin *>(stmt);

    std::string result;
    
    if (external_call)
        result = "EXTERNAL CALL";
    else if (declare_stmt)
        result = declare_stmt->unparseToString();
    else if (undeclare_stmt)
        result = undeclare_stmt->unparseToString();
    else if (constructor_call)
        result = std::string("ConstructorCall(")
            + constructor_call->get_name() + ")";
    else if (destructor_call)
        result = std::string("DestructorCall(")
            + destructor_call->get_name() + ")";
    else if (call_stmt)
        result = call_stmt->unparseToString();
    else if (argument_assignment)
        result = argument_assignment->unparseToString();
    else if (param_assignment)
        result = param_assignment->unparseToString();
    else if (return_assignment)
        result = return_assignment->unparseToString();
    else if (logical_if)
        result = logical_if->unparseToString();
    else if (if_join)
        result = if_join->unparseToString();
    else if (while_join)
        result = while_join->unparseToString();
    else
    {
        switch (stmt->variantT())
        {
        case V_SgNullStatement:
            fprintf(file, "NullStatement (no-op)");
            break;
        case V_SgBreakStmt:
            fprintf(file, "break;");
            break;
        case V_SgLabelStatement:
            fprintf(file, "%s:", isSgLabelStatement(stmt)->get_label().str());
            break;
            /*
        case V_SgIfStmt:
            {
                SgIfStmt *ifs = isSgIfStmt(stmt);
                fprintf(file, "if (%s)",
                        ifs->get_conditional()->unparseToString().c_str());
            }
            break;
            */
        case V_SgForStatement:
            {
#if 0
                SgForStatement *fors = isSgForStatement(stmt);
                SgStatement *init = fors->get_for_init_stmt();
                SgExpression *test = fors->get_test_expr();
                SgExpression *incr = fors->get_increment_expr();
                fprintf(file, "for (%s;%s;%s)",
                    (init != NULL ? init->unparseToString().c_str() : ""),
                    (test != NULL ? test->unparseToString().c_str() : "?"),
                    (incr != NULL ? incr->unparseToString().c_str() : ""));
#else
                SgForStatement *fors = isSgForStatement(stmt);
                SgStatement *test = fors->get_test();
                fprintf(file, "for (;%s;)",
                    (test != NULL ? test->unparseToString().c_str() : "*"));
#endif
            }
            break;
            
        case V_SgWhileStmt:
            {
                SgWhileStmt *whiles = isSgWhileStmt(stmt);
                result = std::string("while (")
                    + whiles->get_condition()->unparseToString() + ")";
            }
            break;
            
        case V_SgDoWhileStmt:
            {
                SgDoWhileStmt *dowhiles = isSgDoWhileStmt(stmt);
                result = std::string("do-while (")
                    + dowhiles->get_condition()->unparseToString() + ")";
            }
            break;
            /*
        case V_SgSwitchStatement:
            {
                SgSwitchStatement *switchs = isSgSwitchStatement(stmt);
                fprintf(file, "switch (%s)",
                        switchs->get_item_selector_root()->get_operand_i()
                            ->unparseToString().c_str());
            }
            break;
            */
        case V_SgCaseOptionStmt:
            {
                SgCaseOptionStmt *cases = isSgCaseOptionStmt(stmt);
                result = std::string("case ")
                    + cases->get_key()->unparseToString() + ":";
            }
            break;
        case V_SgDefaultOptionStmt:
            {
                result = "default:";
            }
            break;
        default:
            result = stmt->unparseToString();
            break;
        }
    }
    if (result.find("\"") != std::string::npos)
    {
        /* quote double quotes with backslash */
        std::string::size_type pos = (std::string::size_type) -2;
        while ((pos = result.find("\"", pos+2)) != std::string::npos)
            result.replace(pos, 1, "\\\"");
        /* TODO: think about escaping backslashes */
        /* maybe like this? escape all backslashes that are
         * followed by a backslash */
        pos = (std::string::size_type) -1;
        while ((pos = result.find("\\", pos+1)) != std::string::npos)
        {
            if (result.at(pos+1) == '\\')
            {
                result.replace(pos, 1, "\\\\");
                pos += 1;
            }
        }
    }
    fprintf(file, "%s", result.c_str());
}

extern "C" KFG_NODE_LIST kfg_predecessors(KFG, KFG_NODE node)
{
    /*
     * Returns the list of predecessors of the node.
     */
    BasicBlock *block = (BasicBlock *) node;
    BlockList *preds = new BlockList();
    std::vector<Edge>::const_iterator i;
    for (i = block->predecessors.begin(); i != block->predecessors.end(); ++i)
        preds->push_back(i->first);
    return new BlockListIterator(preds, preds->begin());
}

extern "C" KFG_NODE_LIST kfg_successors(KFG, KFG_NODE node)
{
    /*
     * Returns the list of successors of the node.
     */
    BasicBlock *block = (BasicBlock *) node;
    BlockList *succs = new BlockList();
    std::vector<Edge>::const_iterator i;
    for (i = block->successors.begin(); i != block->successors.end(); ++i)
        succs->push_back(i->first);
    return new BlockListIterator(succs, succs->begin());
}

extern "C" KFG_NODE kfg_get_call(KFG, KFG_NODE node)
{
    /*
     * Returns the call node belonging to a return node.
     */
    return ((CallBlock *) node)->partner;
}

extern "C" KFG_NODE kfg_get_return(KFG, KFG_NODE node)
{
    /*
     * Returns the return node belonging to a call node.
     */
    return ((CallBlock *) node)->partner;
}

extern "C" KFG_NODE kfg_get_start(KFG, KFG_NODE node)
{
    /*
     * Returns the start node belonging to an end node.
     */
    return ((CallBlock *) node)->partner;
}

extern "C" KFG_NODE kfg_get_end(KFG, KFG_NODE node)
{
    /*
     * Returns the end node belonging to a start node.
     */
    return ((CallBlock *) node)->partner;
}

extern "C" int const *kfg_get_beginnings(KFG)
{
    /*
     * Returns a pointer to an array of procedure numbers,
     * terminated by -1, to start the analysis with. If the function
     * returns an empty list (contains only -1) then the analyzer
     * selects an entry point automatically (see chapter 2.7.2).
     */
    const static int end = -1;
    return &end;
}

extern "C" int kfg_replace_beginnings(KFG, int const *)
{
    /*
     * Replaces the beginnings list of the front end. Can be called
     * after initialization of the CFG before the analysis. The
     * caller has to make the memory management of the arry. The
     * front end should not call, free, or modify the data in the
     * array. Returns 1 for success, 0 if the feature is not
     * supported by the front end, or -1 for an error.
     */
    return 0;
}

extern "C" KFG_NODE kfg_node_list_head(KFG_NODE_LIST list)
{
    /*
     * Returns head of list.
     */
    BlockListIterator *i = (BlockListIterator *) list;
    return *(i->second);
}

extern "C" KFG_NODE_LIST kfg_node_list_tail(KFG_NODE_LIST list)
{
    /*
     * Returns list without the first element.
     */
    BlockListIterator *i = (BlockListIterator *) list;
    return new BlockListIterator(i->first, i->second + 1);
}

extern "C" int kfg_node_list_is_empty(KFG_NODE_LIST list)
{
    /*
     * Returns 1 if the list is empty, 0 otherwise.
     */
    BlockListIterator *i = (BlockListIterator *) list;
    return (i->first->end() == i->second);
}

extern "C" int kfg_node_list_length(KFG_NODE_LIST list)
{
    /*
     * Returns length of node list.
     */
    BlockListIterator *i = (BlockListIterator *) list;
    BlockList::const_iterator j;
    int len = 0;
    for (j = i->second; j != i->first->end(); ++j)
        len++;
    return len;
}

extern "C" unsigned int kfg_edge_type_max(KFG)
{
    /*
     * Returns the number of differend edge types.
     */
    return EDGE_TYPE_MAX;
}

extern "C" KFG_EDGE_TYPE kfg_edge_type(KFG_NODE n1, KFG_NODE n2)
{
    /*
     * Returns the type of the edge from the first node to the
     * second. It leads to a runtime error if there is no such edge.
     */
    BasicBlock *pred = (BasicBlock *) n1;
    BasicBlock *succ = (BasicBlock *) n2;

    std::vector<Edge>::const_iterator i;
    for (i = pred->successors.begin(); i != pred->successors.end(); ++i)
        if (i->first == succ)
            return i->second;

    /* no match found, "runtime error" */
    std::cerr << "ERROR: there is no edge from node " << n1
        << " to node " << n2 << std::endl;
    return 42;
}

extern "C" int kfg_which_in_edges(KFG_NODE node)
{
    /*
     * Returns a bitmask where for every edge type that labels an
     * incoming edge the corresponding bit is set.
     */
    BasicBlock *block = (BasicBlock *) node;

    int mask = 0;
    std::vector<Edge>::const_iterator i;
    for (i = block->predecessors.begin(); i != block->predecessors.end(); ++i)
        mask |= (1U << i->second);
    return mask;
}

extern "C" int kfg_which_out_edges(KFG_NODE node)
{
    /*
     * As kfg_which_in_edges, but for outgoing edges.
     */
    BasicBlock *block = (BasicBlock *) node;

    int mask = 0;
    std::vector<Edge>::const_iterator i;
    for (i = block->successors.begin(); i != block->successors.end(); ++i)
        mask |= (1U << i->second);
    return mask;
}

extern "C" int kfg_num_procs(KFG cfg)
{
    /*
     * Returns the number of procedures in the CFG.
     */
    return ((CFG *) cfg)->procedures->size();
}

extern "C" char *kfg_proc_name(KFG kfg, int n)
{
    /*
     * Returns static pointer to the name of a procedure.
     */
    CFG *cfg = (CFG *) kfg;
    if ((*cfg->procedures)[n]->memberf_name != NULL)
        return (char *) (*cfg->procedures)[n]->memberf_name;
    else
        return (char *) (*cfg->procedures)[n]->name;
}

extern "C" KFG_NODE kfg_numproc(KFG cfg, int n)
{
    /*
     * Returns entry node of a procedure.
     */
    return (*((CFG *) cfg)->procedures)[n]->entry;
}

extern "C" int kfg_procnumnode(KFG, KFG_NODE node)
{
    /*
     * Returns the number of the procedure the node belongs to.
     */
    return ((BasicBlock *) node)->procnum;
}

extern "C" int kfg_procnum(KFG cfg, KFG_NODE_ID id)
{
    /*
     * Returns the number of the procedure the node with the given
     * identifier belongs to.
     */
    return kfg_procnumnode(cfg, kfg_get_node(cfg, id));
}

extern "C" KFG_NODE_LIST kfg_all_nodes(KFG kfg)
{
    /*
     * Returns list of all nodes.
     */
    CFG *cfg = (CFG *) kfg;
    return new BlockListIterator(&cfg->nodes, cfg->nodes.begin());
}

extern "C" KFG_NODE_LIST kfg_entrys(KFG kfg)
{
    /*
     * Returns list of all entry nodes.
     */
    CFG *cfg = (CFG *) kfg;
    return new BlockListIterator(&cfg->entries, cfg->entries.begin());
}

extern "C" KFG_NODE_LIST kfg_calls(KFG kfg)
{
    /*
     * Returns list of all call nodes.
     */
    CFG *cfg = (CFG *) kfg;
    return new BlockListIterator(&cfg->calls, cfg->calls.begin());
}

extern "C" KFG_NODE_LIST kfg_returns(KFG kfg)
{
    /*
     * Returns list of all return nodes.
     */
    CFG *cfg = (CFG *) kfg;
    return new BlockListIterator(&cfg->returns, cfg->returns.begin());
}

extern "C" KFG_NODE_LIST kfg_exits(KFG kfg)
{
    /*
     * Returns list of all exit nodes.
     */
    CFG *cfg = (CFG *) kfg;
    return new BlockListIterator(&cfg->exits, cfg->exits.begin());
}
