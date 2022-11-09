
#ifndef Rose_CodeGen_factory_functions_txx
#define Rose_CodeGen_factory_functions_txx

namespace Rose { namespace CodeGen {

template <typename CRT, typename API>
struct __factory_helper_t<CRT, API, Object::a_function> {
  static declaration_t<Object::a_function> * instantiate(
      Factory<CRT, API> const & factory,
      symbol_t<Object::a_function> * sym,
      SgNamedType * parent,
      tplargs_t & tpl_args
  );

  template <typename... Args>
  static declaration_t<Object::a_function> * instantiate(
      Factory<CRT, API> const & factory,
      symbol_t<Object::a_function> * sym,
      SgNamedType * parent,
      Args... args
  );

  template <typename... Args>
  static reference_t<Object::a_function> * reference(
      Factory<CRT, API> const & factory,
      symbol_t<Object::a_function> * sym,
      SgNamedType * parent,
      Args... args
  );
};

#define DEBUG___factory_helper_t__a_function__instantiate 0

template <typename CRT, typename API>
declaration_t<Object::a_function> * __factory_helper_t<CRT, API, Object::a_function>::instantiate(
  Factory<CRT, API> const & factory,
  symbol_t<Object::a_function> * sym,
  SgNamedType * parent,
  tplargs_t & tpl_args
) {
#if DEBUG___factory_helper_t__a_function__instantiate
  std::cout << "__factory_helper_t<CRT, API, Object::a_function>::instantiate" << std::endl;
  std::cout << "  sym    = " << std::hex << sym << " : " << ( sym ? sym->class_name() : "" ) << std::endl;
  std::cout << "  sym->get_declaration()    = " << std::hex << sym->get_declaration() << " : " << ( sym->get_declaration() ? sym->get_declaration()->class_name() : "" ) << std::endl;
#endif

  ROSE_ABORT(); // TODO
  return nullptr;
}

template <typename CRT, typename API>
template <typename... Args>
declaration_t<Object::a_function> * __factory_helper_t<CRT, API, Object::a_function>::instantiate(
  Factory<CRT, API> const & factory,
  symbol_t<Object::a_function> * sym,
  SgNamedType * parent,
  Args... args
) {
  std::vector<SgTemplateArgument *> tpl_args;
  Rose::Builder::Templates::fillTemplateArgumentList(tpl_args, args...);
  return instantiate(factory, sym, parent, tpl_args);
}

template <typename CRT, typename API>
template <typename... Args>
reference_t<Object::a_function> * __factory_helper_t<CRT, API, Object::a_function>::reference(
  Factory<CRT, API> const & factory,
  symbol_t<Object::a_function> * sym,
  SgNamedType * parent,
  Args... args
) {
  ROSE_ASSERT(!parent); // TODO case of a field or method
  return SageBuilder::buildFunctionRefExp(sym);
}

} }

#endif /* Rose_CodeGen_factory_functions_txx */

