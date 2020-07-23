void method1() {
  // This fails to be handled properly because the header is used twice (leave for later).
     #include "Inner.h"
     v1_rename_me = 10;
}

void method2() {
	#include "Inner.h"
	v1_rename_me = 15;
}

int main(int argc, char* argv[]) {

}
