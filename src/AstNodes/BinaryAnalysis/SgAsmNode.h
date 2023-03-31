/** Base class for all binary analysis IR nodes. */
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
};
