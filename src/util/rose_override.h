#ifndef ROSE_override_H
#define ROSE_override_H

#ifdef __cplusplus
#   if __cplusplus == 201103l
#       define ROSE_OVERRIDE override
#   else
#       define ROSE_OVERRIDE /*override*/
#   endif
#else
#   define ROSE_OVERRIDE /*override*/
#endif

#endif
