
#include "sage3basic.h"
#include "KLT/utils.hpp"

namespace KLT {

namespace Utils {

SgExpression * translateExpression(SgExpression * expr, const symbol_map_t & symbol_map) {
  SgExpression * res = SageInterface::copyExpression(expr);

  // TODO

  return res;
}

SgStatement * translateStatement(SgStatement * stmt, const symbol_map_t & symbol_map) {
  SgStatement * res = SageInterface::copyStatement(stmt);

  // TODO

  return res;
}

}

}

