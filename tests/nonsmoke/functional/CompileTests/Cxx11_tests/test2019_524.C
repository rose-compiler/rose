// Here is your next ARDRA bug: ROSE-2428

//REQUIRED: typedef:
typedef __SIZE_TYPE__ size_t;
//REQUIRED: templte:
template <typename t_t_parm_5>
struct Struct_3 {
  template <int t_parm_2>
  struct Struct_2 {
  };
};

template <typename T>
class Class_1 {
public:
  static void func_2() {};
};

// REQUIRED: size_t, not int:
template <size_t t_parm_1>
struct Struct_1 {
  template <typename t_t_parm_4>
  static void func_1() {
    Class_1<typename Struct_3<t_t_parm_4>::template Struct_2<t_parm_1 - 1>::type>::func_2();
  }
};

// produces this error:
// identityTranslator[9216] 1.23522s Rose[FATAL]: assertion failed:
// identityTranslator[9216] 1.23535s Rose[FATAL]:   /g/g17/charles/code/ROSE/rose-master-0.9.13.8/src/frontend/CxxFrontend/EDG/edgRose/edgRose.C:18987
// identityTranslator[9216] 1.23541s Rose[FATAL]:   SgExpression *EDG_ROSE_Translation::convert_constant_template_parameter(a_constant *)
// identityTranslator[9216] 1.23546s Rose[FATAL]:   required: edg_scope != __null

