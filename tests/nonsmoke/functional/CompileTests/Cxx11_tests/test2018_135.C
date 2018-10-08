// ROSE-1206 / RZ ROSE-31
// This version works for g++, but fails for ROSE.

#include <xmmintrin.h>
void
func1 () {
  _mm_set_ps(0.0, 0.0, 0.0, 0.0);
  _mm_set_ps1(0.0);
}

