/** An implementation of binary control flow graph. The nodes of the graph are the virtual addresses of basic blocks and the
 *  directed edges follow the flow of control from one block to another, the so called "successors" of a block. This
 *  implementation emphasizes simplicity. */
class BinaryCFG {
public:
    BinaryCFG(): ast(NULL) {};
    explicit BinaryCFG(SgNode *ast): ast(NULL) { init(ast); }
    ~BinaryCFG() {}

    class NodeFunctor {
    public:
        virtual ~NodeFunctor() {}
        virtual void operator()(BinaryCFG*, SgAsmBlock*) = 0;
    };
    
    /** (Re)associates this graph with a new part of the AST. */
    void init(SgNode *ast) {
        clear();
        this->ast = ast;
        struct T1: public SgSimpleProcessing {
            BinaryCFG *cfg;
            T1(BinaryCFG *cfg): cfg(cfg) {}
            void visit(SgNode *node) {
                SgAsmBlock *bb = isSgAsmBlock(node);
                if (bb && !bb->get_statementList().empty() && isSgAsmInstruction(bb->get_statementList().front()))
                    cfg->graph_nodes[bb->get_address()] = bb;
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
            (f)(this, gni->second);
    }

    /** Returns the block at the specified address. Returns null if there's no block known to start at that address. */
    SgAsmBlock *block(rose_addr_t va) {
        NodeMap::iterator gni = graph_nodes.find(va);
        if (gni==graph_nodes.end()) return NULL;
        return gni->second;
    }

private:
    typedef std::map<rose_addr_t, SgAsmBlock*> NodeMap;
    SgNode *ast;                /**< Root of the part of the AST which this graph represents. */
    NodeMap graph_nodes;        /**< Maps virtual address to basic block */
};

/** A simple implementation of a binary function call graph. It calculates (during construction) the incoming function call
 *  edges for every function in the given control flow graph and regurgitates that info on demand. */
class BinaryCG {
public:
    /* Caller edges are from blocks of a function to addresses (either in other functions or unknown) */
    typedef std::pair<SgAsmBlock*, rose_addr_t> CallToEdge;
    typedef std::vector<CallToEdge> CallToEdges;
    typedef std::map<SgAsmFunctionDeclaration*, CallToEdges> CallerMap;
    CallerMap caller_edges;

    /* Callee edges are from the callee function back to the caller function (not block). We also keep track of the number of
     * calls from caller to callee. */
    typedef std::map<SgAsmFunctionDeclaration*, size_t> CallFromEdges;
    typedef std::map<SgAsmFunctionDeclaration*, CallFromEdges> CalleeMap;
    CalleeMap callee_edges;

    BinaryCG(BinaryCFG &cfg) {
        struct T1: public BinaryCFG::NodeFunctor {
            BinaryCG *cg;
            T1(BinaryCG *cg): cg(cg) {}
            void operator()(BinaryCFG *cfg, SgAsmBlock *src_bb) {
                SgAsmFunctionDeclaration *src_func = SageInterface::getEnclosingNode<SgAsmFunctionDeclaration>(src_bb);
                ROSE_ASSERT(src_func!=NULL);
                const SgAsmTargetPtrList &sucs = src_bb->get_successors();
                for (SgAsmTargetPtrList::const_iterator si=sucs.begin(); si!=sucs.end(); ++si) {
                    rose_addr_t dst_addr = (*si)->get_address();
                    SgAsmBlock *dst_bb = cfg->block(dst_addr);
                    SgAsmFunctionDeclaration *dst_func = dst_bb ?
                                                         SageInterface::getEnclosingNode<SgAsmFunctionDeclaration>(dst_bb) :
                                                         NULL;
                    if (src_func!=dst_func) {
                        cg->caller_edges[src_func].push_back(std::make_pair(src_bb, (*si)->get_address()));
                        if (dst_func)
                            cg->callee_edges[dst_func][src_func] += 1;
                    }
                }
            }
        } t1(this);
        cfg.apply(t1);
    }
};
