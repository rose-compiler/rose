
#ifndef Rose_Source_Generation_factory_namespaces_txx
#define Rose_Source_Generation_factory_namespaces_txx

namespace Rose { namespace Source { namespace Generation {

template <typename CRT, typename API>
struct __factory_helper_t<CRT, API, Object::a_namespace> {
  template <typename... Args>
  static reference_t<Object::a_namespace> * reference(
      Factory<CRT, API> const & factory,
      symbol_t<Object::a_namespace> * sym,
      SgNamedType * parent,
      Args... args
  );
};

template <typename CRT, typename API>
template <typename... Args>
reference_t<Object::a_namespace> * __factory_helper_t<CRT, API, Object::a_namespace>::reference(
  Factory<CRT, API> const & /*factory*/,
  symbol_t<Object::a_namespace> * /*sym*/,
  SgNamedType * /*parent*/,
  Args... /*args*/
) {
  ROSE_ABORT(); // TODO
  return nullptr;
}

} } }

#endif /* Rose_Source_Generation_factory_namespaces_txx */

