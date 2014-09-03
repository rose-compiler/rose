#ifndef DFASTATTRIBUTE_H
#define DFASTATTRIBUTE_H

#include <string>
using std::string;

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
class DFAstAttribute : public AstAttribute {
 public:
  DFAstAttribute();
  string virtual toString();
  ~DFAstAttribute();
};

#endif
