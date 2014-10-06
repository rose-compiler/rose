#ifndef ROSE_override_H
#define ROSE_override_H

#if defined(__STDC__)
# if __STDC_VERSION__ == 201112l
#   define ROSE_OVERRIDE override
# else
#   define ROSE_OVERRIDE /*override*/
# endif
#else
# define ROSE_OVERRIDE /*override*/
#endif

#endif
