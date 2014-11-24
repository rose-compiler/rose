// DQ (10/5/2014): This is more strict now that we include rose_config.h in the sage3basic.h.
// #include "rose.h"
#include "sage3basic.h"

#include "SingleStatementToBlockNormalization.h"
#include <vector>

using namespace std;

class SingleStatementToBlockVisitor: public ROSE_VisitorPatternDefaultBase {
private:
    vector<SgStatement*>  singleStatementBlocks;
public:
    SingleStatementToBlockVisitor(){}
    virtual ~SingleStatementToBlockVisitor() {singleStatementBlocks.clear();}
    void Normalize() {
        for (vector<SgStatement*>::iterator it = singleStatementBlocks.begin(); it != singleStatementBlocks.end(); it++) {
            SageInterface::makeSingleStatementBodyToBlock(*it);
        }
    }
    
protected:
    virtual void visit(SgNode * node) {
    }
    
#define BLOCKIFY_ONE_BODY(type, access) \
virtual void visit(type * node){\
if(node->access() && !isSgBasicBlock(node->access())) {\
singleStatementBlocks.push_back(node->access());\
}}
    
#define BLOCKIFY_TWO_BODY(type, access1, access2)\
virtual void visit(type * node){\
if(node->access1() && !isSgBasicBlock(node->access1())) {\
singleStatementBlocks.push_back(node->access1());\
} \
if(node->access2() && !isSgBasicBlock(node->access2())) {\
singleStatementBlocks.push_back(node->access2());\
}}
    
    BLOCKIFY_ONE_BODY(SgWhileStmt, get_body)
    BLOCKIFY_ONE_BODY(SgDoWhileStmt, get_body)
    BLOCKIFY_ONE_BODY(SgForAllStatement, get_body)
    BLOCKIFY_ONE_BODY(SgForStatement, get_loop_body)
    BLOCKIFY_ONE_BODY(SgSwitchStatement, get_body)
    BLOCKIFY_ONE_BODY(SgCaseOptionStmt, get_body)
    BLOCKIFY_ONE_BODY(SgDefaultOptionStmt, get_body)
    BLOCKIFY_ONE_BODY(SgTryStmt, get_body)
    BLOCKIFY_ONE_BODY(SgCatchOptionStmt, get_body)
    BLOCKIFY_TWO_BODY(SgIfStmt, get_true_body, get_false_body)
};

SingleStatementToBlockNormalizer::SingleStatementToBlockNormalizer(){
    singleStatementToBlock = new SingleStatementToBlockVisitor();
}
SingleStatementToBlockNormalizer::~SingleStatementToBlockNormalizer(){
    delete singleStatementToBlock;
}
void SingleStatementToBlockNormalizer::Normalize(SgNode * node) {
    traverse(node, postorder);
    SingleStatementToBlockVisitor * visitor = dynamic_cast<SingleStatementToBlockVisitor*>(singleStatementToBlock);
    ROSE_ASSERT(visitor);
    visitor->Normalize();
}
void SingleStatementToBlockNormalizer::NormalizeWithinFile(SgNode * node) {
    traverseWithinFile(node, postorder);
    SingleStatementToBlockVisitor * visitor = dynamic_cast<SingleStatementToBlockVisitor*>(singleStatementToBlock);
    ROSE_ASSERT(visitor);
    visitor->Normalize();
}
void SingleStatementToBlockNormalizer::NormalizeInputFiles(SgProject * project) {
    traverse(project, postorder);
    SingleStatementToBlockVisitor * visitor = dynamic_cast<SingleStatementToBlockVisitor*>(singleStatementToBlock);
    ROSE_ASSERT(visitor);
    visitor->Normalize();
}
void SingleStatementToBlockNormalizer::visit(SgNode * node) {
    node->accept(*singleStatementToBlock);
}
