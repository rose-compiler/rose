
#include "DLX/Tooling/language.hpp"

namespace DLX {

namespace Directives {

template <>
generic_clause_t<Tooling::language_t> * buildClause<Tooling::language_t>(Tooling::language_t::clause_kinds_e kind) {
  switch (kind) {
    case Tooling::language_t::e_tooling_clause_var:
      return new clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_var>();
    case Tooling::language_t::e_tooling_clause_value:
      return new clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_value>();
    case Tooling::language_t::e_tooling_clause_file:
      return new clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_file>();
    case Tooling::language_t::e_tooling_clause_format:
      return new clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_format>();
    case Tooling::language_t::e_tooling_clause_tag:
      return new clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_tag>();
    case Tooling::language_t::e_tooling_clause_pack:
      return new clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_pack>();
    case Tooling::language_t::e_clause_last:
      assert(false);
    default:
     assert(false);
  }
}
 
template <>
bool parseClauseParameters<Tooling::language_t>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  generic_clause_t<Tooling::language_t> * clause
) {
  switch (clause->kind) {
    case Tooling::language_t::e_tooling_clause_var:
      return Frontend::Frontend<Tooling::language_t>::parseClauseParameters<Tooling::language_t::e_tooling_clause_var>(
        directive_str, directive_node, (clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_var> *)clause
      );
    case Tooling::language_t::e_tooling_clause_value:
      return Frontend::Frontend<Tooling::language_t>::parseClauseParameters<Tooling::language_t::e_tooling_clause_value>(
        directive_str, directive_node, (clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_value> *)clause
      );
    case Tooling::language_t::e_tooling_clause_file:
      return Frontend::Frontend<Tooling::language_t>::parseClauseParameters<Tooling::language_t::e_tooling_clause_file>(
        directive_str, directive_node, (clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_file> *)clause
      );
    case Tooling::language_t::e_tooling_clause_format:
      return Frontend::Frontend<Tooling::language_t>::parseClauseParameters<Tooling::language_t::e_tooling_clause_format>(
        directive_str, directive_node, (clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_format> *)clause
      );
    case Tooling::language_t::e_tooling_clause_tag:
      return Frontend::Frontend<Tooling::language_t>::parseClauseParameters<Tooling::language_t::e_tooling_clause_tag>(
        directive_str, directive_node, (clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_tag> *)clause
      );
    case Tooling::language_t::e_tooling_clause_pack:
      return Frontend::Frontend<Tooling::language_t>::parseClauseParameters<Tooling::language_t::e_tooling_clause_pack>(
        directive_str, directive_node, (clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_pack> *)clause
      );
    case Tooling::language_t::e_clause_last:
      assert(false);
    default:
      assert(false);
  }
}

}

}

