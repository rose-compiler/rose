
#include "CodeThornOptions.h"

bool CodeThornOptions::activeOptionsRequireZ3Library() {
  return z3BasedReachabilityAnalysis;
}
