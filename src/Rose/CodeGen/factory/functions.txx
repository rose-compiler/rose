
#ifndef Rose_CodeGen_factory_functions_txx
#define Rose_CodeGen_factory_functions_txx

namespace Rose { namespace CodeGen {

template <typename CRT, typename API>
struct __factory_helper_t<CRT, API, Object::a_function> {
  template <typename... Args>
  static declaration_t<Object::a_function> * instantiate(
      Factory<CRT, API> & factory,
      symbol_t<Object::a_function> * sym,
      SgNamedType * parent,
      Args... args
  );

  template <typename... Args>
  static reference_t<Object::a_function> * reference(
      Factory<CRT, API> & factory,
      symbol_t<Object::a_function> * sym,
      SgNamedType * parent,
      Args... args
  );
};

template <typename CRT, typename API>
template <typename... Args>
declaration_t<Object::a_function> * __factory_helper_t<CRT, API, Object::a_function>::instantiate(
  Factory<CRT, API> & factory,
  symbol_t<Object::a_function> * sym,
  SgNamedType * parent,
  Args... args
) {
  ROSE_ABORT(); // TODO
  return nullptr;
}

template <typename CRT, typename API>
template <typename... Args>
reference_t<Object::a_function> * __factory_helper_t<CRT, API, Object::a_function>::reference(
  Factory<CRT, API> & factory,
  symbol_t<Object::a_function> * sym,
  SgNamedType * parent,
  Args... args
) {
  ROSE_ABORT(); // TODO
  return nullptr;
}

} }

#endif /* Rose_CodeGen_factory_functions_txx */

