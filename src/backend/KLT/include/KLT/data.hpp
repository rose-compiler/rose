
#ifndef __KLT_DATA_HPP__
#define __KLT_DATA_HPP__

#include <vector>

class SgType;
class SgExpression;
class SgVariableSymbol;

namespace KLT {

namespace Data {

struct section_t {
  SgExpression * offset;
  SgExpression * length;

  section_t(SgExpression * offset_ = NULL, SgExpression * length_ = NULL);
};

struct data_t {
  typedef SgVariableSymbol vsym;

  vsym * symbol;
  SgType * base_type;
  std::vector<section_t> sections;

  data_t(vsym * symbol_, SgType * base_type_);
};

}

}

#endif /* __KLT_DATA_HPP__ */

