
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
  Factory<CRT, API> const & /*factory*/,
  symbol_t<Object::a_class> * sym,
  SgNamedType * /*parent*/,
  tplargs_t & tpl_args
) {
#if DEBUG___factory_helper_t__a_class__instantiate
  std::cout << "__factory_helper_t<CRT, API, Object::a_class>::instantiate" << std::endl;
  std::cout << "  sym    = " << std::hex << sym << " : " << ( sym ? sym->class_name() : "" ) << std::endl;
#endif

  SgName fname(sym->get_name().getString());
  SgName fname_tplargs = SageBuilder::appendTemplateArgumentsToName(fname, tpl_args);

  SgTemplateClassDeclaration * tpl_decl = isSgTemplateClassDeclaration(sym->get_declaration());
  ROSE_ASSERT(tpl_decl);
#if DEBUG___factory_helper_t__a_class__instantiate
  std::cout << "  tpl_decl    = " << std::hex << tpl_decl << " : " << ( tpl_decl ? tpl_decl->class_name() : "" ) << std::endl;
#endif

  SgScopeStatement * defn_scope = tpl_decl->get_scope();
  SgTemplateInstantiationDecl * xdecl = new SgTemplateInstantiationDecl(fname_tplargs, tpl_decl->get_class_type(), nullptr, nullptr, tpl_decl, tpl_args);
  ROSE_ASSERT(xdecl != nullptr);
#if DEBUG___factory_helper_t__a_class__instantiate
  std::cout << "  xdecl    = " << std::hex << xdecl << " : " << ( xdecl ? xdecl->class_name() : "" ) << std::endl;
#endif
  xdecl->set_parent(defn_scope);
  xdecl->set_templateName(fname);
  xdecl->set_scope(defn_scope);
  defn_scope->insert_symbol(fname_tplargs, new SgClassSymbol(xdecl));

  // We need it to be first defining to create a type
  xdecl->set_firstNondefiningDeclaration(xdecl);
  xdecl->set_type(SgClassType::createType(xdecl));

  // We really need a definition to instantiate members (FIXME what does SgTemplateInstantiationDefn bring?)
  SgTemplateInstantiationDefn * xdefn = new SgTemplateInstantiationDefn();
  xdecl->set_definition(xdefn);
  xdefn->set_parent(xdecl);
  xdecl->set_definingDeclaration(xdecl);

  SageInterface::setSourcePositionForTransformation(xdecl);

  for (auto tpl_arg: tpl_args) {
    tpl_arg->set_parent(xdecl);
  }
  
  return xdecl;
}

#ifdef NOTDEFN
  SgType * base_type = Rose::Builder::Templates::instantiateNonrealTypes(tpl_decl->get_base_type(), tpl_decl->get_templateParameters(), tpl_args);
#if DEBUG___factory_helper_t__a_class__instantiate
  std::cout << "  base_type    = " << std::hex << base_type << " : " << ( base_type ? base_type->class_name() : "" ) << std::endl;
#endif

  SgScopeStatement * defn_scope = tpl_decl->get_scope();
  SgName type_name(sym->get_name().getString());
  SgTemplateInstantiationTypedefDeclaration * tddecl = SageBuilder::buildTemplateInstantiationTypedefDeclaration_nfi(
    type_name, base_type, defn_scope, false, tpl_decl, tpl_args
  );
  ROSE_ASSERT(tddecl != nullptr);
#if DEBUG___factory_helper_t__a_class__instantiate
  std::cout << "  tddecl    = " << std::hex << tddecl << " : " << ( tddecl ? tddecl->class_name() : "" ) << std::endl;
  std::cout << "    ->get_base_type()    = " << std::hex << tddecl->get_base_type() << " : " << ( tddecl->get_base_type() ? tddecl->get_base_type()->class_name() : "" ) << std::endl;
#endif
  if (tddecl->get_base_type() != base_type) {
    tddecl->set_base_type(base_type); // Found previously built typedef but base-type different => comes from frontend and is probably incomplete
  }
  defn_scope->append_statement(tddecl);

  for (auto tpl_arg: tpl_args) {
    tpl_arg->set_parent(tddecl);
  }
#endif

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
  Factory<CRT, API> const & /*factory*/,
  symbol_t<Object::a_class> * sym,
  SgNamedType * parent,
  Args... /*args*/
) {
  ROSE_ASSERT(!parent); // TODO case of a field or method
  return sym->get_declaration()->get_type();
}

} }

#endif /* Rose_CodeGen_factory_classes_txx */

