#ifndef ASLATTRIBUTE_H
#define ASLATTRIBUTE_H

#include <satire_rose.h>
#include <string>
#include "asl.h"


#define ASL_ATTRIBUTE_ID "SATIRE:aslattribute"

class ASLAttribute : public AstAttribute{
 private:
  std::string location;

 public:
  ASLAttribute(AbstractSourceLocation asl);
  std::string toString();
};


#endif
