
#include "data-manager.hpp"

#include <cassert>

namespace DataManager {

float * init(unsigned dim, unsigned long * size_) {
  assert(dim > 0);
  
  unsigned long size = 1;
  for (unsigned i = 0; i < dim; i++)
    size *= size_[i];

  return new float[size];
}

float * load(unsigned dim, unsigned long * size, char * filename) {
  float * res = init(dim, size);

  // TODO
  
  return res;
}

float compare(unsigned dim, unsigned long * size, float * a, float * b) {
  // TODO

  return 0.;
}

}
