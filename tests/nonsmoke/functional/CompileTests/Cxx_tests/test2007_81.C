/* 
Email from Andreas:

when compiling the attached file with ROSE I get the following error:

"/home/andreas/REPOSITORY-BUILD/gcc41/mozilla/js/src/test-clz.c", line
4: warning:
         variable "j_" is used before its value is set
  __builtin_clz(j_ - 1);
                ^

"/home/andreas/REPOSITORY-BUILD/gcc41/mozilla/js/src/test-clz.c", line
4: error:
         identifier "__builtin_clz" is undefined
  __builtin_clz(j_ - 1);
  ^ 

*/


int main(){
 unsigned int j_ ;
 
 __builtin_clz(j_ - 1);
};
