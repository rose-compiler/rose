#ifndef REVERSE_COMPUTATION_NORMALIZATION
#define REVERSE_COMPUTATION_NORMALIZATION

#include <rose.h>

// Normalize an event function.
void normalizeEvent(SgFunctionDefinition* func_def);

bool isAssignmentOp(SgExpression* e);

// Transform a modifying expression into several ones contained in a comma 
// operator expression. The transformations are:
//
//    a = b  ==>  a = b, a
//    --a  ==>  --a, a
//    a--  ==>  t = a, --a, t
//    a && b  ==>  t = a, t && t = b, t
//    a || b  ==>  t = a, t || t = b, t
//
void getAndReplaceModifyingExpression(SgExpression*& exp);

SgExpression* normalizeExpression(SgExpression* exp);

// Split a comma expression into several statements.
void splitCommaOpExp(SgExpression* exp);

// Propagate comma operation expressions, e.g. (a, b) + c ==> a, (b + c).
SgExpression* propagateCommaOpExp(SgExpression* exp);

// Propagate conditional operation expressions, 
// e.g. (a ? b : c) = d ==> a ? (b = d) : (c = d).
SgExpression* propagateConditionalExp(SgExpression* exp);

inline SgExpression* propagateCommaOpAndConditionalExp(SgExpression* exp)
{
    exp = propagateCommaOpExp(exp);
    return propagateConditionalExp(exp);
}

// Remove braces of a basic block in which there is no variable declared.
void removeUselessBraces(SgNode* root);

// The following function should be put in utilities.h
template <class T>
std::vector<T*> querySubTree(SgNode* root, t_traverseOrder order = postorder)
{
    struct Traversal : public AstSimpleProcessing
    {
        std::vector<T*> all_nodes;
        virtual void visit(SgNode* n)
        {
            T* node = dynamic_cast<T*>(n);
            if (node) all_nodes.push_back(node);
        }
    }; 

    Traversal traversal;
    traversal.traverse(root, order);
    return traversal.all_nodes;
}

#endif
