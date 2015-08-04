
#ifndef __KLT_UTILS_HPP__
#define __KLT_UTILS_HPP__

#include <iostream>
#include <fstream>
#include <map>

class SgExpression;
class SgVarRefExp;
class SgStatement;
class SgVariableSymbol;
namespace KLT {
  class Generator;
  namespace Descriptor {
    struct loop_t;
    struct tile_t;
    struct section_t;
    struct data_t;
    struct kernel_t;
  }
  namespace Kernel {
    struct kernel_t;
  }
  namespace LoopTree {
    struct node_t;
    struct block_t;
    struct cond_t;
    struct loop_t;
    struct tile_t;
    struct stmt_t;
  }
}

namespace KLT {

namespace Utils {

struct symbol_map_t {
  typedef SgVariableSymbol vsym_t;
  typedef ::KLT::Descriptor::data_t data_t;

  typedef std::map<size_t  , vsym_t *> iter_translation_t;
  typedef std::map<vsym_t *, vsym_t *> vsym_translation_t;
  typedef std::map<vsym_t *, data_t *> data_map_t;

  iter_translation_t iter_loops;
  vsym_translation_t orig_loops;
  iter_translation_t iter_tiles;

  vsym_translation_t parameters;

  data_map_t data;
  vsym_translation_t data_trans;
  vsym_translation_t data_rtrans;

  vsym_t * loop_context;
  vsym_t * data_context;

  SgVarRefExp * translate(SgVarRefExp * expr) const;
  SgPntrArrRefExp * translate(SgPntrArrRefExp * expr) const;
  SgExpression * translate(SgExpression * expr) const;
  SgStatement  * translate(SgStatement  * stmt) const;
};

} // namespace KLT::Utils

void initAstFromString(std::ifstream & in_file);

void ensure(char c);

} // namespace KLT

#endif /* __KLT_UTILS_HPP__ */
