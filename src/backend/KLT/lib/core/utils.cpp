
#include "KLT/utils.hpp"
#include "sage3basic.h"
#include "AstFromString.h"

namespace KLT {

void initAstFromString(std::ifstream & in_file) {
  SgScopeStatement * scope = SageBuilder::buildBasicBlock();

  AstFromString::c_sgnode = scope;
  SageBuilder::pushScopeStack(scope);

  SgName pow_name("pow");
  SgFunctionDeclaration * pow_decl = SageBuilder::buildNondefiningFunctionDeclaration(
                                       pow_name,
                                       SageBuilder::buildDoubleType(),
                                       SageBuilder::buildFunctionParameterList(
                                         SageBuilder::buildInitializedName("base", SageBuilder::buildDoubleType()),
                                         SageBuilder::buildInitializedName("exp", SageBuilder::buildDoubleType())
                                       ),
                                       scope,
                                       NULL
                                     );
  scope->insert_symbol(pow_name, new SgFunctionSymbol(pow_decl));

  SgName sqrt_name("sqrt");
  SgFunctionDeclaration * sqrt_decl = SageBuilder::buildNondefiningFunctionDeclaration(
                                       sqrt_name,
                                       SageBuilder::buildDoubleType(),
                                       SageBuilder::buildFunctionParameterList(
                                         SageBuilder::buildInitializedName("v", SageBuilder::buildDoubleType())
                                       ),
                                       scope,
                                       NULL
                                     );
  scope->insert_symbol(sqrt_name, new SgFunctionSymbol(sqrt_decl));

  SgName abs_name("fabs");
  SgFunctionDeclaration * abs_decl = SageBuilder::buildNondefiningFunctionDeclaration(
                                       abs_name,
                                       SageBuilder::buildDoubleType(),
                                       SageBuilder::buildFunctionParameterList(
                                         SageBuilder::buildInitializedName("v", SageBuilder::buildDoubleType())
                                       ),
                                       scope,
                                       NULL
                                     );
  scope->insert_symbol(abs_name, new SgFunctionSymbol(abs_decl));

  in_file.seekg (0, in_file.end);
  int length = in_file.tellg();
  in_file.seekg (0, in_file.beg);

  char * tmp_char_str = new char [length + 1];
  in_file.read(tmp_char_str, length);
  tmp_char_str[length] = 0;

  AstFromString::c_char = tmp_char_str;
  AstFromString::afs_skip_whitespace();
}

void ensure(char c) {
  AstFromString::afs_skip_whitespace();
  assert(AstFromString::afs_match_char(c));
  AstFromString::afs_skip_whitespace();
}

}

