/*!
 * Calculate data type sizes for UPC types, with customized base type sizes
 *
 * Liao 8/7/2008
 */
#include "rose.h"
#include <iostream>

using namespace std;
using namespace SageInterface;

StructCustomizedSizes upc_sizes;

void initUpcSizes()
{
    upc_sizes.str_abi="x86"; // default to x86 (i386, ILP32)
    upc_sizes.sz_shared_ptr= 8;
    upc_sizes.sz_alignof_shared_ptr= 4;
    upc_sizes.sz_pshared_ptr= 8;
    upc_sizes.sz_alignof_pshared_ptr= 4;
    upc_sizes.sz_mem_handle= 4;
    upc_sizes.sz_alignof_mem_handle= 4;
    upc_sizes.sz_reg_handle= 4;
    upc_sizes.sz_alignof_reg_handle=4;
    upc_sizes.sz_void_ptr= 4;
    upc_sizes.sz_alignof_void_ptr= 4;
    upc_sizes.sz_ptrdiff_t= 4;
    upc_sizes.sz_alignof_ptrdiff_t=4;

    // implied by ABI, redundant here actually
    upc_sizes.sz_char=1;
    upc_sizes.sz_alignof_char=1;
    upc_sizes.sz_int=4;
    upc_sizes.sz_alignof_int=4;
    upc_sizes.sz_short=2;
    upc_sizes.sz_alignof_short=2;
    upc_sizes.sz_long=4;
    upc_sizes.sz_alignof_long=4;
    upc_sizes.sz_longlong=8;
    upc_sizes.sz_alignof_longlong=4;
    upc_sizes.sz_float=4;
    upc_sizes.sz_alignof_float=8;
    upc_sizes.sz_double=8;
    upc_sizes.sz_alignof_double=4;
    upc_sizes.sz_longdouble=12;
    upc_sizes.sz_alignof_longdouble=4;
 
    upc_sizes.sz_size_t=4;
    upc_sizes.sz_alignof_size_t=4;
    upc_sizes.sz_alignof_dbl_1st=0;
    upc_sizes.sz_alignof_int64_1st=0;
    upc_sizes.sz_alignof_sharedptr_1st =0;
    upc_sizes.sz_alignof_psharedptr_1st =0;
    upc_sizes.sz_alignof_dbl_innerstruct=0;
    upc_sizes.sz_alignof_int64_innerstruct=0;
    upc_sizes.sz_alignof_sharedptr_innerstruct =0;
    upc_sizes.sz_alignof_psharedptr_innerstruct= 0;
    upc_sizes.sz_maxblocksz= 4194304;
}

int main(int argc, char** argv) {

   SgProject* proj = frontend(argc, argv);

  // Set up the struct layout chain
  initUpcSizes(); 
  CustomizedPrimitiveTypeLayoutGenerator gen1_upc(NULL,&upc_sizes);
  NonpackedTypeLayoutGenerator gen_upc(&gen1_upc);

  // Process every type used in a variable or parameter declaration
  vector<SgNode*> initNames = NodeQuery::querySubTree(proj, V_SgInitializedName);
  for (size_t i = 0; i < initNames.size(); ++i) {
    SgInitializedName* in = isSgInitializedName(initNames[i]);
    SgType* t = in->get_type();
    if (isSgTypeEllipse(t)) continue;
    cout << in->get_name().getString() << " has type " << t->unparseToString() << ":\n";
    cout << "For a customized UPC platform:\n";
    cout << gen_upc.layoutType(t) << "\n";
    size_t size = gen_upc.layoutType(t).size;
    size_t element_size=size;
    if (isSgArrayType(t)) 
      element_size = gen_upc.layoutType(
                SageInterface::getArrayElementType(isSgArrayType(t))).size;

#if 0
    if (isSgArrayType(t))
    {
      cout<<"Found an array, output its element type info."<<endl;
      cout<< gen_upc.layoutType(SageInterface::getArrayElementType(isSgArrayType(t)))<<endl;
      // Array of shared UPC elements

    }
#endif    
    if (isUpcSharedType(t))
    { 
      size_t block_bytes = SageInterface::getUpcSharedBlockSize(t);
      if (!isSgArrayType(t) || block_bytes == 0)
        { block_bytes = size; }
      else
        {block_bytes = min (block_bytes*element_size, size);}
      size_t num_blocks = (size % block_bytes==0)?(size/block_bytes):size/block_bytes +1;
      int hasThreads=0;
      if (isSgArrayType(t))
        if (isUpcArrayWithThreads(isSgArrayType(t)))
          hasThreads =1;

      cout<<"Found a shared UPC type: block bytes="<<block_bytes
          <<" Number of block="<<num_blocks
          <<" Multiply by THREADS="<<hasThreads
          <<" Element size="<<element_size
          <<endl;
    } // UPC shared types

  } // end for
  return 0;
}
