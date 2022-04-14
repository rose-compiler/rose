
#ifndef ROSE_CODEGEN_FACTORY_VARIABLES_TXX
#define ROSE_CODEGEN_FACTORY_VARIABLES_TXX

namespace Rose { namespace CodeGen {

template <typename CRT, typename API>
struct __factory_helper_t<CRT, API, Object::a_variable> {
  template <typename... Args>
  static declaration_t<Object::a_variable> * instantiate(
      Factory<CRT, API> & factory,
      symbol_t<Object::a_variable> * sym,
      SgClassType * parent,
      Args... args
  );

  template <typename... Args>
  static reference_t<Object::a_variable> * reference(
      Factory<CRT, API> & factory,
      symbol_t<Object::a_variable> * sym,
      SgClassType * parent,
      Args... args
  );
};

template <typename CRT, typename API>
template <typename... Args>
declaration_t<Object::a_variable> * __factory_helper_t<CRT, API, Object::a_variable>::instantiate(
  Factory<CRT, API> & factory,
  symbol_t<Object::a_variable> * sym,
  SgClassType * parent,
  Args... args
) {
  ROSE_ABORT(); // TODO
  return nullptr;
}

template <typename CRT, typename API>
template <typename... Args>
reference_t<Object::a_variable> * __factory_helper_t<CRT, API, Object::a_variable>::reference(
  Factory<CRT, API> & factory,
  symbol_t<Object::a_variable> * sym,
  SgClassType * parent,
  Args... args
) {
  ROSE_ASSERT(!parent); // TODO case of a field or method
  return SageBuilder::buildVarRefExp(sym);
}

} }

#endif /*  ROSE_CODEGEN_FACTORY_VARIABLES_TXX */

