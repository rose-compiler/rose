
#ifndef COND_COMPSLICE
#define COND_COMPSLICE

#include <CompSlice.h>

class DynamicCompSlice  : public CompSlice
{
 public:
  DynamicCompSlice( int level);
  DynamicCompSlice( const DynamicCompSlice& that);
  ~DynamicCompSlice();

  unsigned QuerySliceStmtGroupIndex( const LoopTreeNode *s) const;
  unsigned QuerySliceGroupNumber() const;
};

class DynamicSlicing : public DependenceHoisting
{
  virtual CompSlice* CreateCompSlice( int level) { return new DynamicCompSlice(level); }
 public:
  virtual LoopTreeNode* Transform( LoopTreeDepComp& c,
                                   const CompSlice *slice, LoopTreeNode *root);
};

#endif
