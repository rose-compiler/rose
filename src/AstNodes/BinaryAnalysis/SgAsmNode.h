#include <boost/format.hpp>

/** Base class for all binary analysis IR nodes. */
[[Rosebud::abstract]]
class SgAsmNode: public SgNode {
protected:
    // Rosebud generates call to destructorHelper in every destructor implementation. This gives the user a chance to do something
    // special. Any implementations should be prepared to handle multiple invocations on the same object, and should not access any
    // data members in any derived class. Standard C++ rules about calling virtual functions in destructors apply here.
    virtual void destructorHelper() {}

public:
    /** Allocate and return a new node after setting its parent.
     *
     *  ROSE's AST has this annoying, error-prone process for adding a node to the tree. One has to not only add the downward edge
     *  from parent to child, but also explicitly add the upward edge from child to parent. This two-step process makes it difficult
     *  to create child nodes and link them into the parent while the parent is being constructed. This function attempts to make
     *  this easier by doing everything in one step.
     *
     *  Use it like this:
     *
     * @code
     *  Parent::Parent(int foo, int bar)
     *      : p_child(createAndParent<Child>(this), p_foo(foo), p_bar(bar) {}
     * @endcode */
    template<class T>
    static typename std::enable_if<std::is_base_of<SgNode, T>::value, T*>::type
    createAndParent(SgNode *parent) {
        auto node = new T;
        node->set_parent(parent);
        return node;
    }

    /** Set a child edge in a tree to point to a specific child.
     *
     *  Normally one has to remember to set the parent pointer when adding a child to a tree, and also be careful that the result is
     *  still a tree. This helper function tries to make that easier by checking some things and setting the parent pointer
     *  automatically.
     *
     *  For instance, this should work:
     *
     * @code
     *  SgAsmBinaryExpression *parent = ...;
     *  SgAsmBinaryExpression *child1 = ...;
     *  SgAsmBinaryExpression *child2 = ...;
     *
     *  parent->set_lhs(child1);
     *  ASSERT_require(child1->get_parent() == parent);
     *
     *  parent->set_lhs(child2);
     *  ASSERT_require(child1->get_parent() == nullptr);
     *  ASSERT_require(child2->get_parent() == parent);
     *
     *  parent->set_lhs(nullptr);
     *  ASSERT_require(child2->get_parent() == nullptr);
     * @endcode
     *
     *  The following should not work when assertions are enabled:
     *
     * @code
     *  SgAsmBinaryExpression *parent = ...;
     *  SgAsmBinaryExpression *child1 = ...;
     *  SgAsmBinaryExpression *child2 = ...;
     *
     *  parent->set_lhs(child1);
     *  parent->set_rhs(child1); // no longer a tree, but still acyclic
     * @endcode
     *
     * @{ */
    template<class T>
    typename std::enable_if<
        std::is_pointer<T>::value,
        void>::type
    changeChildPointer(T& edge, T const& child) {
        if (child != edge) {
            // If there is an old child, check that it has the correct parent and then remove it.
            if (edge) {
                ASSERT_require2(edge->get_parent() == this,
                                (boost::format("node %p is a child of %p but has wrong parent %p")
                                 % edge % this % edge->get_parent()).str());
                edge->set_parent(nullptr);
                edge = nullptr;
            }

            // If there is a new child, check its parent pointer and then insert it.
            if (child) {
                ASSERT_require2(child->get_parent() == nullptr,
                                (boost::format("node %p is to be a child of %p but is already a child of %p")
                                 % child % this % child->get_parent()).str());
                child->set_parent(this);
                edge = child;
            }
        }
    }

    // This gets called for non-pointers, which is the situation for nodes that are only lists of other nodes. We're currently
    // not checking them because the ROSETTA-generated API has too many ways to get around this check, most of which make it
    // impossible to enforce constraints regarding the parent/child consistency. For example:
    //
    //    node->children().clear(); // removes a whole bunch of children but doesn't clear their parent pointers
    //    node->children()[i] = child; // changes the ith child, but doesn't clear the previous ith child's parent pointer
    //    for (auto &tmp: node->children()) tmp = nullptr; // changes each child but doesn't clear parent pointers
    //    ad nausiam...
    template<class T>
    typename std::enable_if<
        !std::is_pointer<T>::value,
        void>::type
    changeChildPointer(T& edge, T const& child) {
        edge = child;
    }
    /** @} */
};
