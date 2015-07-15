
#include "sage3basic.h"

#include "DLX/Core/constructs.hpp"
#include "DLX/Core/frontend.hpp"
#include "DLX/Core/directives.hpp"
#include "DLX/TileK/language.hpp"

#include <cassert>

class SgLocatedNode;

namespace DLX {

namespace Directives {

template <>
generic_construct_t<TileK::language_t> * buildConstruct<TileK::language_t>(TileK::language_t::construct_kinds_e kind) {
  switch (kind) {
    case TileK::language_t::e_construct_kernel:
     return new construct_t<TileK::language_t, TileK::language_t::e_construct_kernel>();
    case TileK::language_t::e_construct_loop:
     return new construct_t<TileK::language_t, TileK::language_t::e_construct_loop>();
    case TileK::language_t::e_construct_last:
     assert(false);
    default:
     assert(false);
  }
}

template <>
bool findAssociatedNodes<TileK::language_t>(
  SgLocatedNode * directive_node,
  generic_construct_t<TileK::language_t> * construct,
  const std::map<SgLocatedNode *, directive_t<TileK::language_t> *> & translation_map
) {
  switch (construct->kind) {
    case TileK::language_t::e_construct_kernel:
     return Frontend::Frontend<TileK::language_t>::findAssociatedNodes<TileK::language_t::e_construct_kernel>(
       directive_node, (construct_t<TileK::language_t, TileK::language_t::e_construct_kernel> *)construct, translation_map
     );
    case TileK::language_t::e_construct_loop:
     return Frontend::Frontend<TileK::language_t>::findAssociatedNodes<TileK::language_t::e_construct_loop>(
       directive_node, (construct_t<TileK::language_t, TileK::language_t::e_construct_loop> *)construct, translation_map
     );
    case TileK::language_t::e_construct_last:
     assert(false);
    default:
     assert(false);
  }
}

}

}

