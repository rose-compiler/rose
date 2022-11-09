
#ifndef Rose_CodeGen_factory_classes_txx
#define Rose_CodeGen_factory_classes_txx

namespace Rose { namespace CodeGen {

template <typename CRT, typename API>
struct __factory_helper_t<CRT, API, Object::a_class> {
  static declaration_t<Object::a_class> * instantiate(
      Factory<CRT, API> const & factory,
      symbol_t<Object::a_class> * sym,
      SgNamedType * parent,
      tplargs_t & tpl_args
  );

  template <typename... Args>
  static declaration_t<Object::a_class> * instantiate(
      Factory<CRT, API> const & factory,
      symbol_t<Object::a_class> * sym,
      SgNamedType * parent,
      Args... args
  );

  template <typename... Args>
  static reference_t<Object::a_class> * reference(
      Factory<CRT, API> const & factory,
      symbol_t<Object::a_class> * sym,
      SgNamedType * parent,
      Args... args
  );
};

#define DEBUG___factory_helper_t__a_class__instantiate 0

template <typename CRT, typename API>
declaration_t<Object::a_class> * __factory_helper_t<CRT, API, Object::a_class>::instantiate(
  Factory<CRT, API> const & factory,
  symbol_t<Object::a_class> * sym,
  SgNamedType * parent,
  tplargs_t & tpl_args
) {
#if DEBUG___factory_helper_t__a_class__instantiate
  std::cout << "__factory_helper_t<CRT, API, Object::a_class>::instantiate" << std::endl;
  std::cout << "  sym    = " << std::hex << sym << " : " << ( sym ? sym->class_name() : "" ) << std::endl;
#endif

  ROSE_ABORT(); // TODO
  return nullptr;
}

template <typename CRT, typename API>
template <typename... Args>
declaration_t<Object::a_class> * __factory_helper_t<CRT, API, Object::a_class>::instantiate(
  Factory<CRT, API> const & factory,
  symbol_t<Object::a_class> * sym,
  SgNamedType * parent,
  Args... args
) {
  std::vector<SgTemplateArgument *> tpl_args;
  Rose::Builder::Templates::fillTemplateArgumentList(tpl_args, args...);
  return instantiate(factory, sym, parent, tpl_args);
}

template <typename CRT, typename API>
template <typename... Args>
reference_t<Object::a_class> * __factory_helper_t<CRT, API, Object::a_class>::reference(
  Factory<CRT, API> const & factory,
  symbol_t<Object::a_class> * sym,
  SgNamedType * parent,
  Args... args
) {
  ROSE_ASSERT(!parent); // TODO case of a field or method
  return sym->get_declaration()->get_type();
}

} }

#endif /* Rose_CodeGen_factory_classes_txx */

