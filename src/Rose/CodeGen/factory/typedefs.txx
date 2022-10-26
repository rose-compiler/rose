
#ifndef Rose_CodeGen_factory_typedefs_txx
#define Rose_CodeGen_factory_typedefs_txx

namespace Rose { namespace CodeGen {

template <typename CRT, typename API>
struct __factory_helper_t<CRT, API, Object::a_typedef> {
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

template <typename CRT, typename API>
template <typename... Args>
declaration_t<Object::a_typedef> * __factory_helper_t<CRT, API, Object::a_typedef>::instantiate(
  Factory<CRT, API> const & factory,
  symbol_t<Object::a_typedef> * sym,
  SgNamedType * parent,
  Args... args
) {
  std::string tpl_args_str = Rose::Builder::Templates::strTemplateArgumentList(args...);
  // TODO lookup (sym,tpl_args_str) in cache

  std::vector<SgTemplateArgument *> tpl_args;
  Rose::Builder::Templates::fillTemplateArgumentList(tpl_args, args...);

  SgType * base_type = SageBuilder::buildIntType(); // TODO instantiate base of typedef?

  SgTemplateTypedefDeclaration * tpl_decl = isSgTemplateTypedefDeclaration(sym->get_declaration());
  ROSE_ASSERT(tpl_decl);

  SgScopeStatement * defn_scope = tpl_decl->get_scope();
  SgName type_name(sym->get_name().getString());
  SgTemplateInstantiationTypedefDeclaration * tddecl = SageBuilder::buildTemplateInstantiationTypedefDeclaration_nfi(
    type_name, base_type, defn_scope, false, tpl_decl, tpl_args
  );
  ROSE_ASSERT(tddecl != nullptr);
  defn_scope->append_statement(tddecl);

  return tddecl;
}

template <typename CRT, typename API>
template <typename... Args>
reference_t<Object::a_typedef> * __factory_helper_t<CRT, API, Object::a_typedef>::reference(
  Factory<CRT, API> const & factory,
  symbol_t<Object::a_typedef> * sym,
  SgNamedType * parent,
  Args... args
) {
  ROSE_ASSERT(!parent); // TODO case of a field or method
  return sym->get_declaration()->get_type();
}

} }

#endif /* Rose_CodeGen_factory_typedefs_txx */

