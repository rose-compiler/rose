
#include "scsm-sage.hpp"

#include "sage3basic.h"

#include "AstConsistencyTests.h"

int main(int argc, char ** argv) {
  SgProject * project = new SgProject::SgProject();
  { // Add default command line to an empty project
    std::vector<std::string> arglist;
      arglist.push_back("c++");
      arglist.push_back("-c");
    project->set_originalCommandLineArgumentList (arglist);
  }

  SCSM::Driver<SCSM::Sage> driver("output", project);

  SgClassSymbol * class_symbol = NULL;
  {
    SCSM::Sage<SgClassDeclaration>::object_desc_t desc;
      desc.name = "A";
      desc.kind = (unsigned long)SgClassDeclaration::e_class;

    SCSM::Sage<SgClassDeclaration>::build_result_t result = driver.build<SgClassDeclaration>(desc);

    class_symbol = result.symbol;
  }
  assert(class_symbol != NULL);

  SgMemberFunctionSymbol * member_function_symbol = NULL;
  {
    SCSM::Sage<SgMemberFunctionDeclaration>::object_desc_t desc;
      desc.name = "foo";
      desc.return_type = SageBuilder::buildVoidType();
      desc.params = SageBuilder::buildFunctionParameterList();
      desc.parent_class = class_symbol;

    SCSM::Sage<SgMemberFunctionDeclaration>::build_result_t result = driver.build<SgMemberFunctionDeclaration>(desc);

    member_function_symbol = result.symbol;

    SgFunctionDefinition * definition = result.definition;

    // TODO
  }

  SgFunctionSymbol * function_symbol = NULL;
  {
    SCSM::Sage<SgFunctionDeclaration>::object_desc_t desc;
      desc.name = "foo";
      desc.return_type = SageBuilder::buildVoidType();
      desc.params = SageBuilder::buildFunctionParameterList();

    SCSM::Sage<SgFunctionDeclaration>::build_result_t result = driver.build<SgFunctionDeclaration>(desc);

    function_symbol = result.symbol;

    SgScopeStatement * scope = result.definition->get_body();

    SgStatement * stmt = SageBuilder::buildVariableDeclaration("a", class_symbol->get_type(), NULL, scope);
    SageInterface::appendStatement(stmt, scope);

    SgVariableSymbol * var_symbol = scope->lookup_variable_symbol("a");
    assert(var_symbol != NULL);

    stmt = SageBuilder::buildExprStatement(
      SageBuilder::buildFunctionCallExp(
        SageBuilder::buildDotExp(
          SageBuilder::buildVarRefExp(var_symbol),
          SageBuilder::buildMemberFunctionRefExp(member_function_symbol, false, false)
        ),
        SageBuilder::buildExprListExp()
      )
    );
    SageInterface::appendStatement(stmt, scope);
  }
  assert(function_symbol != NULL);

//AstTests::runAllTests(project);

  project->unparse();

  return 0;
}

