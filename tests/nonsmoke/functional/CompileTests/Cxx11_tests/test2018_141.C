struct Struct_10 {};

template <typename T, T... vs>
struct Struct_9 {
  using typedef_5 = Struct_9;
};


template <typename T, typename N>
struct Struct_8;
template <typename T>
struct Struct_8<T, Struct_10> : Struct_9<T> {};

template <int t_parm_t>
struct Struct_7 {
  using typedef_4 = typename Struct_8<int, Struct_10>::typedef_5;
};

template <typename T>
struct Struct_6;
template <int... t_parm_2>
struct Struct_6<Struct_9<int, t_parm_2...>> {};

template<typename T>
class Class_5 {
public:
  T *func_2();
};

template<typename T, typename ... typename_2>
class Class_4{
public:
  static constexpr int member_1 = sizeof ... (typename_2);
  //NEEDED:
  using typedef_1 = Struct_6<typename Struct_7<member_1>::typedef_4>;
};

template<typename T, typename ... typename_2>
class Class_3:
  public Class_5<T>,
  public Class_4<Class_3<T, typename_2 ...> >{
};

class Class_2 {};

template<typename T>
class Class_1 {
public:
  void func_1(Class_3<int, Class_2, int> **parm_1);
};

template<typename T>
void Class_1<T>::func_1(Class_3<int, Class_2, int> **parm_1) {
  int *local_1 = parm_1[0]->func_2();
}

#if 0
And the error:

// identityTranslator: /g/g17/charles/code/ROSE/rose-0.9.10.110/src/frontend/SageIII/sageInterface/sageBuilder.C:13079: SgClassDeclaration *SageBuilder::buildClassDeclaration_nfi(const SgName &, SgClassDeclaration::class_types, SgScopeStatement *, SgClassDeclaration *, bool, std::vector<SgTemplateArgument *, std::allocator<SgTemplateArgument *>> *): Assertion `nondefdecl->get_type()->get_declaration() == nondefdecl' failed.

ERROR: nondefdecl = 0x2aaab9c058b0 = SgTemplateInstantiationDecl
ERROR: nondefdecl->get_type() = 0x2aaab9823890 = SgClassType
ERROR: nondefdecl->get_type()->get_declaration() = 0x2aaab9c04f50 = SgTemplateInstantiationDecl
nondefdecl->get_name() = Class_4 < ::Class_3 < int  , ::Class_2 , int  >  > 
nondefdecl->get_type()->get_name() = Class_4 < ::Class_3 < int  , ::Class_2 , int  >  > 
nondefdecl->get_type()->get_declaration()->get_name() = Class_4 < ::Class_3 < int  , ::Class_2 , int  >  > 
nondefdecl->get_mangled_name() = L45R
nondefdecl->get_type()->get_mangled() = L45R
nondefdecl->get_type()->get_declaration()->get_mangled_name() = L45R
#endif

 
