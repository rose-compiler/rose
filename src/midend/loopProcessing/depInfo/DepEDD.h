
#ifndef DEP_EDD_INFO
#define DEP_EDD_INFO

#include <DepRel.h>
//!Extended dependence matrix: 
// each element of the matrix is a dependence direction relationship: 
class DepEDD 
{
  unsigned nr, nc, num;
  DepRel* vec;
  unsigned GetLength() const { return num; }
 public:
  DepEDD() { nr = nc = num = 0; vec = 0; }
  DepEDD(unsigned _nr, unsigned _nc, unsigned _spare = 1);
  DepEDD(const DepEDD &that);
  void operator = (const DepEDD &that);
  ~DepEDD();

  unsigned SnkDim() const { return nc; }
  unsigned SrcDim() const { return nr; }
  DepRel& Entry(unsigned loop1, unsigned loop2) const;

  void Reset(unsigned _nr, unsigned _nc);
  void Initialize(const DepRel &init);
};

#endif

