
#ifndef Rose_Source_CodeGen_factory_typedefs_txx
#define Rose_Source_CodeGen_factory_typedefs_txx

namespace Rose { namespace Source { namespace CodeGen {

template <typename CRT, typename API>
struct __factory_helper_t<CRT, API, Object::a_typedef> {
  static declaration_t<Object::a_typedef> * instantiate(
      Factory<CRT, API> const & factory,
      symbol_t<Object::a_typedef> * sym,
      SgNamedType * parent,
      tplargs_t & tpl_args
  );

  template <typename... Args>
  static declaration_t<Object::a_typedef> * instantiate(
      Factory<CRT, API> const & factory,
      symbol_t<Object::a_typedef> * sym,
      SgNamedType * parent,
      Args... args
  );

  template <typename... Args>
  static reference_t<Object::a_typedef> * reference(
      Factory<CRT, API> const & factory,
      symbol_t<Object::a_typedef> * sym,
      SgNamedType * parent,
      Args... args
  );
};

#define DEBUG___factory_helper_t__a_typedef__instantiate 0

template <typename CRT, typename API>
declaration_t<Object::a_typedef> * __factory_helper_t<CRT, API, Object::a_typedef>::instantiate(
  Factory<CRT, API> const & /*factory*/,
  symbol_t<Object::a_typedef> * sym,
  SgNamedType * /*parent*/,
  tplargs_t & tpl_args
) {
#if DEBUG___factory_helper_t__a_typedef__instantiate
  std::cout << "__factory_helper_t<CRT, API, Object::a_typedef>::instantiate" << std::endl;
  std::cout << "  sym             = " << std::hex << sym << " : " << ( sym ? sym->class_name() : "" ) << std::endl;
  std::cout << "  tpl_args.size() = " << std::dec << tpl_args.size() << std::endl;
#endif

  SgName fname(sym->get_name().getString());
  SgName fname_tplargs = SageBuilder::appendTemplateArgumentsToName(fname, tpl_args);
  if (tpl_args.size() == 0) {
    fname_tplargs += "<>";
  }

  SgTemplateTypedefDeclaration * tpl_decl = isSgTemplateTypedefDeclaration(sym->get_declaration());
  ROSE_ASSERT(tpl_decl);
#if DEBUG___factory_helper_t__a_typedef__instantiate
  std::cout << "  tpl_decl    = " << std::hex << tpl_decl << " : " << ( tpl_decl ? tpl_decl->class_name() : "" ) << std::endl;
#endif

  SgType * base_type = Rose::Builder::Templates::instantiateNonrealTypes(tpl_decl->get_base_type(), tpl_decl->get_templateParameters(), tpl_args);
#if DEBUG___factory_helper_t__a_typedef__instantiate
  std::cout << "  base_type    = " << std::hex << base_type << " : " << ( base_type ? base_type->class_name() : "" ) << std::endl;
#endif

  SgScopeStatement * defn_scope = tpl_decl->get_scope();
  
  SgTemplateInstantiationTypedefDeclaration * tddecl = new SgTemplateInstantiationTypedefDeclaration(
      fname_tplargs, base_type, nullptr,        nullptr,                 nullptr,    tpl_decl,                      tpl_args
  );
  ROSE_ASSERT(tddecl != nullptr);
#if DEBUG___factory_helper_t__a_typedef__instantiate
  std::cout << "  tddecl    = " << std::hex << tddecl << " : " << ( tddecl ? tddecl->class_name() : "" ) << std::endl;
  std::cout << "    ->get_base_type()    = " << std::hex << tddecl->get_base_type() << " : " << ( tddecl->get_base_type() ? tddecl->get_base_type()->class_name() : "" ) << std::endl;
#endif
  tddecl->set_parent(defn_scope);
  tddecl->set_templateName(fname);
  tddecl->set_scope(defn_scope);
  defn_scope->insert_symbol(fname_tplargs, new SgTypedefSymbol(tddecl));
  tddecl->set_firstNondefiningDeclaration(tddecl);

  tddecl->set_type(SgTypedefType::createType(tddecl));

  for (auto tpl_arg: tpl_args) {
    tpl_arg->set_parent(tddecl);
  }

  return tddecl;
}

template <typename CRT, typename API>
template <typename... Args>
declaration_t<Object::a_typedef> * __factory_helper_t<CRT, API, Object::a_typedef>::instantiate(
  Factory<CRT, API> const & factory,
  symbol_t<Object::a_typedef> * sym,
  SgNamedType * parent,
  Args... args
) {
  std::vector<SgTemplateArgument *> tpl_args;
  Rose::Builder::Templates::fillTemplateArgumentList(tpl_args, args...);
  return instantiate(factory, sym, parent, tpl_args);
}

#define DEBUG___factory_helper_t__a_typedef__reference 0

template <typename CRT, typename API>
template <typename... Args>
reference_t<Object::a_typedef> * __factory_helper_t<CRT, API, Object::a_typedef>::reference(
  Factory<CRT, API> const & /*factory*/,
  symbol_t<Object::a_typedef> * sym,
  SgNamedType * parent,
  Args... /*args*/
) {
#if DEBUG___factory_helper_t__a_typedef__reference
  std::cout << "__factory_helper_t<CRT, API, Object::a_typedef>::reference" << std::endl;
  std::cout << "  sym    = " << std::hex << sym << " : " << ( sym ? sym->class_name() : "" ) << std::endl;
#endif
  ROSE_ASSERT(!parent); // TODO case of a field or method

  reference_t<Object::a_typedef> * res = sym->get_declaration()->get_type();
#if DEBUG___factory_helper_t__a_typedef__reference
  std::cout << "  res    = " << std::hex << res << " : " << ( res ? res->class_name() : "" ) << std::endl;
#endif
  return res;
}

} } }

#endif /* Rose_Source_CodeGen_factory_typedefs_txx */

