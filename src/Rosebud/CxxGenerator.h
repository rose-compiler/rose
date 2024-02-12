#ifndef Rosebud_CxxGenerator_H
#define Rosebud_CxxGenerator_H
#include <Rosebud/Generator.h>

namespace Rosebud {

/** Base class for generators that produce C++ code. */
class CxxGenerator: public Generator {
protected:
    size_t outputWidth = 130;                           // nominal width of the generated code

protected:
    /** Return a title comment that says the file is machine generated. */
    virtual std::string machineGenerated(char commentType = '/');

    /** Expression for initializing a property in a constructor, or empty.
     *
     *  For most properties, this is simply the initialization expression parsed from the input. However, some properties
     *  need extra arguments, etc. that can be returned by this funciton. For instance, a TreeEdge property is initialized
     *  in a special way:
     *
     * @code
     *  // Class with property definition
     *  class Node: public TreeNode {
     *      [[Rosebud::property]]
     *      TreeEdge<Foo> foo = Foo::instance();
     *
     *      [[Rosebud::property]]
     *      int bar = 42;
     *  };
     *
     *  // Generated default constructor
     *  Node::Node()
     *      : foo_(*this, Foo::instance()), bar_(42) {}
     * @endcode
     *
     * So the initializer expression for the "foo" property is overridden to add the "*this" argument, but for "bar" it is not
     * overridden.
     *
     * The @p expr argument is the default initializer expression such as the string from the right hand side of the "=" in the
     * property definition, or some other expression. */
    virtual std::string ctorInitializerExpression(const Ast::PropertyPtr&, const std::string &expr);

    /** Statement to initialize a property.
     *
     *  The return string is a statement that initializes the property data member if the data member needs to be initialized other
     *  than with a default constructor. This is used in the generated "initializeProperties" member function which is called by
     *  user-defined constructors prior to the user assigning a different value. The returned string is either empty or includes
     *  the terminating semicolon. */
    virtual std::string resetStatement(const Ast::PropertyPtr&);

    /** Type for the data member for a property.
     *
     *  The base implementation simply returns the property type from the input. */
    virtual std::string propertyDataMemberType(const Ast::PropertyPtr&);

    /** Type for the return value for a property accessor.
     *
     *  This is usually the same as the property data member type but without any 'mutable'. Do not add reference or 'const'. */
    virtual std::string propertyAccessorReturnType(const Ast::PropertyPtr&);

    /** Type for the argument for a property mutator.
     *
     *  If the property data member type is `T`, then this is usually `const T&`. */
    virtual std::string propertyMutatorArgumentType(const Ast::PropertyPtr&);

    /** Initial value expression for the property.
     *
     *  The base implementation returns whatever expression was to the right of the "=" in the property definition. */
    virtual std::string initialValue(const Ast::PropertyPtr&);

    /** Type of value for initializing a property.
     *
     *  This is the type used by mutator arguments, but without the const reference part. The base implementation simply returns the
     *  type from the property definition after removing @c volatile and @c mutable, but sometimes we need something else. For
     *  instance, a property of type @c TreeEdge<T> is never initialized from another tree edge, but rather from a @c
     *  std::shared_ptr<T>, as follows:
     *
     * @code
     *  // Class with property definition
     *  class Node: public TreeNode {
     *      [[Rosebud::property]]
     *      TreeEdge<Foo> foo;
     *  };
     *
     *  // Generated mutator member function declaration
     *  void foo(std::shared_ptr<Foo> const&);
     * @endcode */
    virtual std::string valueType(const Ast::PropertyPtr&);

    /** Emit code for the class destructor. */
    virtual void genDestructor(std::ostream &header, std::ostream &impl, const Ast::ClassPtr&);

    /** Emit code for the class default constructor. */
    virtual void genDefaultConstructor(std::ostream &header, std::ostream &impl, const Ast::ClassPtr&, Access);

    /** Emit code for the constructor with ctor_args property arguments.
     *
     *  Returns true if it generated a constructor. */
    virtual bool genArgsConstructor(std::ostream &header, std::ostream &impl, const Ast::ClassPtr&, const Hierarchy&, Access);

    /** Emit code for the constructor body. */
    virtual void genConstructorBody(std::ostream&, const Ast::ClassPtr&);

    /** Emit code that initializes all local properties. */
    virtual void genInitProperties(std::ostream &header, std::ostream &impl, const Ast::ClassPtr&);

    /** Given a type name, remove the outer pointer if present.
     *
     *  @li Given "Thing*" return "Thing"
     *  @li Given "ThingPtr" return "Thing"
     *  @li Given "ThingConstPtr" return "Thing"
     *  @li Given "Thing::Ptr" return "Thing"
     *  @li Given "Thing::ConstPtr" return "Thing"
     *  @li Given "Thing" (not a pointer) return "Thing" */
    virtual std::string removePointer(const std::string&) const;
};

} // namespace
#endif
