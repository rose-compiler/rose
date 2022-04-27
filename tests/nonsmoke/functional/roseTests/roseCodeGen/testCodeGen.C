
#include "sage3basic.h"

#include "Rose/CodeGen.h"

#ifndef MY_HEADER_PATH
#  error "MY_HEADER_PATH must be set!"
#endif

struct my_api_t : Rose::CodeGen::API<my_api_t> {
  a_namespace MyNsp        { nullptr };
  a_class     my_class_t   { nullptr };
  a_typedef   my_typedef_t { nullptr };
  a_function  my_function  { nullptr };
  a_variable  my_variable  { nullptr };
};

namespace Rose { namespace CodeGen {

template <>
std::string const API<my_api_t>::name{"MyAPI"};

template <>
std::vector<std::string> const API<my_api_t>::paths{MY_HEADER_PATH};

template <>
std::vector<std::string> const API<my_api_t>::flags{"-DSKIP_ROSE_BUILTIN_DECLARATIONS"};

template <>
std::vector<std::string> const API<my_api_t>::files{"my_lib.h"};

template <>
std::map<std::string, API<my_api_t>::a_namespace my_api_t::* > const API<my_api_t>::namespaces{
  { "::MyNsp", &my_api_t::MyNsp    }
};

template <>
std::map<std::string, API<my_api_t>::a_class my_api_t::* > const API<my_api_t>::classes{
  { "::MyNsp::my_class_t", &my_api_t::my_class_t     }
};

template <>
std::map<std::string, API<my_api_t>::a_typedef my_api_t::* > const API<my_api_t>::typedefs{
  { "::MyNsp::my_typedef_t", &my_api_t::my_typedef_t     }
};

template <>
std::map<std::string, API<my_api_t>::a_variable my_api_t::* > const API<my_api_t>::variables{
  { "::MyNsp::my_variable", &my_api_t::my_variable }
};

template <>
std::map<std::string, API<my_api_t>::a_function my_api_t::* > const API<my_api_t>::functions{
  { "::MyNsp::my_function", &my_api_t::my_function }
};

} }

struct my_factory_t : Rose::CodeGen::Factory<my_factory_t, my_api_t> {
  using Rose::CodeGen::Factory<my_factory_t, my_api_t>::Factory;
};

int main( int argc, char * argv[] ) {
  ROSE_INITIALIZE;
  std::vector<std::string> args(argv, argv+argc);
  Rose::CodeGen::Driver driver(args);

  my_factory_t my_factory(driver);

  my_factory.api.display(std::cout);

  auto res = my_factory.reference<Rose::CodeGen::Object::a_variable>(&my_api_t::my_variable);
  std::cout << "res = " << std::hex << res << " : " << (res ? res->class_name() : "") << std::endl;

  return backend(driver.project);
}

