#ifndef ROSE_CodeGen_Factory_H
#define ROSE_CodeGen_Factory_H

#include <Rose/CodeGen/API.h>

namespace Rose { namespace CodeGen {

/**
 * \brief constructs expressions and types for the given API
 *
 * \tparam CRT https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
 * \tparam API a specialization of Rose::CodeGen::API
 *
 * 
 *
 */
template <typename CRT, typename API>
class Factory;

/**
 * \brief enables partial specializations w.r.t the template parameter `otag`
 *
 * \tparam CRT https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
 * \tparam API a specialization of Rose::CodeGen::API
 * \tparam otag Object of the specialization
 *
 */
template <typename CRT, typename apiT, Object otag>
struct __factory_helper_t;

using tplargs_t = std::vector<SgTemplateArgument *>;

template <typename CRT, typename API>
class Factory {
  public:
    using factory_t = Factory<CRT, API>;

  public:
    Driver & driver;
    API api;

  public:
    Factory(Driver & driver_);

    /**
     * Return an instantiation 
     *
     * \tparam otag must be one of a_class/a_typedef/a_variable/a_function
     * \tparam Args types of the nodes to use as template arguments (SgType/SgExpression for type/non-type respectively
     *
     * \param obj must points to a template symbol
     * \param args nodes to use as template arguments
     * \param parent is used when called by reference which was called by one of the `access` methods
     *
     * \return a template instantiation
     *
     */
    template <Object otag, typename... Args>
    declaration_t<otag> * instantiate(symbol_t<otag> * API::* obj, SgNamedType * parent, Args... args) const;

    /**
     * Return an expression or type referencing the object. Args are forwarded to `instantiate` as needed.
     *
     * \tparam otag must be one of a_class/a_typedef/a_variable/a_function
     * \tparam Args types of template arguments (see instantiate)
     *
     * \param obj points to a symbol
     * \param args template arguments (see instantiate)
     * \param parent is used when called by one of the `access` methods
     *
     * \return a reference expression
     *
     */
    template <Object otag, typename... Args>
    reference_t<otag> * reference(symbol_t<otag> * API::* obj, SgNamedType * parent, Args... args) const;

    /**
     * Build expression to access a member of the parent expression. Args are forwarded to `instantiate` as needed.
     *
     * \tparam otag must be one of a_variable/a_function
     * \tparam Args types of template arguments (see instantiate)
     *
     * \param obj points to a symbol
     * \param parent
     * \param args template arguments (see instantiate)
     *
     * \return either `.` or `->` operator with `parent` as lhs and `reference(obj, args...)` as rhs
     *
     */
    template <Object otag, typename... Args>
    SgExpression * access(symbol_t<otag> * API::* obj, SgExpression * parent, Args... args) const;


  protected:
    /**
     * Select return type for access based on Object type
     *
     * \tparam otag must be one of a_class/a_typedef or a_variable/a_function
     *
     */
    template <Object otag>
    using access_return_t = std::conditional_t<otag == Object::a_class || otag == Object::a_typedef, SgType, SgExpression>;

    template <Object otag, std::enable_if_t<otag == Object::a_class || otag == Object::a_typedef> * = nullptr>
    static SgScopedType * build_scoped_ref(SgNamedType * lhs, reference_t<otag> * rhs);

    template <Object otag, std::enable_if_t<otag != Object::a_class && otag != Object::a_typedef> * = nullptr>
    static SgScopedRefExp * build_scoped_ref(SgNamedType * lhs, reference_t<otag> * rhs);

  public:
    /**
     * Build expression or type to access static member and subtype of the parent type. Args are forwarded to `instantiate` as needed.
     *
     * @c otag must be one of a_class/a_typedef or a_variable/a_function
     * @c Args types of template arguments (see instantiate)
     *
     * @c obj points to a symbol
     * @c lhs
     * @c args template arguments (see instantiate)
     *
     */
    template <Object otag, typename... Args>
    access_return_t<otag> * access(symbol_t<otag> * API::* obj, SgNamedType * parent, Args... args) const;
};

} }

#include "Rose/CodeGen/factory/factory.txx"
#include "Rose/CodeGen/factory/namespaces.txx"
#include "Rose/CodeGen/factory/classes.txx"
#include "Rose/CodeGen/factory/typedefs.txx"
#include "Rose/CodeGen/factory/variables.txx"
#include "Rose/CodeGen/factory/functions.txx"

#endif
