#include <stdio.h>

#include <boost/preprocessor/slot/slot.hpp>
#include <boost/preprocessor/stringize.hpp>

#define X() 4

#define BOOST_PP_VALUE 1 + 2 + 3 + X()
#include BOOST_PP_ASSIGN_SLOT(1)

#undef X

int main(int argc, char** argv) {
   printf("%s\n", BOOST_PP_STRINGIZE(BOOST_PP_SLOT(1)));
   return 0;
}