
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
std::string const API<my_api_t>::cache{""};

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

void build_test_files(
  Rose::CodeGen::Driver & driver,
  SgGlobal * & header_global,
  SgGlobal * & source_global,
  SgGlobal * & main_global,
  std::string lib_header
) {
  auto header_id = driver.create("test.hxx");
  driver.setUnparsedFile(header_id);
  header_global = driver.getGlobalScope(header_id);
  driver.addExternalHeader(header_id, lib_header, false);

  auto source_id = driver.create("test.cxx");
  driver.setUnparsedFile(source_id);
  driver.setCompiledFile(source_id);
  source_global = driver.getGlobalScope(source_id);
  driver.addExternalHeader(source_id, "test.hxx", false);

  auto main_id = driver.create("main.cxx");
  driver.setUnparsedFile(main_id);
  driver.setCompiledFile(main_id);
  main_global = driver.getGlobalScope(main_id);
  driver.addExternalHeader(main_id, "test.hxx", false);
}

int main( int argc, char * argv[] ) {
  ROSE_INITIALIZE;
  std::vector<std::string> args(argv, argv+argc);
  Rose::CodeGen::Driver driver(args);

  // Constructing the factory loads the API
  my_factory_t my_factory(driver);
  my_factory.api.display(std::cout);

  // Prepare some files where to test the code generation
  SgGlobal * header_global;
  SgGlobal * source_global;
  SgGlobal * main_global;
  build_test_files(driver, header_global, source_global, main_global, "my_lib.h");

  // Adding a main function to run-test the generated code
  // TODO argc/argv
  SgFunctionDeclaration * main_decl = SageBuilder::buildDefiningFunctionDeclaration(
      "main", SageBuilder::buildIntType(), SageBuilder::buildFunctionParameterList(), source_global
  );
  SgFunctionDefinition * main_defn = main_decl->get_definition();
  ROSE_ASSERT(main_defn);

  // Test 0: added a reference in `main`

  auto res = my_factory.reference<Rose::CodeGen::Object::a_variable>(&my_api_t::my_variable);
  std::cout << "res = " << std::hex << res << " : " << (res ? res->class_name() : "") << std::endl;
  main_defn->append_statement(SageBuilder::buildExprStatement(res));

  // Done: add `main` to its file at last

  main_global->append_statement(main_decl);
  return backend(driver.project);
}

