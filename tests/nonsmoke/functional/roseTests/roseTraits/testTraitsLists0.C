
#include "Rose/Traits/grammar/lists.h"

#define DO_NODE 1
#define DO_NTYPE 0

#define DO_ALL 1
#define DO_CONCRETE 0

#define DO_OUTPUT 0

// The class hierarchy can be inspected to retrieve specific classes

#if DO_ALL
// Listing all subclasses of a given class (including itself)
# if DO_NODE
using all = Rose::Traits::list_all_subclasses<SgNode>;
# endif
# if DO_NTYPE
using named_types = Rose::Traits::list_all_subclasses<SgNamedType>;
# endif
#endif

#if DO_CONCRETE
// Or, only the concrete subclasses.
# if DO_NODE
using concrete = Rose::Traits::list_concrete_subclasses<SgNode>;
# endif
# if DO_NTYPE
using concrete_named_types = Rose::Traits::list_concrete_subclasses<SgNamedType>;
# endif
#endif

int main(int argc, char * argv[]) {
#if DO_OUTPUT
# if DO_ALL
#  if DO_NODE
  std::cout << "Number of nodes:                " << std::dec << all::length << std::endl;
#  endif
#  if DO_NTYPE
  std::cout << "Number of named types:          " << std::dec << named_types::length << std::endl;
#  endif
# endif
# if DO_CONCRETE
#  if DO_NODE
  std::cout << "Number of concrete nodes:       " << std::dec << concrete::length << std::endl;
#  endif
#  if DO_NTYPE
  std::cout << "Number of concrete named types: " << std::dec << concrete_named_types::length << std::endl;
#  endif
# endif
#endif
  return 0;
}

