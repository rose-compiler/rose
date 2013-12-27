
#include "KLT/utils.hpp"
#include "AstFromString.h"

namespace KLT {

void initAstFromString(std::ifstream & in_file) {
  SgScopeStatement * scope = SageBuilder::buildBasicBlock();

  AstFromString::c_sgnode = scope;
  SageBuilder::pushScopeStack(scope);

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

