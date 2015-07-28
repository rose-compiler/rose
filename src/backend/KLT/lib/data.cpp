
#include "KLT/data.hpp"

namespace KLT {

namespace Data {

section_t::section_t(SgExpression * offset_, SgExpression * length_) :
  offset(offset_), length(length_)
{}

data_t::data_t(vsym * symbol_, SgType * base_type_) :
  symbol(symbol_), base_type(base_type_), sections()
{}

}

}

