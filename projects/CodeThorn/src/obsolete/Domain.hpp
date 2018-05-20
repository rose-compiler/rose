#ifndef DOMAIN_HPP
#define DOMAIN_HPP

#include "NumberIntervalLattice.h"

class Domain {
public:
  virtual NumberIntervalLattice add(NumberIntervalLattice& e1, NumberIntervalLattice& e2)=0;
  virtual NumberIntervalLattice sub(NumberIntervalLattice& e1, NumberIntervalLattice& e2)=0;
  virtual NumberIntervalLattice mul(NumberIntervalLattice& e1, NumberIntervalLattice& e2)=0;
  virtual NumberIntervalLattice div(NumberIntervalLattice& e1, NumberIntervalLattice& e2)=0;
  virtual NumberIntervalLattice neg(NumberIntervalLattice& e2)=0;
  virtual NumberIntervalLattice intValue(NumberIntervalLattice& e0)=0;
};

#endif
