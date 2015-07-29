
#ifndef __KLT_UTILS_HPP__
#define __KLT_UTILS_HPP__

#include <iostream>
#include <fstream>
#include <map>

class SgExpression;
class SgStatement;
class SgVariableSymbol;
namespace KLT {
  class Runtime;
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
  typedef std::map<vsym_t *, vsym_t *> vsym_translation_t;
  typedef std::map<data_t *, vsym_t *> data_translation_t;

  vsym_translation_t iterators;
  vsym_translation_t parameters;
  data_translation_t data;

  vsym_t * loop_context;
  vsym_t * data_context;
};

SgExpression * translateExpression(SgExpression * expr, const symbol_map_t & symbol_map);
SgStatement  * translateStatement (SgStatement  * stmt, const symbol_map_t & symbol_map);

}

void initAstFromString(std::ifstream & in_file);

void ensure(char c);

}

#endif /* __KLT_UTILS_HPP__ */
