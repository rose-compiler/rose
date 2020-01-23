#ifndef ROSE_override_H
#define ROSE_override_H

#ifdef __cplusplus
#   if __cplusplus >= 201103l
#       define ROSE_OVERRIDE override
#       define ROSE_FINAL final
#   else
#       define ROSE_OVERRIDE /*override*/
#       define ROSE_FINAL /*final*/
#   endif
#else
#   define ROSE_OVERRIDE /*override*/
#   define ROSE_FINAL /*final*/
#endif

#endif
