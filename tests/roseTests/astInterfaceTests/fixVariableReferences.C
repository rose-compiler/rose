#include "rose.h"

namespace si = SageInterface;
namespace sb = SageBuilder;

int main(int argc, char *argv[]) {
  SgProject* proj = frontend(argc, argv);
  proj->skipfinalCompileStep(true);

  SgFunctionDefinition *m_def =
      si::findFunctionDeclaration(
          proj, "foo", NULL, true)->get_definition();

  // This is ok
  si::appendStatement(
      sb::buildExprStatement(
          sb::buildVarRefExp("a")),
      m_def->get_body());

  // This causes an assertion failure in fixVariableReferences
  si::appendStatement(
      sb::buildExprStatement(
          sb::buildArrowExp(
              sb::buildVarRefExp("a"),
              sb::buildVarRefExp("x"))),
      m_def->get_body());
  
  fprintf(stderr, "Fixing variable references\n");
  si::fixVariableReferences(proj);

  fprintf(stderr, "Testing\n");
  AstTests::runAllTests(proj);
  
  return backend(proj);
}
