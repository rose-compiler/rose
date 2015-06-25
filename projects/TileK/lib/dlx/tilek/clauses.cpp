
#include "DLX/Core/clauses.hpp"
#include "DLX/Core/frontend.hpp"
#include "DLX/TileK/language.hpp"

#include <cassert>

class SgLocatedNode;

namespace DLX {

namespace Directives {

template <>
generic_clause_t<TileK::language_t> * buildClause<TileK::language_t>(TileK::language_t::clause_kinds_e kind) {
  switch (kind) {
    case TileK::language_t::e_clause_data:
      return new clause_t<TileK::language_t, TileK::language_t::e_clause_data>();
    case TileK::language_t::e_clause_tile:
      return new clause_t<TileK::language_t, TileK::language_t::e_clause_tile>();
    case TileK::language_t::e_clause_last:
      assert(false);
    default:
     assert(false);
  }
}
 
template <>
bool parseClauseParameters<TileK::language_t>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  generic_clause_t<TileK::language_t> * clause
) {
  switch (clause->kind) {
    case TileK::language_t::e_clause_data:
      return Frontend::Frontend<TileK::language_t>::parseClauseParameters<TileK::language_t::e_clause_data>(
        directive_str, directive_node, (clause_t<TileK::language_t, TileK::language_t::e_clause_data> *)clause
      );
    case TileK::language_t::e_clause_tile:
      return Frontend::Frontend<TileK::language_t>::parseClauseParameters<TileK::language_t::e_clause_tile>(
        directive_str, directive_node, (clause_t<TileK::language_t, TileK::language_t::e_clause_tile> *)clause
      );
    case TileK::language_t::e_clause_last:
      assert(false);
    default:
      assert(false);
  }
}

}

}

