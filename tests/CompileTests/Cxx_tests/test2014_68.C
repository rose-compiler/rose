
#if 1

namespace XXX {

template <typename TYPE> void coarsenNodalByInjection(TYPE** fx, TYPE** cx, const bool coarsen_slides );

void injectCoarsenNodal(
   int* fine_data,
   int* coarse_data
   ) 
   {
     coarsenNodalByInjection<int>(&fine_data, &coarse_data, false );
   }

template <typename TYPE>
void coarsenNodalByInjection(
   TYPE** fx,
   TYPE** cx,
   const bool coarsen_slides )
   {
     int r[3] = {1,1,1};
   }

}
#endif

