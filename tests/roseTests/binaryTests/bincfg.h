/** An implementation of binary control flow graph. The nodes of the graph are the virtual addresses of basic blocks and the
 *  directed edges follow the flow of control from one block to another, the so called "successors" of a block.  Not all
 *  successors can be determined statically, and the complete() method invoked on a node will indicate whether all successors
 *  are known.
 *
 *  This implementation emphasizes performance and simplicity.  Rather than recalculating outgoing edges at each request, the
 *  edge list is cached.  Computing successors is a somewhat intensive operation since it may require semantic analysis of the
 *  entire basic block. */
class BinaryCFG {
private:
    /** Type of each node of the graph. */
    struct Node {
        Node(): ast_node(NULL), is_current(false) {}
        SgAsmBlock *ast_node;                           /**< The basic block to which this graph node corresponds. */
        bool is_current;                                /**< True if successors have been calculated. */
        Disassembler::AddressSet successors;            /**< List of all known successors if is_current is true */
        bool complete;                                  /**< True if all possible successors are known */
    };
    typedef std::map<rose_addr_t, Node> NodeMap;
    typedef std::vector<SgAsmInstruction*> Instructions;

public:
    BinaryCFG(): ast(NULL) {};
    explicit BinaryCFG(SgNode *ast): ast(NULL) { init(ast); }
    ~BinaryCFG() { clear(); }

    class NodeFunctor {
    public:
        virtual ~NodeFunctor() {}
        virtual void operator()(BinaryCFG*, rose_addr_t) = 0;
    };
    
    /** (Re)associates this graph with a new part of the AST.  If @p ast is the same node to which the graph is initially
     *  rooted then all that happens is the edge information is thrown away and recalculated as necessary. */
    void init(SgNode *ast) {
        clear();
        this->ast = ast;
        struct T1: public SgSimpleProcessing {
            BinaryCFG *cfg;
            T1(BinaryCFG *cfg): cfg(cfg) {}
            void visit(SgNode *node) {
                SgAsmBlock *bb = isSgAsmBlock(node);
                if (bb && !bb->get_statementList().empty() && isSgAsmInstruction(bb->get_statementList().front()))
                    cfg->graph_nodes[bb->get_address()].ast_node = bb;
            }
        };
        T1(this).traverse(ast, preorder);
    }

    /** Clears the node and edge information that's cached. */
    void clear() {
        graph_nodes.clear();
    }

    /** Removes nodes associated with a particular function. */
    void erase(SgAsmFunctionDeclaration *func) {
        const SgAsmStatementPtrList &stmts = func->get_statementList();
        for (SgAsmStatementPtrList::const_iterator si=stmts.begin(); si!=stmts.end(); ++si) {
            SgAsmBlock *bb = isSgAsmBlock(*si);
            if (bb) graph_nodes.erase(bb->get_address());
        }
    }
    
    /** Invokes the functor at every node of the graph. */
    void apply(NodeFunctor &f) {
        for (NodeMap::iterator gni=graph_nodes.begin(); gni!=graph_nodes.end(); ++gni)
            (f)(this, gni->first);
    }

    /** Returns true if the all successors of the specified node are known. Returns false if the specified address does not
     *  exist as a node of the graph. */
    bool is_complete(rose_addr_t va) {
        NodeMap::iterator gni=graph_nodes.find(va);
        if (gni==graph_nodes.end()) return false;
        return cache_edges(gni->second);
    }

    /** Returns the known successors for a node at the specified virtual address. */
    const Disassembler::AddressSet &successors(rose_addr_t va) {
        static const Disassembler::AddressSet empty;
        NodeMap::iterator gni = graph_nodes.find(va);
        if (gni==graph_nodes.end()) return empty;
        cache_edges(gni->second);
        return gni->second.successors;
    }

    /** Returns the function containing the specified graph node. */
    SgAsmFunctionDeclaration *function(rose_addr_t va) {
        NodeMap::iterator gni = graph_nodes.find(va);
        if (gni==graph_nodes.end()) return NULL;
        return isSgAsmFunctionDeclaration(gni->second.ast_node->get_parent());
    }

