
#include "MFB/Sage/driver.hpp"

#include "MFB/Sage/namespace-declaration.hpp"
#include "MFB/Sage/function-declaration.hpp"
#include "MFB/Sage/member-function-declaration.hpp"
#include "MFB/Sage/class-declaration.hpp"
#include "MFB/Sage/variable-declaration.hpp"

#include "sage3basic.h"

#include "AstConsistencyTests.h"

int main(int argc, char ** argv) {
  SgProject * project = new SgProject();
  std::vector<std::string> arglist;
    arglist.push_back("c++");
    arglist.push_back("-DSKIP_ROSE_BUILTIN_DECLARATIONS");
    arglist.push_back("-c");
  project->set_originalCommandLineArgumentList (arglist);

  MultiFileBuilder::Driver<MultiFileBuilder::Sage> driver(project);

  // Load a pair header and source files
  unsigned long loaded_file_id = driver.loadPairOfFiles(argv[1], "", "");
  // Create a source file
  unsigned long  build_file_id = driver.createStandaloneSourceFile("test");

  // Get the global scope accross files
  SgGlobal * global_scope_across_files = project->get_globalScopeAcrossFiles();
  assert(global_scope_across_files != NULL);

  // Lookup namespace A
  SgNamespaceSymbol * A_sym = SageInterface::lookupNamespaceSymbolInParentScopes("A", global_scope_across_files);
  assert(A_sym != NULL);
  SgNamespaceDeclarationStatement * A_decl = A_sym->get_declaration();
  assert(A_decl != NULL);
  SgNamespaceDefinitionStatement * A_defn = A_decl->get_definition();
  assert(A_defn != NULL);

  // Lookup namespace B
  SgNamespaceSymbol * B_sym = SageInterface::lookupNamespaceSymbolInParentScopes("B", global_scope_across_files);
  assert(B_sym != NULL);
  SgNamespaceDeclarationStatement * B_decl = B_sym->get_declaration();
  assert(B_decl != NULL);
  SgNamespaceDefinitionStatement * B_defn = B_decl->get_definition();
  assert(B_defn != NULL);

  // Lookup class A::foo
  SgClassSymbol * foo_sym = SageInterface::lookupClassSymbolInParentScopes("foo", A_defn);
  assert(foo_sym != NULL);
  SgClassDeclaration * foo_decl = foo_sym->get_declaration();
  assert(foo_decl != NULL);
  foo_decl = isSgClassDeclaration(foo_decl->get_definingDeclaration());
  assert(foo_decl != NULL);
  SgClassDefinition * foo_defn = foo_decl->get_definition();
  assert(foo_defn != NULL);

  // Lookup field A::foo::x
  SgVariableSymbol * x_sym = SageInterface::lookupVariableSymbolInParentScopes("x", foo_defn);
  assert(x_sym != NULL);

  // Lookup method A::foo::f
  SgMemberFunctionSymbol * f_sym = (SgMemberFunctionSymbol *)SageInterface::lookupFunctionSymbolInParentScopes("f", foo_defn);
  assert(f_sym != NULL);

  // Lookup variable A::y
  SgVariableSymbol * y_sym = SageInterface::lookupVariableSymbolInParentScopes("y", A_defn);
  assert(y_sym != NULL);

  // Lookup function B::bar
  SgFunctionSymbol * bar_sym =  SageInterface::lookupFunctionSymbolInParentScopes("bar", B_defn);
  assert(bar_sym != NULL);

/* Goal:

#include "foo-bar.hpp"

namespace C {
  void test() {
    A::foo tmp = A::y.f();
    tmp.x = 1;
    B::bar(tmp);
  }
}

*/

  // Create namespace C

  MultiFileBuilder::Sage<SgNamespaceDeclarationStatement>::object_desc_t C_desc("C", NULL, build_file_id);

  SgNamespaceSymbol * C_sym = driver.build<SgNamespaceDeclarationStatement>(C_desc);

  // Create function C::test

  MultiFileBuilder::Sage<SgFunctionDeclaration>::object_desc_t test_desc(
    "test",
    SageBuilder::buildVoidType(),
    SageBuilder::buildFunctionParameterList(),
    C_sym,
    build_file_id
  );

  MultiFileBuilder::Sage<SgFunctionDeclaration>::build_result_t test_result = driver.build<SgFunctionDeclaration>(test_desc);

  // Get the C::test body

  SgBasicBlock * test_body = test_result.definition->get_body();
  assert(test_body != NULL);
  SgStatement * stmt = NULL;

  // Notifies the driver that we use loaded symbols in this scope

  foo_sym = driver.useSymbol<SgClassDeclaration>         (foo_sym, test_body);
  x_sym   = driver.useSymbol<SgVariableDeclaration>      (x_sym,   test_body);
  f_sym   = driver.useSymbol<SgMemberFunctionDeclaration>(f_sym,   test_body);
  y_sym   = driver.useSymbol<SgVariableDeclaration>      (y_sym,   test_body);
  bar_sym = driver.useSymbol<SgFunctionDeclaration>      (bar_sym, test_body);

  // Build declaration:   A::foo tmp = A::y.f();

  SgInitializer * init_tmp = SageBuilder::buildAssignInitializer(
    SageBuilder::buildFunctionCallExp(
      SageBuilder::buildDotExp(
        SageBuilder::buildVarRefExp(y_sym),
        SageBuilder::buildMemberFunctionRefExp(f_sym, false, false)
      ),
      SageBuilder::buildExprListExp()
    )
  );
  stmt = SageBuilder::buildVariableDeclaration("tmp", foo_sym->get_type(), init_tmp, test_body);
  SageInterface::appendStatement(stmt, test_body);

  SgVariableSymbol * tmp_sym = test_body->lookup_variable_symbol("tmp");
  assert(tmp_sym != NULL);

  // Build expression statement:   tmp.x = 1;

  stmt = SageBuilder::buildExprStatement(
    SageBuilder::buildAssignOp(
      SageBuilder::buildDotExp(
        SageBuilder::buildVarRefExp(tmp_sym),
        SageBuilder::buildVarRefExp(x_sym)
      ),
      SageBuilder::buildIntVal(1)
    )
  );
  SageInterface::appendStatement(stmt, test_body);

  // Build function call:   B::bar(tmp);

  stmt = SageBuilder::buildExprStatement(
    SageBuilder::buildFunctionCallExp(
      SageBuilder::buildFunctionRefExp(bar_sym),
      SageBuilder::buildExprListExp(SageBuilder::buildVarRefExp(tmp_sym))
    )
  );
  SageInterface::appendStatement(stmt, test_body);

  // Unparse

  project->unparse();

  return 0;
}

