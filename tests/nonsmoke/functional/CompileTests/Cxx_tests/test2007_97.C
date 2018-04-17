/*
When compiling the attached files in ROSE I get the following error:
                             File-Name:
/home/andreas/REPOSITORY-BUILD/gcc-4.1.1/mozilla/xpcom/components/test.cpp
/home/andreas/links/g++-411: line 2:  4359 Floating point
exception/home/andreas/REPOSITORY-BUILD/gcc-4.1.1/ROSE/June-29a-2007-Unsafe/tutorial/identityTranslator
"$@"

It is possible to change this code so that it segfaults.

Andreas 
*/

#include "test2007_97.h"

int
nsCategoryManager::SuppressNotifications(bool aSuppress)
{
  return 0;
}

class nsCategoryManagerFactory 
   {
     public:
       nsCategoryManagerFactory() { }
  virtual int __attribute__ ((regparm (0), cdecl)) QueryInterface(     
                           );

   };

