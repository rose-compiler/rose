
#ifndef ROSE_CODEGEN_FACTORY_NAMESPACES_TXX
#define ROSE_CODEGEN_FACTORY_NAMESPACES_TXX

namespace Rose { namespace CodeGen {

template <typename CRT, typename API>
struct __factory_helper_t<CRT, API, Object::a_namespace> {
  template <typename... Args>
  static reference_t<Object::a_namespace> * reference(
      Factory<CRT, API> & factory,
      symbol_t<Object::a_namespace> * sym,
      SgClassType * parent,
      Args... args
  );
};

template <typename CRT, typename API>
template <typename... Args>
reference_t<Object::a_namespace> * __factory_helper_t<CRT, API, Object::a_namespace>::reference(
  Factory<CRT, API> & factory,
  symbol_t<Object::a_namespace> * sym,
  SgClassType * parent,
  Args... args
) {
  ROSE_ABORT(); // TODO
  return nullptr;
}

} }

#endif /*  ROSE_CODEGEN_FACTORY_NAMESPACES_TXX */

