#include <vector>
#include <stdlib.h>

template <class CT>
typename CT::value_type *_haskell_get_ctr_elems(CT *ctr, size_t *length)
   {
     size_t ctrSize = ctr->size();
     *length = ctrSize;
     typename CT::value_type *elems = static_cast<typename CT::value_type *>(malloc(ctrSize * sizeof(typename CT::value_type)));
     size_t ctrPos = 0;
     for (typename CT::iterator i = ctr->begin(); i != ctr->end(); ++i, ++ctrPos)
        {
          elems[ctrPos] = *i;
        }
     return elems;
   }

template <class CT>
CT *_haskell_new_ctr(typename CT::value_type *elems, size_t length)
   {
     return new CT(elems, elems+length);
   }

extern "C" {

void **_haskell_get_ptr_vector_elems(std::vector<void *> *vec, size_t *length);
int *_haskell_get_int_vector_elems(std::vector<int> *vec, size_t *length);

}
