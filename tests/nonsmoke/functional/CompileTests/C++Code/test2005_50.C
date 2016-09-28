
#include <stdio.h>

// this shows up in header files and must work since header files can't 
// in /usr/include can't be modified!
// __extension__ struct __gconv_step_data __data __flexarr;


int main()
   {
     printf ("__cplusplus is %s \n",(__cplusplus != 0) ? "defined" : "NOT defined");

     printf ("__USE_BSD is %s \n",(__USE_BSD != 0) ? "defined" : "NOT defined");
     printf ("__USE_ISOC99 is %s \n",(__USE_ISOC99 != 0) ? "defined" : "NOT defined");
     printf ("__USE_UNIX98 is %s \n",(__USE_UNIX98 != 0) ? "defined" : "NOT defined");

// this is defined for g++ 3.4.x but not for g++ 3.3.x  so we have to test it only where it is defined
#ifdef _GLIBCXX_HAVE_INT64
     int64_t x;
#endif

//   printf ("_GLIBCXX_HAVE_INT64 is %s \n",(_GLIBCXX_HAVE_INT64 != 0) ? "defined" : "NOT defined");
//   printf ("_GLIBCXX_HAVE_STDINT_H is %s \n",(_GLIBCXX_HAVE_STDINT_H != 0) ? "defined" : "NOT defined");

  // printf (" is %s \n",( != 0) ? "defined" : "NOT defined");

#ifdef __GNUG__
     printf ("__GNUG__ = %d \n",__GNUG__);
#else
     printf ("__GNUG__ is not defined \n");
#endif

#ifdef __GNUC__
     printf ("__GNUC__ = %d \n",__GNUC__);
#else
     printf ("__GNUC__ is not defined \n");
#endif

#ifdef __GNUC_MINOR__
     printf ("__GNUC_MINOR__ = %d \n",__GNUC_MINOR__);
#else
     printf ("__GNUC_MINOR__ is not defined \n");
#endif

#ifdef __GNUC_PREREQ
     printf ("__GNUC_PREREQ(2,7)  = %d \n",__GNUC_PREREQ(2,7));
     printf ("__GNUC_PREREQ(2,8)  = %d \n",__GNUC_PREREQ(2,8));
     printf ("__GNUC_PREREQ(2,9)  = %d \n",__GNUC_PREREQ(2,9));
     printf ("__GNUC_PREREQ(2,95) = %d \n",__GNUC_PREREQ(2,95));
     printf ("__GNUC_PREREQ(2,97) = %d \n",__GNUC_PREREQ(2,97));
     printf ("__GNUC_PREREQ(3,0)  = %d \n",__GNUC_PREREQ(3,0));
     printf ("__GNUC_PREREQ(3,1)  = %d \n",__GNUC_PREREQ(3,1));
     printf ("__GNUC_PREREQ(3,2)  = %d \n",__GNUC_PREREQ(3,2));
     printf ("__GNUC_PREREQ(3,3)  = %d \n",__GNUC_PREREQ(3,3));
     printf ("__GNUC_PREREQ(3,4)  = %d \n",__GNUC_PREREQ(3,4));
     printf ("__GNUC_PREREQ(3,5)  = %d \n",__GNUC_PREREQ(3,5));
     printf ("__GNUC_PREREQ(4,0)  = %d \n",__GNUC_PREREQ(4,0));
     printf ("__GNUC_PREREQ(4,1)  = %d \n",__GNUC_PREREQ(4,1));
#endif

     return 0;
   }

