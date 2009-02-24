#include <rose.h>
#include "CFGVisitor.hh"
#include "DBFactory.hh"
#include <ext/hash_set>
#include <deque>

using namespace VirtualCFG;
using namespace std;

Element *getElement(SgNode*);
Element *getNumber(unsigned int);

extern Domain *nodes;
extern Domain *numbers;

CFGVisitor::CFGVisitor()
{
    Domain *sig[4]= {nodes,numbers,nodes,numbers};
    string doc[4]= {"source node","source index", "target node", "target index"};

    cfgNext= db->createRelation( "cfgNext", 4, sig, doc );
#if 0
    cfgBefore= db->createRelation( "cfgBefore", 2, sig, doc );
    cfgAfter= db->createRelation( "cfgAfter", 2, sig, doc );
#endif
}

struct hashCFGNode {
size_t operator()( const CFGNode &n ) const {
    return ((size_t)n.getNode())^(~n.getIndex());
}
};


/* depth fist search of control flow grpah */
void
CFGVisitor::explore( SgNode *node )
{
    __gnu_cxx::hash_set<CFGNode,hashCFGNode> visited;
    std::deque<CFGNode> tovisit;

    tovisit.push_back( makeCfg(node) );

    while (tovisit.size() != 0) {
        CFGNode n= tovisit.back(); /* front for breadth */
        tovisit.pop_back(); /* front for breadth */
        visited.insert(n);

        vector<CFGEdge> edges= n.outEdges();
        for (unsigned int i= 0; i < edges.size(); i++) {
            Element *tuple[4];
            tuple[0]= getElement( edges[i].source().getNode() );
            tuple[1]= getNumber( edges[i].source().getIndex() );
            tuple[2]= getElement( edges[i].target().getNode() );
            tuple[3]= getNumber( edges[i].target().getIndex() );
            cfgNext->insert(tuple);

            CFGNode target= edges[i].target();
            if (visited.find(target) == visited.end())
                tovisit.push_back( target );
        }
    }
}

void
CFGVisitor::visit( SgNode *node )
{
    if (isSgFunctionDefinition(node)) {
        explore(node);
    }
}

#if 0

/*random access control flowvisit*/
void
CFGVisitor::visit( SgNode *node )
{
    if (!(
            isSgStatement(node)
            || isSgExpression(node)
            || isSgInitializedName(node)
       ))
    {
        return;
    }

    SgStatement *stmt= isSgStatement(node);
    if (stmt) {
        SgScopeStatement *s= stmt->get_scope();
        if (!(
                isSgBasicBlock(s)
                || isSgCatchOptionStmt(s)
                || isSgDoWhileStmt(s)
                || isSgForStatement(s)
                || isSgFunctionDefinition(s)
                || isSgIfStmt(s)
                || isSgSwitchStatement(s)
                || isSgWhileStmt(s)
            ))
        {
            return;
        }
    }

    SgInitializedName *iname= isSgInitializedName(node);
    if (iname) {
        SgScopeStatement *s= iname->get_scope();
        if (!(
                isSgBasicBlock(s)
                || isSgCatchOptionStmt(s)
                || isSgDoWhileStmt(s)
                || isSgForStatement(s)
                || isSgFunctionDefinition(s)
                || isSgIfStmt(s)
                || isSgSwitchStatement(s)
                || isSgWhileStmt(s)
            ))
        {
            return;
        }
    }

    int i= 0;
    for (
            SgNode *ancestor= node->get_parent();
            ancestor && i < 2;
            ancestor= ancestor->get_parent(), i++
        )
    {
        if (isSgType(ancestor)) {
            return;
        }
    }

    if ( isSgBasicBlock(node) && isSgIfStmt( node->get_parent() ) ) {
        cerr << node->get_file_info()->displayString() << endl;
        cerr << node->unparseToString() << endl;
    }


    Element *tuple[4];

    VirtualCFG::InterestingNode cfgnode= VirtualCFG::makeInterestingCfg(node);
    vector<VirtualCFG::InterestingEdge> edges = cfgnode.outEdges();

    for (int i= 0; i < edges.size(); i++) {
        tuple[0]= getElement( edges[i].source().getNode() );
        tuple[1]= getNumber( edges[i].source().getIndex() );
        tuple[2]= getElement( edges[i].target().getNode() );
        tuple[3]= getNumber( edges[i].target().getIndex() );
        cfgNext->insert(tuple);
    }

    tuple[0]= getElement( edges[0].source().getNode() );
    tuple[1]= getNumber( edges[0].source().getIndex() );
    cfgBefore->insert(tuple);
    tuple[0]= getElement( edges[edges.size()-1].source().getNode() );
    tuple[1]= getNumber( edges[edges.size()-1].source().getIndex() );
    cfgAfter->insert(tuple);
}
#endif

