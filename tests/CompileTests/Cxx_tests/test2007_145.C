/*
Bug name:       runon-#define

Reported by:    Brian White

Date:           Sept 28, 2007

Date sent:      Sep 30, 2007

E-mail subject: more SPEC 2006 bugs

Occurs in:      SPEC CPU 2006 453.povray/src/fnpovfpu.cpp

Brief description:      The character \ is used to allow for continued
                        lines in #defines.  g++ allows a #define to
                        "end" in a \.  e.g., here are two #defines:

                        #define CMP(x,y) \
                          x == y \

                        #define NEQ(x,y) \
                          x != y

                        The empty line between the two #defines is
                        significant.

                        ROSE omits this empty line to output

                        #define CMP(x,y) \
                          x == y \
                        #define NEQ(x,y) \
                          x != y

                        and the error

rose_bug.cc:3:2: '#' is not followed by a macro parameter

Files
-----
bug.cc          The two #defines above; the first being a run-on.


rose_bug.cc     Output from ROSE backend for the two #defines.  As
                shown above, it does not retain the empty line
                separating the #defines.
*/

#define CMP(x,y) \
  x == y \

#define NEQ(x,y) \
  x != y
