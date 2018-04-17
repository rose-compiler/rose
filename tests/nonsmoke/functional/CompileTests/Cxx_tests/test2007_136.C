/*
Bug name:       include-outside-of-namespace

Reported by:    Brian White

Date:           Sept 29, 2007

Date sent:      Sept 30, 2007

E-mail subject: more SPEC 2006 bugs

Occurs in:      SPEC CPU 2006   483.xalancbmk/src/LinuxPlatformUtils.cpp

Brief description:      A #include within the context of a namespace
                        is emitted outside that namespace if 
                        the namespace (of the same name) 
                        had been previously opened and closed, as in:
                        
                        namespace XERCES_CPP_NAMESPACE { }

                        namespace XERCES_CPP_NAMESPACE {

                          #include "bar.c"

                        }


Files
-----
bug.cc          The above example.

rose_bug.cc     The output from the backend for bug.cc.  In it, we can
                see that the #include is outside the namespace:

                namespace XERCES_CPP_NAMESPACE
                {
                }
                namespace XERCES_CPP_NAMESPACE
                {
                }
                #include "bar.c"

*/

namespace XERCES_CPP_NAMESPACE { }

namespace XERCES_CPP_NAMESPACE {

#include "test2007_136.h"

}
