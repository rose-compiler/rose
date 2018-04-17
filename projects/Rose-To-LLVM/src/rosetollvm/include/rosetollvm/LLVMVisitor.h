#ifndef LLVM_VISITOR
#define LLVM_VISITOR

#include <rosetollvm/Option.h>
#include <rosetollvm/Control.h>
#include <rosetollvm/LLVMAstAttributes.h>
#include <rose.h>

class LLVMVisitor : public AstPrePostProcessing {
public:
    LLVMVisitor(Option &option_, Control &control_) : option(option_),
                                                      control(control_),
                                                      visit_suspended_by_node(NULL)
    {}

protected:

    Option &option;
    Control &control;
    LLVMAstAttributes *attributes;
    CodeEmitter *codeOut;


    void setAttributes(LLVMAstAttributes *attributes_) {
        attributes = attributes_;
        codeOut = &(attributes -> getCodeOut());
    }

    /**
     * visit_suspended_by_node, initially NULL, is used to avoid visitation of the descendants of a node P.
     * To achieve this, the user sets visit_suspended_by_node to P when previsiting P. Once that's done, no
     * node Q such that Q != P will be visited as long as visit_suspended_by_node == P.  At the beginiing of
     * the post-visit function, when P is finally reached again, visit_suspended_by_node is reset to NULL,
     * allowing visitation to resume starting with a post-visitation of P itself. 
     *
     * This feature is important because there appears to be cases in Sage III where the Rose compiler
     * calculates the value of a constant expression at compile time, replaces the tree correspondingto the
     * constant expression by a Value node, but nonetheless, makes the original tree a subtree of the value
     * node in question. That subtree also gets traversed during a normal Pre/Post visit. Thus, to avoid
     * this problem, when we encounter a constant value (SgValueExp) during a visit, we instruct the visitor
     * to ignore all its descendants, if any.
     *
     * I suspect that the reason for this arrangement is to keep track of the original user-specification.
     * When in the course of semantic analysis a (AST) subtree has been resolved to another subtree, It would
     * have been desirable for Rose to allow the user to pass an argument to the visitor to instruct it as to
     * what to do when it encounters a "resolved" subtree. Namely, visit:
     *
     *  1. Original nodes only
     *  2. Resolved nodes only
     *  3. Both sets of nodes
     */
    SgNode *visit_suspended_by_node;
 
    /**
     * Implement the Rose PrePostOrder pre-visit here and have it use our local pre-Visitor interface
     */
    void preOrderVisit(SgNode *node) {
        if (preVisitEnter(node)) {
            preVisit(node);
            preVisitExit(node);
        }
    }

    /**
     * Implement the Rose PrePostOrder post-visit here and have it use our local post-Visitor interface
     */
    void postOrderVisit(SgNode *node) {
        if (postVisitEnter(node)) {
            postVisit(node);
            postVisitExit(node);
        }
    }

    /**
     * Split the pre-visit into three functions:  a boolean function preVisitEnter that
     * does some preprocessing and returns true if the visitor is to continue processing
     * it (and its descendant) and false otherwise; a preVisit function that actually
     * visits the node; a postVisitExit function that does some final processing after
     * the visit.
     */
    virtual bool preVisitEnter(SgNode *node) = 0;
    virtual void preVisit(SgNode *node) = 0;
    virtual void preVisitExit(SgNode *node) = 0;

    /**
     * Same as above for post visit
     */
    virtual bool postVisitEnter(SgNode *node) = 0;
    virtual void postVisit(SgNode *node) = 0;
    virtual void postVisitExit(SgNode *node) = 0;
};

#endif
