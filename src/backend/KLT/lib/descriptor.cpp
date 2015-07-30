
#include "KLT/descriptor.hpp"

namespace KLT {

namespace Descriptor {

tile_t::tile_t() : id(0), kind(e_not_tile), order(0), param(NULL) {}
tile_t::tile_t(size_t id_, tile_kind_e kind_, size_t order_, SgExpression * param_) : id(id_), kind(kind_), order(order_), param(param_) {}

loop_t::loop_t() : id(0), lb(NULL), ub(NULL), stride(NULL), tiles(), iterator(NULL) {}
loop_t::loop_t(size_t id_, SgExpression * lb_, SgExpression * ub_, SgExpression * stride_, SgVariableSymbol * iterator_) :
  id(id_), lb(lb_), ub(ub_), stride(stride_), tiles(), iterator(iterator_) {}

section_t::section_t(SgExpression * offset_, SgExpression * length_) :
  offset(offset_), length(length_) {}

data_t::data_t(SgVariableSymbol * symbol_, SgType * base_type_) :
  symbol(symbol_), base_type(base_type_), sections() {}

kernel_t::kernel_t(size_t id_, std::string kernel_name_) :
  id(id_), kernel_name(kernel_name_), loops(), tiles(), parameters(), data() {}

} // namespace KLT::Descriptor

} // namespace KLT

