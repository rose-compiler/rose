#include <bits/c++config.h>
#include <bits/c++config.h>
#include <locale.h>
#include <stddef.h>

#include<stdio.h>

#include <string>

int main()
   {
     printf ("__cplusplus         = %zu \n",__cplusplus);

#ifdef __clang__
     printf ("__clang__           = %d  \n",__clang__);
#else
     printf ("Not a clang compiler \n");
#endif

#ifdef __INTEL_COMPILER
     printf ("__INTEL_COMPILER    = %d  \n",__INTEL_COMPILER);
#else
     printf ("Not an Intel compiler \n");
#endif

#ifdef GLIBCXX_BEGIN_NAMESPACE
     printf ("GLIBCXX_BEGIN_NAMESPACE IS defined \n");
#else
     printf ("GLIBCXX_BEGIN_NAMESPACE is NOT defined \n");
#endif
     printf ("__GNUC__            = %d  \n",__GNUC__);
     printf ("__GNUC_MINOR__      = %d  \n",__GNUC_MINOR__);
     printf ("__GNUC_PATCHLEVEL__ = %d  \n",__GNUC_PATCHLEVEL__);
     printf ("_GLIBCXX_CONSTEXPR  = %s  \n","_GLIBCXX_CONSTEXPR");

#if defined(__EDG_VERSION__)
     printf ("__EDG_VERSION__     = %d \n",__EDG_VERSION__);
#else
     printf ("__EDG_VERSION__ is not defined! \n");
#endif

#if defined(ROSE_CONSTEXPR)
  // std::string s = {ROSE_CONSTEXPR};
  // printf ("ROSE_CONSTEXPR      = %s \n","ROSE_CONSTEXPR");
     printf ("ROSE_CONSTEXPR IS defined! \n");
#else
     printf ("ROSE_CONSTEXPR is NOT defined! \n");
#endif
   }
