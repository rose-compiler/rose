
#include "DLX/Logger/language.hpp"

namespace DLX {

namespace Directives {

template <>
generic_clause_t<Logger::language_t> * buildClause<Logger::language_t>(Logger::language_t::clause_kinds_e kind) {
  switch (kind) {
    case Logger::language_t::e_logger_clause_where:
      return new clause_t<Logger::language_t, Logger::language_t::e_logger_clause_where>();
    case Logger::language_t::e_logger_clause_message:
      return new clause_t<Logger::language_t, Logger::language_t::e_logger_clause_message>();
    case Logger::language_t::e_logger_clause_conds:
      return new clause_t<Logger::language_t, Logger::language_t::e_logger_clause_conds>();
    case Logger::language_t::e_logger_clause_params:
      return new clause_t<Logger::language_t, Logger::language_t::e_logger_clause_params>();
    case Logger::language_t::e_clause_last:
      assert(false);
    default:
     assert(false);
  }
}
 
template <>
bool parseClauseParameters<Logger::language_t>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  generic_clause_t<Logger::language_t> * clause
) {
  switch (clause->kind) {
    case Logger::language_t::e_logger_clause_where:
      return Frontend::Frontend<Logger::language_t>::parseClauseParameters<Logger::language_t::e_logger_clause_where>(
        directive_str, directive_node, (clause_t<Logger::language_t, Logger::language_t::e_logger_clause_where> *)clause
      );
    case Logger::language_t::e_logger_clause_message:
      return Frontend::Frontend<Logger::language_t>::parseClauseParameters<Logger::language_t::e_logger_clause_message>(
        directive_str, directive_node, (clause_t<Logger::language_t, Logger::language_t::e_logger_clause_message> *)clause
      );
    case Logger::language_t::e_logger_clause_conds:
      return Frontend::Frontend<Logger::language_t>::parseClauseParameters<Logger::language_t::e_logger_clause_conds>(
        directive_str, directive_node, (clause_t<Logger::language_t, Logger::language_t::e_logger_clause_conds> *)clause
      );
    case Logger::language_t::e_logger_clause_params:
      return Frontend::Frontend<Logger::language_t>::parseClauseParameters<Logger::language_t::e_logger_clause_params>(
        directive_str, directive_node, (clause_t<Logger::language_t, Logger::language_t::e_logger_clause_params> *)clause
      );
    case Logger::language_t::e_clause_last:
      assert(false);
    default:
      assert(false);
  }
}

}

}

