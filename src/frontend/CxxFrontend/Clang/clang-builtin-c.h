
// FIXME Clang know __builtin_va_list but not __builtin_va_start and __builtin_va_end, why?

// Note that this permits the generated graps to be as small as possible where
// the input code is defined using -DSKIP_ROSE_BUILTIN_DECLARATIONS
#ifndef SKIP_ROSE_BUILTIN_DECLARATIONS

#if 1
void * __builtin_va_start(__builtin_va_list, ...);
void * __builtin_va_end(__builtin_va_list);
#endif

#endif
