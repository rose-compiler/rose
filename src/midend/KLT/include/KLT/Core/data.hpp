
#ifndef __KLT_DATA_HPP__
#define __KLT_DATA_HPP__

#include <vector>

class SgType;
class SgExpression;
class SgVariableSymbol;

namespace KLT {

namespace Descriptor {
  struct section_t;
  struct data_t;
}

namespace Data {

typedef ::KLT::Descriptor::section_t section_t;

typedef ::KLT::Descriptor::data_t data_t;

}

}

#endif /* __KLT_DATA_HPP__ */

