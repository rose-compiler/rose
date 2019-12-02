// Includes definition of class Simple (includes Inner_test8.h using < >) test8/subdir/Inner_test8.h has int v1;
// include <> searches system directories before searching directories specified using -I
// So it finds the test8/subdir/Inner_test8.h
// #include "Simple.h"

// Includes definition of class Simple2 (includes Inner_test8.h using " ") test8/Inner_test8.h has int v2_rename_me;
// inlcude "" searches the current directory and then any specified inlcude paths using -I
// So it finds the test8/Inner_test8.h
#include "Simple2.h"

#if 0
Simple::Simple(){
  v1 = 15;
}
#endif

#if 1
Simple2::Simple2(){
  v2_rename_me = 155;
}
#endif

int main(int argc, char* argv[]) {

}
