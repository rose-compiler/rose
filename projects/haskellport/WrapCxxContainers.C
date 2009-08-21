#include <list>
#include <vector>
#include <WrapCxxContainers.h>

using namespace std;

extern "C" {

#if 0
list<void *> *_haskell_new_ptr_list(void)
   {
     return new list<void *>;
   }

vector<void *> *_haskell_new_ptr_vector(void)
   {
     return new vector<void *>;
   }
#endif

#define CxxContainer_INSTANCE(ctrtype, ctrname) \
ctrtype *_haskell_new_##ctrname(ctrtype::value_type *elems, size_t length) \
   { \
     return _haskell_new_ctr<ctrtype>(elems, length); \
   } \
\
void _haskell_delete_##ctrname(ctrtype *ctr) \
   { \
     delete ctr; \
   } \
\
ctrtype::value_type *_haskell_get_##ctrname##_elems(ctrtype *ctr, size_t *length) \
   { \
     return _haskell_get_ctr_elems(ctr, length); \
   }

CxxContainer_INSTANCE(list<void *>, ptr_list)
CxxContainer_INSTANCE(vector<void *>, ptr_vector)
CxxContainer_INSTANCE(vector<int>, int_vector)
CxxContainer_INSTANCE(vector<unsigned long>, ulong_vector)
CxxContainer_INSTANCE(vector<unsigned long long>, ull_vector)
CxxContainer_INSTANCE(vector<unsigned char>, uchar_vector)
CxxContainer_INSTANCE(vector<unsigned int>, uint_vector)

}

