
#include "Rose/Traits/grammar/predicates.h"

int main(int argc, char * argv[]) {
  std::cout << "Rose::Traits::is_subclass_of<SgValueExp, SgValueExp> = " << Rose::Traits::is_subclass_of<SgValueExp, SgValueExp> << std::endl;
  std::cout << "Rose::Traits::is_subclass_of<SgValueExp, SgIntVal> = " << Rose::Traits::is_subclass_of<SgValueExp, SgIntVal> << std::endl;
  std::cout << "Rose::Traits::is_subclass_of<SgType, SgIntVal> = " << Rose::Traits::is_subclass_of<SgType, SgIntVal> << std::endl;
  return 0;
}

