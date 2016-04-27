#include <bits/c++config.h>
#include <bits/c++config.h>
#include <locale.h>
#include <stddef.h>

#include<stdio.h>

int main()
   {
     printf ("__cplusplus         = %zu \n",__cplusplus);
#ifdef __clang__
     printf ("__clang__           = %d  \n",__clang__);
#else
     printf ("Not a clang compiler \n");
#endif
#ifdef GLIBCXX_BEGIN_NAMESPACE
     printf ("GLIBCXX_BEGIN_NAMESPACE IS defined \n");
#else
     printf ("GLIBCXX_BEGIN_NAMESPACE is NOT defined \n");
#endif
     printf ("__GNUC__            = %d  \n",__GNUC__);
     printf ("__GNUC_MINOR__      = %d  \n",__GNUC_MINOR__);
     printf ("__GNUC_PATCHLEVEL__ = %d  \n",__GNUC_PATCHLEVEL__);
   }
