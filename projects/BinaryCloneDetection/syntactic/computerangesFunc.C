
/* This code is used to compute the grouping ranges for vectors.
 * It does not need a vector file as inputs; its usage:
 * ./thisfile <distance> <lowerbound> <upperbound> [flag-output_range]
 *
 * So, it only generates a set of ranges. Pls use another code to dispatch
 * the vectors so it's more flexible to how merge certain ranges together.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <regex.h>
#include <math.h>
#include <vector>
#include "computerangesFunc.h"

using namespace std;

const double epsilon = 1e-6;

const int max_num_ranges=1024*1024*1024; // could the size of a vector be so huge?

vector<Range> computeranges (double d, long lo, long hi)
{
  double c;
  vector<long> lows, highs;
  long ri = 0;
  int i;

  assert( d>=0.0 );
  assert( lo>=1 ); // 0 is meaningless for vectors
  assert( hi>=lo );

  if ( fabs(d)<epsilon )
    c = -1;
  else
    c = d;
  
  /* the first range: */
  lows.push_back(0);
  highs.push_back((long)ceil(lo+d)); // "ceil" is more coservative than "floor"; if ranges are always integers, "floor" should be better.
  ri = 1; // assume ( max_num_ranges>2 );
  while ( ri<max_num_ranges && lows[ri-1]<=hi ) {
    /* compute the range for a new group */
    if ( ri >= max_num_ranges-1 ) {
      fprintf(stderr, "ERR: Reach %d ranges. The last range is set to include all rest sizes.\n", max_num_ranges);
      if ( c<0 )
        lows.push_back((long)ceil(highs[ri-1]-c));
      else
	lows.push_back((long)ceil(highs[ri-1]-c*lows[ri-1]/lo));
    } else {
      /* enlarge the arrays if they are too small: */
      if ( ri == 1 ) {
	/* begin the second range: */
	if ( c<0 ) {
	  /* do not to worry about false clone transitions or scalable "c": */
	  lows.push_back((long)ceil(lo+d-c));
	  highs.push_back((long)ceil((lo+d)*lows[ri]/lo));
	} else {
	  lows.push_back((long)ceil(lo+d-c));
	  highs.push_back((long)ceil(lo+2*d+d*d/lo-c*c/lo+c+1));
	}
      } else {
	if ( c<0 ) {
	  lows.push_back((long)ceil(highs[ri-1]-c));
	  highs.push_back((long)ceil((lo+d)*lows[ri]/lo));
	} else {
	  lows.push_back((long)ceil(highs[ri-1]-c*lows[ri-1]/lo));
	  highs.push_back((long)ceil((lo+d+c)*highs[ri-1]/lo-(d*c+c*c)*lows[ri-1]/lo/lo+1));
	}
      }
      ri++;
    }
  }
  highs[ri-1] = -1; // "-1" means all rest sizes

  vector<Range> ranges(lows.size());
  for ( i=0; i<ri; i++) {
    ranges[i] = Range(i, lows[i], highs[i]);
  }
  return ranges;
}

