#ifndef DFASTATTRIBUTE_H
#define DFASTATTRIBUTE_H

#include <iostream>
#include <string>
using std::string;

#include "VariableIdMapping.h"

namespace CodeThorn {

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
class DFAstAttribute : public AstAttribute {
 public:
  DFAstAttribute();
  string virtual toString();
  void virtual toStream(std::ostream& os,VariableIdMapping*);
  ~DFAstAttribute();
};

}

// backward compatibility
namespace SPRAY = CodeThorn;

#endif
