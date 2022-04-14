
#ifndef ROSE_CODEGEN_FACTORY_TYPEDEFS_TXX
#define ROSE_CODEGEN_FACTORY_TYPEDEFS_TXX

namespace Rose { namespace CodeGen {

template <typename CRT, typename API>
struct __factory_helper_t<CRT, API, Object::a_typedef> {
  template <typename... Args>
  static declaration_t<Object::a_typedef> * instantiate(
      Factory<CRT, API> & factory,
      symbol_t<Object::a_typedef> * sym,
      SgClassType * parent,
      Args... args
  );

  template <typename... Args>
  static reference_t<Object::a_typedef> * reference(
      Factory<CRT, API> & factory,
      symbol_t<Object::a_typedef> * sym,
      SgClassType * parent,
      Args... args
  );
};

template <typename CRT, typename API>
template <typename... Args>
declaration_t<Object::a_typedef> * __factory_helper_t<CRT, API, Object::a_typedef>::instantiate(
  Factory<CRT, API> & factory,
  symbol_t<Object::a_typedef> * sym,
  SgClassType * parent,
  Args... args
) {
  ROSE_ABORT(); // TODO
  return nullptr;
}

template <typename CRT, typename API>
template <typename... Args>
reference_t<Object::a_typedef> * __factory_helper_t<CRT, API, Object::a_typedef>::reference(
  Factory<CRT, API> & factory,
  symbol_t<Object::a_typedef> * sym,
  SgClassType * parent,
  Args... args
) {
  ROSE_ABORT(); // TODO
  return nullptr;
}

} }

#endif /*  ROSE_CODEGEN_FACTORY_TYPEDEFS_TXX */