    /** Class utility function to return the vector of instructions associated with a basic block. */
    static Instructions instructions(SgAsmBlock *bb) {
        Instructions retval;
        const SgAsmStatementPtrList &stmts = bb->get_statementList();
        for (SgAsmStatementPtrList::const_iterator si=stmts.begin(); si!=stmts.end(); ++si) {
            SgAsmInstruction *insn = isSgAsmInstruction(*si);
            if (insn)
                retval.push_back(insn);
        }
        return retval;
    }

private:
    bool cache_edges(Node &n) {
        if (!n.is_current) {
            Instructions insns = instructions(n.ast_node);
            ROSE_ASSERT(!insns.empty());
            n.successors = insns.front()->get_successors(insns, &n.complete);
            n.is_current = true;
        }
        return n.complete;
    }
            
    SgNode *ast;                /**< Root of the part of the AST which this graph represents. */
    NodeMap graph_nodes;        /**< Cached info about each basic block. */
};


/** An implementation of a binary function call graph. This adapts the control flow graph to serve as a function call graph.
 *  The implementation emphasizes simplicity and performance, calculating and caching control flow information only when
 *  necessary. */
class BinaryCG {
public:
    class NodeFunctor {
    public:
        virtual ~NodeFunctor() {}
        virtual void operator()(BinaryCG*, SgAsmFunctionDeclaration*) = 0;
    };
    
    BinaryCG(BinaryCFG &cfg): p_cfg(cfg) {}

    /** Returns the control flow graph associated with this call graph. */
    BinaryCFG &cfg() { return p_cfg; }

    /** Returns the set of functions that call the specified function. The specified function is included in the list if it
     *  has a flow control edge from any of its basic blocks to its entry basic block. */
    std::set<SgAsmFunctionDeclaration*> callers(SgAsmFunctionDeclaration *func) {
        struct T1: public BinaryCFG::NodeFunctor {
            SgAsmFunctionDeclaration *func;
            std::set<SgAsmFunctionDeclaration*> callers;
            T1(SgAsmFunctionDeclaration *func): func(func) {}
            void operator()(BinaryCFG *cfg, rose_addr_t va) {
                SgAsmFunctionDeclaration *caller = cfg->function(va);
                const Disassembler::AddressSet &sucs = cfg->successors(va);
                for (Disassembler::AddressSet::const_iterator si=sucs.begin(); si!=sucs.end(); ++si) {
                    SgAsmFunctionDeclaration *callee = cfg->function(*si);
                    if (callee==func && (caller!=func || *si==func->get_entry_va()))
                        callers.insert(caller);
                }
            }
        } t1(func);
        p_cfg.apply(t1);
        return t1.callers;
    }

    /** Returns the addresses that the specified function branches to. These addresses are the entry points of functions and
     *  thus are also nodes of the underlying control flow graph.  The specified function's entry address is included in the
     *  returned list if any block of the function branches to the entry point of the function. */
    Disassembler::AddressSet callees(SgAsmFunctionDeclaration *func) {
        struct T1: public BinaryCFG::NodeFunctor {
            SgAsmFunctionDeclaration *func;
            Disassembler::AddressSet callees;
            T1(SgAsmFunctionDeclaration *func): func(func) {}
            void operator()(BinaryCFG *cfg, rose_addr_t va) {
                if (cfg->function(va)==func) {
                    const Disassembler::AddressSet &sucs = cfg->successors(va);
                    for (Disassembler::AddressSet::const_iterator si=sucs.begin(); si!=sucs.end(); ++si) {
                        SgAsmFunctionDeclaration *callee = cfg->function(*si);
                        if (callee!=func || *si==func->get_entry_va())
                            callees.insert(*si);
                    }
                }
            }
        } t1(func);
        p_cfg.apply(t1);
        return t1.callees;
    }

    /** Invokes the functor at every node of the graph. */
    void apply(NodeFunctor &f) {
        struct T1: public BinaryCFG::NodeFunctor {
            std::set<SgAsmFunctionDeclaration*> funcs;
            void operator()(BinaryCFG *cfg, rose_addr_t va) {
                SgAsmFunctionDeclaration *func = cfg->function(va);
                if (func) funcs.insert(func);
            }
        } t1;
        p_cfg.apply(t1);

        for (std::set<SgAsmFunctionDeclaration*>::iterator fi=t1.funcs.begin(); fi!=t1.funcs.end(); ++fi)
            (f)(this, *fi);
    }

private:
    BinaryCFG &p_cfg;
};
