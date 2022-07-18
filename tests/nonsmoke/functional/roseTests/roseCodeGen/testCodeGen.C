
#include "sage3basic.h"

#include "Rose/CodeGen.h"

#ifndef MY_HEADER_PATH
#  error "MY_HEADER_PATH must be set!"
#endif

struct my_api_t : Rose::CodeGen::API<my_api_t> {
  a_namespace MyNsp          { nullptr };
  a_typedef   my_typedef_t   { nullptr };
  a_class     my_class_t     { nullptr };
  a_function  my_function    { nullptr };
  a_variable  my_variable    { nullptr };
  a_namespace OtherNsp       { nullptr };
  a_class     MyTplCls       { nullptr };
  a_typedef   OtherTplClass1 { nullptr };
  a_typedef   OtherTplClass2 { nullptr };
};

namespace Rose { namespace CodeGen {

template <>
std::string const API<my_api_t>::name{"MyAPI"};

template <>
std::string const API<my_api_t>::cache{""};

template <>
std::vector<std::string> const API<my_api_t>::paths{MY_HEADER_PATH};

template <>
std::vector<std::string> const API<my_api_t>::flags{"-DSKIP_ROSE_BUILTIN_DECLARATIONS"};

template <>
std::vector<std::string> const API<my_api_t>::files{"my_lib.h"};

template <>
std::map<std::string, API<my_api_t>::a_namespace my_api_t::* > const API<my_api_t>::namespaces{
  { "::MyNsp",    &my_api_t::MyNsp    },
  { "::OtherNsp", &my_api_t::OtherNsp    }
};

template <>
std::map<std::string, API<my_api_t>::a_class my_api_t::* > const API<my_api_t>::classes{
  { "::MyNsp::my_class_t", &my_api_t::my_class_t     },
  { "::OtherNsp::MyTplCls", &my_api_t::MyTplCls     }
};

template <>
std::map<std::string, API<my_api_t>::a_typedef my_api_t::* > const API<my_api_t>::typedefs{
  { "::MyNsp::my_typedef_t", &my_api_t::my_typedef_t     },
  { "::OtherNsp::OtherTplClass1", &my_api_t::OtherTplClass1     },
  { "::OtherNsp::OtherTplClass2", &my_api_t::OtherTplClass2     }
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

  // Constructing the factory loads the API

  my_factory_t my_factory(driver);
  my_factory.api.display(std::cout);

  // Prepare some files where to test the code generation

  auto header_id = driver.create("test.hxx");
  driver.setUnparsedFile(header_id);
  SgGlobal * header_global = driver.getGlobalScope(header_id);
  driver.addExternalHeader(header_id, "my_lib.h", true);

  auto source_id = driver.create("test.cxx");
  driver.setUnparsedFile(source_id);
  driver.setCompiledFile(source_id);
  SgGlobal * source_global = driver.getGlobalScope(source_id);
  driver.addExternalHeader(source_id, "test.hxx", false);

  auto main_id = driver.create("main.cxx");
  driver.setUnparsedFile(main_id);
  driver.setCompiledFile(main_id);
  SgGlobal * main_global = driver.getGlobalScope(main_id);
  driver.addExternalHeader(main_id, "test.hxx", false);

  // Adding a main function to run-test the generated code
  // TODO argc/argv
  SgFunctionDeclaration * main_decl = SageBuilder::buildDefiningFunctionDeclaration(
      "main", SageBuilder::buildIntType(), SageBuilder::buildFunctionParameterList(), source_global
  );
  SgFunctionDefinition * main_defn = main_decl->get_definition();
  ROSE_ASSERT(main_defn);

  // Test 0: reference to variable from API

  auto varref_0 = my_factory.reference<Rose::CodeGen::Object::a_variable>(&my_api_t::my_variable);
  std::cout << "varref_0 = " << std::hex << varref_0 << " : " << (varref_0 ? varref_0->class_name() : "") << std::endl;
  main_defn->append_statement(SageBuilder::buildExprStatement(varref_0));

  // Test 1: variable declaration with class-type from API

  auto type_1 = my_factory.reference<Rose::CodeGen::Object::a_class>(&my_api_t::my_class_t);
  std::cout << "type_1 = " << std::hex << type_1 << " : " << (type_1 ? type_1->class_name() : "") << std::endl;
  main_defn->append_statement(SageBuilder::buildVariableDeclaration("var_1", type_1, nullptr, main_defn));

  // Test 1: variable declaration with alias-type from API

  auto type_2 = my_factory.reference<Rose::CodeGen::Object::a_typedef>(&my_api_t::my_typedef_t);
  std::cout << "type_2 = " << std::hex << type_2 << " : " << (type_2 ? type_2->class_name() : "") << std::endl;
  main_defn->append_statement(SageBuilder::buildVariableDeclaration("var_2", type_2, nullptr, main_defn));

  // Done: add `main` to its file at last

  main_global->append_statement(main_decl);

  driver.exportNameQualification(std::vector<size_t>{ header_id, source_id, main_id }, my_factory.api.fids());

  return backend(driver.project);
}

