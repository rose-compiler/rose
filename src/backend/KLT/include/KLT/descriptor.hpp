
#ifndef __KLT_DESCRIPTOR_HPP__
#define __KLT_DESCRIPTOR_HPP__

#include <vector>
#include <string>

#include <cstddef>

class SgExpression;
class SgVariableSymbol;
class SgType;

namespace KLT {

namespace Descriptor {

enum tile_kind_e {
  e_not_tile = -1,
  e_static_tile = 0,
  e_dynamic_tile = 1,
  e_last_klt_tile = e_dynamic_tile
};

struct tile_t {
  size_t id;
  tile_kind_e kind;
  size_t order;
  SgExpression * param;

  tile_t();
  tile_t(size_t id_, tile_kind_e kind_, size_t order_, SgExpression * param_);
};

struct loop_t {
  size_t id;
  SgExpression * lb;
  SgExpression * ub;
  SgExpression * stride;
  std::vector<tile_t *> tiles;
  SgVariableSymbol * iterator;

  loop_t();
  loop_t(size_t id_, SgExpression * lb_, SgExpression * ub_, SgExpression * stride_, SgVariableSymbol * iterator_);
};

struct section_t {
  SgExpression * offset;
  SgExpression * length;

  section_t(SgExpression * offset_, SgExpression * length_);
};

struct data_t {
  SgVariableSymbol * symbol;
  SgType * base_type;
  std::vector<section_t *> sections;

  data_t(SgVariableSymbol * symbol_, SgType * base_type_);
};

struct kernel_t {
  size_t id;
  std::string kernel_name;

  std::vector<loop_t> loops;
  std::vector<tile_t> tiles;

  std::vector<SgVariableSymbol *> parameters;
  std::vector<data_t *> data;

  kernel_t(size_t id_, std::string kernel_name_);
};

} // namespace KLT::Descriptor

} // namespace KLT

#endif /* __KLT_DESCRIPTOR_HPP__ */

