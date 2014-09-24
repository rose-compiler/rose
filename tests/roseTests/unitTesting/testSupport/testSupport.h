
/*
 * File to include to make use of the ROSE unit testing support
 * infrastructure
 */

#include "rose.h"
#include "gtest/gtest.h"

template<typename T>
bool isNull(SgNode* t){
 return (dynamic_cast<T*>(t) == NULL);
}



void foo(){

  SgTypeInt *p = SageBuilder::buildIntType();
  if(isNull<SgTypeInt>(p)){
    std::cout << "Is not of type int" << std::endl;
  } else {
    std::cout << "Is of type int" << std::endl;
  }

  SgTypeChar *pp = SageBuilder::buildCharType();
  if(isNull<SgTypeInt>(pp)){
    std::cout << "Is not of type int" << std::endl;
  } else {
    std::cout << "Is of type int" << std::endl;
  }



}
