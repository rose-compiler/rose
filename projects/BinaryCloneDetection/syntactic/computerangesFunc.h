#ifndef COMPUTERANGESFUNC_H
#define COMPUTERANGESFUNC_H

#include <vector>

struct Range {
  int id;
  long low, high;

  Range(int id, long low, long high): id(id), low(low), high(high) {}
  Range(): id(0), low(0), high(0) {}

  bool contains(long n) const {
    return n >= low && (n <= high || high == -1);
  }
};

std::vector<Range> computeranges (double d, long lo, long hi);

#endif // COMPUTERANGESFUNC_H
