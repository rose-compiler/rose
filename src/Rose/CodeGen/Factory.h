
#ifndef ROSE_CODEGEN_FACTORY_H
#define ROSE_CODEGEN_FACTORY_H

#include "Rose/CodeGen/API.h"

namespace Rose { namespace CodeGen {

/**
 * \brief 
 */
template <typename CRT, typename API>
class Factory;

/**
 * \brief 
 */
template <typename CRT, typename apiT, Object otag>
struct __factory_helper_t;

template <typename CRT, typename API>
class Factory {
  public:
    using factory_t = Factory<CRT, API>;

  public:
    Driver & driver;
    API api;

  public:
    Factory(Driver & driver_) : driver(driver_), api() {
      api.load(driver);
    }
    /**
     * Return an instantiation 
     *
     * \param otag must be one of a_class/a_typedef/a_variable/a_function
     * \param obj points to a template symbol
     * \param args template arguments
     * \param parent is used when called by reference which was called by one of the `access` methods
     *
     * \return a template instantiation
     *
     */
    template <Object otag, typename... Args>
    declaration_t<otag> * instantiate(symbol_t<otag> * API::* obj, Args... args, SgClassType * parent=nullptr) {
      symbol_t<otag> * sym = api.*obj;
      ROSE_ASSERT(sym);
      ROSE_ASSERT(is_template_symbol_variant<otag>(sym->variantT()));
      return __factory_helper_t<CRT, API, otag>::instantiate(*this, sym, parent, args...);
    }

    /**
     * Return an expression or type referencing the object. Args are forwarded to `instantiate` as needed.
     *
     * \param otag must be one of a_class/a_typedef/a_variable/a_function
     * \param obj points to a symbol
     * \param args template arguments if obj is a template
     * \param parent is used when called by one of the `access` methods
     *
     * \return a reference expression
     *
     */
    template <Object otag, typename... Args>
    reference_t<otag> * reference(symbol_t<otag> * API::* obj, Args... args, SgClassType * parent=nullptr) {
      symbol_t<otag> * sym = api.*obj;
      ROSE_ASSERT(sym);
      
      if (is_template_symbol_variant<otag>(sym->variantT())) {
        declaration_t<otag> * decl = instantiate<otag>(obj, args..., parent);
        ROSE_ASSERT(decl != nullptr);

        ROSE_ABORT(); // TODO get symbol from decl
      } else {
        // FIXME sanity-check: `args` is empty
      }

      return __factory_helper_t<CRT, API, otag>::reference(*this, sym, parent, args...);
    }

    /**
     * Build expression to access a member of the parent expression. Args are forwarded to `instantiate` as needed.
     *
     * \param otag must be one of a_variable/a_function
     * \param obj points to a symbol
     * \param parent
     * \param args template arguments if obj is a template
     *
     * \return either `.` or `->` operator with `parent` as lhs and `reference(obj, args...)` as rhs
     *
     */
    template <Object otag, typename... Args>
    SgExpression * access(symbol_t<otag> * API::* obj, SgExpression * parent, Args... args) {

      SgType * ptype = parent->get_type(); // TODO strip type modifiers and references

      bool lhs_has_ptr_type = isSgPointerType(parent->get_type());
      if (lhs_has_ptr_type) {
        ptype = ((SgPointerType*)ptype)->get_base_type(); // TODO strip type modifiers and references
      }

      SgClassType * xtype = isSgClassType(ptype);
      ROSE_ASSERT(xtype != nullptr);

      reference_t<otag> * rhs = reference(obj, args..., ptype);
      ROSE_ASSERT(rhs != nullptr);

      if (lhs_has_ptr_type) {
        return SageBuilder::buildArrowExp(parent, rhs);
      } else {
        return SageBuilder::buildDotExp(parent, rhs);
      }
    }

    /**
     * Build expression or type to access static member and subtype of the parent type. Args are forwarded to `instantiate` as needed.
     *
     * \param otag must be one of a_class/a_typedef or a_variable/a_function
     * \param obj points to a symbol
     * \param lhs
     * \param args template arguments if obj is a template
     *
     */
    template <Object otag, typename... Args, typename OutNodeT = std::conditional_t<otag == Object::a_class || otag == Object::a_typedef, SgType, SgExpression>>
    OutNodeT * access(symbol_t<otag> * API::* obj, SgNamedType * parent, Args... args) {
      reference_t<otag> * rhs = reference(obj, parent, args...);
      // TODO build either SgScopedRefType SgScopedRefExp
      return rhs;
    }
};

#if 0
template <typename CRT, typename apiT>
template <>
reference_t<Object::a_variable> * Factory<CRT,apiT>::reference<Object::a_variable>(symbol_t<Object::a_variable> * sym) {
  return nullptr;
}
#endif

} }

#include "Rose/CodeGen/factory/namespaces.txx"
#include "Rose/CodeGen/factory/classes.txx"
#include "Rose/CodeGen/factory/typedefs.txx"
#include "Rose/CodeGen/factory/variables.txx"
#include "Rose/CodeGen/factory/functions.txx"

#endif /* ROSE_CODEGEN_FACTORY_H */

