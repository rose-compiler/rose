#include <rose.h>
#include <vector>
#include <WrapCxxContainers.h>

using namespace std;

struct FillVectorRefToPtrHandler : ReferenceToPointerHandlerImpl<FillVectorRefToPtrHandler>
   {
     vector<void **> &ptrs;
     vector<int> &variants;

     FillVectorRefToPtrHandler(vector<void **> &ptrs, vector<int> &variants) : ptrs(ptrs), variants(variants) {}

     template<class NodeType>
     void genericApply(NodeType *&node, const SgName &name, bool traverse)
        {
          if (traverse)
             {
               ptrs.push_back(reinterpret_cast<void **>(&node));
               variants.push_back(NodeType::static_variant);
             }
        }

   };

extern "C" {

// TODO: remove
int _haskell_variant(SgNode *n)
   {
     return n->variantT();
   }

size_t _haskell_get_traversal_successor_ptrs(SgNode *n, void ****ptrs, int **variants)
   {
     size_t length;
     vector<void **> vPtrs;
     vector<int> vVariants;

     FillVectorRefToPtrHandler rtph(vPtrs, vVariants);
     n->processDataMemberReferenceToPointers(&rtph);

     *ptrs = _haskell_get_ctr_elems(&vPtrs, &length);
     *variants = _haskell_get_int_vector_elems(&vVariants, &length);

#if 0
     printf("C %p:\n", n);

     for (int i = 0; i < length; ++i)
        {
          printf("C   %p -> %p (%d)\n", (*ptrs)[i], *((*ptrs)[i]), (*variants)[i]);
        }
#endif

     return length;
   }

}
