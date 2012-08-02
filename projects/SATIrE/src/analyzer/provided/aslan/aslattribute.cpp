#include "aslattribute.h"

ASLAttribute::ASLAttribute(AbstractSourceLocation asl){
  this->location=asl.toString();
}

std::string ASLAttribute::toString(){
  return location;
}
