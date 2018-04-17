#define HYPRE_SMP_PRIVATE HYPRE_BOX_SMP_PRIVATE,hypre__nx,hypre__ny,hypre__nz
#ifdef HYPRE_USING_OPENMP
#ifndef HYPRE_SMP_REDUCTION_OP
#pragma omp parallel for private(HYPRE_SMP_PRIVATE) schedule(static)
#endif
#ifdef HYPRE_SMP_REDUCTION_OP
#pragma omp parallel for private(HYPRE_SMP_PRIVATE) \
  reduction(HYPRE_SMP_REDUCTION_OP: HYPRE_SMP_REDUCTION_VARS) \
schedule(static)
#endif
#endif
#undef HYPRE_BOX_SMP_PRIVATE
