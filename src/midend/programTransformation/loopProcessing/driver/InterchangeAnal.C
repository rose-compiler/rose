
#include <CompSliceLocality.h>
#include <InterchangeAnal.h>
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

#define MAXDEPTH 20

float SliceNestingLevel( const CompSlice *slice)
{
  int level = 0, num = 0;
  CompSlice::ConstLoopIterator iter=slice->GetConstLoopIterator();
  for ( LoopTreeNode *n; (n = iter.Current()); iter++) {
    level += n->LoopLevel();
    num++;
  }
  return level*1.0 / num;
}

void ArrangeNestingOrder ::
RearrangeSliceNest( CompSliceNest &g, float *weightvec)
{
  int num = g.NumberOfEntries();
  assert( num < MAXDEPTH);
  for ( int j = 0; j < num; ++j) {
     // const CompSlice *slice = g[j];
     float w = weightvec[j];
     int index = j;
     for ( int k = j; k < num ; ++k) {
        // const CompSlice *slice1 = g[k];
        float w1 = weightvec[k];
        if ( w1 < w) {
           index = k;
           w = w1;
        }
     }
     if (index != j) {
       weightvec[index] = weightvec[j];
       weightvec[j] = w;
       g.SwapEntry(j,index);
     }
  }
}

void ArrangeNestingOrder :: operator()( CompSliceLocalityRegistry *anal,CompSliceNest& g)
{
  float weightvec[MAXDEPTH];
  SetNestingWeight( anal, g, weightvec );
  RearrangeSliceNest( g, weightvec );
}

void ArrangeOrigNestingOrder ::
SetNestingWeight( CompSliceLocalityRegistry *anal, CompSliceNest &g, float *weightvec )
{
   for (int i = 0; i < g.NumberOfEntries(); i++) {
      weightvec[i] = SliceNestingLevel(g[i]);
   }
}

void ArrangeReuseOrder ::
SetNestingWeight( CompSliceLocalityRegistry *anal, CompSliceNest &g, float *weightvec )
{
  for (int i = 0; i < g.NumberOfEntries(); i++) {
     weightvec[i] = anal->TemporaryReuses( g[i] );
     weightvec[i] += anal->SpatialReuses( g[i] );
  }
}

