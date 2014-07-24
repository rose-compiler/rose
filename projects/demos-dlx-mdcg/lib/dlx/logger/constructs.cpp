
#include "DLX/Logger/language.hpp"

namespace DLX {

namespace Directives {

template <>
generic_construct_t<Logger::language_t> * buildConstruct<Logger::language_t>(Logger::language_t::construct_kinds_e kind) {
  switch (kind) {
    case Logger::language_t::e_logger_construct_log:
     return new construct_t<Logger::language_t, Logger::language_t::e_logger_construct_log>();
    case Logger::language_t::e_construct_last:
     assert(false);
    default:
     assert(false);
  }
}

template <>
bool findAssociatedNodes<Logger::language_t>(
  SgLocatedNode * directive_node,
  generic_construct_t<Logger::language_t> * construct,
  const std::map<SgLocatedNode *, directive_t<Logger::language_t> *> & translation_map
) {
  switch (construct->kind) {
    case Logger::language_t::e_logger_construct_log:
     return Frontend::Frontend<Logger::language_t>::findAssociatedNodes<Logger::language_t::e_logger_construct_log>(
       directive_node, (construct_t<Logger::language_t, Logger::language_t::e_logger_construct_log> *)construct, translation_map
     );
    case Logger::language_t::e_construct_last:
     assert(false);
    default:
     assert(false);
  }
}

}

}

