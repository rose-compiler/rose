// Compile-only test to ensure that BinaryAnalysis::Dominance can be subclassed.
#include "rose.h"
#include "BinaryDominance.h"

class MyDominance: public BinaryAnalysis::Dominance {
protected:
    typedef BinaryAnalysis::Dominance Super;
public:


    // Pretend that this is a function we want to override
    template<class ControlFlowGraph>
    void build_postdom_relation_from_cfg(const ControlFlowGraph &cfg,
                                         typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                         RelationMap<ControlFlowGraph> &result) {
        std::cout <<"MyDominance::build_postdom_relation_from_cfg()\n";
        Super::build_postdom_relation_from_cfg(cfg, start, result);
    }

    // build_postdom_relation_from_cfg() is polymorphic, so we need to override all forms.
    template<class ControlFlowGraph>
    RelationMap<ControlFlowGraph>
    build_postdom_relation_from_cfg(const ControlFlowGraph &cfg,
                                    typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start) {
        return Super::build_postdom_relation_from_cfg(cfg, start);
    }
    
    template<class ControlFlowGraph>
    RelationMap<ControlFlowGraph>
    build_postdom_relation_from_cfg(const ControlFlowGraph &cfg,
                                    typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                    typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor stop) {
        return Super::build_postdom_relation_from_cfg(cfg, start);
    }
    
    template<class ControlFlowGraph>
    void build_postdom_relation_from_cfg(const ControlFlowGraph &cfg,
                                         typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                         typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor stop,
                                         RelationMap<ControlFlowGraph> &idom/*out*/) {
        Super::build_postdom_relation_from_cfg(cfg, start, stop, idom);
    }
};

typedef BinaryAnalysis::ControlFlow::Graph CFG;
typedef boost::graph_traits<CFG>::vertex_descriptor CFG_Vertex;
typedef BinaryAnalysis::Dominance::Graph DG;
typedef BinaryAnalysis::Dominance::RelationMap<CFG> DG_RelMap;

static struct MyVisitor: AstSimpleProcessing {
    size_t nvisits;
    MyVisitor(): nvisits(0) {}
    void visit(SgNode *node) {
        SgAsmFunction *func = isSgAsmFunction(node);
        if (func && 0==func->get_name().compare("simple06")) {
            ++nvisits;
            CFG cfg = BinaryAnalysis::ControlFlow().build_cfg_from_ast<CFG>(func);
            CFG_Vertex start = 0;
            assert(get(boost::vertex_name, cfg, start)==func->get_entry_block());
            DG_RelMap dgmap1 = BinaryAnalysis::Dominance().build_postdom_relation_from_cfg(cfg, start);
            DG_RelMap dgmap2 = MyDominance().build_postdom_relation_from_cfg(cfg, start);
        }
    }
} visitor;
    
int
main(int argc, char *argv[])
{
    SgProject *project = frontend(argc, argv);
    visitor.traverse(project, preorder);
    assert(visitor.nvisits>0);
    return 0;
};

