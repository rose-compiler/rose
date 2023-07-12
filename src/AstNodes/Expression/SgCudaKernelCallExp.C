#include <sage3basic.h>

void
SgCudaKernelCallExp::post_construction_initialization()
   {
     if (p_function != NULL)
          p_function->set_parent(this);

     // TODO : exec_config
   }

SgType*
SgCudaKernelCallExp::get_type() const
   {
#if 0
     printf ("In SgCudaKernelCallExp::get_type() \n");
#endif

     return SgFunctionCallExp::get_type();
   }
