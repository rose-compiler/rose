
namespace MyNsp {
  using my_typedef_t = int;

  struct my_class_t {};

  void my_function();

  int my_variable;
};

namespace OtherNsp {
  template <typename T, bool>
  struct MyTplCls {
    using type_t = T const *;
  };

  template <typename T>
  struct MyTplCls<T,false> {
    using type_t = T const &;
  };

  template <typename T>
  using OtherTplClass1 = MyTplCls<T, false>;

  template <typename T>
  using OtherTplClass2 = MyTplCls<T, true>;

  
  template <class RetT, class ValT>
  RetT my_tpl_function(ValT my_param);
};

