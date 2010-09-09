
#include <general.h>

#include <stdlib.h>
#include <assert.h>

#include <DepEDD.h>

inline void CopyVec( DepRel *vec1, DepRel *vec2, unsigned num)
{
  for (unsigned i = 0; i < num; i++)
    vec1[i] = vec2[i];
}

DepEDD :: DepEDD(unsigned _nr, unsigned _nc, unsigned spare)
{
  nr = _nr;
  nc = _nc;
  num = (nr+spare) * (nc+spare);
  if (num == 0)
    vec = 0; 
  else 
    vec = new DepRel[num];
}

void DepEDD :: Initialize (const DepRel &init)
{
  for (unsigned int i = 0; i < num; i++)
      vec[i] = init;
}

DepEDD :: DepEDD( const DepEDD &that)
{
  nr = that.nr;
  nc = that.nc;
  num = that.num;
  if (num == 0) {
    vec = 0;
  }
  else {
    vec = new DepRel[num];
    CopyVec(vec, that.vec, num);
  }
}

void DepEDD :: operator = (const DepEDD &that)
{
  nr = that.nr;
  nc = that.nc;
  if (num < that.num) {
    delete [] vec;
    num = that.num;
    vec = new DepRel[num];
  }
  if (that.num > 0) {
    CopyVec(vec, that.vec, that.num);
  }
}

DepEDD ::  ~DepEDD()
{
  if (vec != 0)
    delete [] vec;
}

DepRel& DepEDD :: Entry( unsigned index1, unsigned index2) const
{
  unsigned index = index1 * nc + index2;
  assert(index < num);
  return vec[index];
}

void DepEDD :: Reset( unsigned index1, unsigned index2)
{
  unsigned num1 = index1 * index2;
  nr = index1;
  nc = index2;
  if (num < num1) {
    delete [] vec;
    num = num1;
    vec = new DepRel[num];
  }
}







