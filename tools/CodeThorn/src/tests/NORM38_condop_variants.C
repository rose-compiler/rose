int abs1(int __x) { return __x >= 0 ? __x : -__x; }
int abs2(int __x) { int tmp; tmp=__x >= 0 ? __x : -__x; return tmp; }
int abs3(int __x) { int tmp; __x >= 0 ? __x : -__x; return tmp; }
int abs4(int __x) { int tmp; __x >= 0 ? __x : __x; return tmp; }
void f1() { int tmp; int __x; if(__x >= 0 ? __x : __x) { tmp=1;} }
void f2() { int __x; if(__x >= 0 ? __x : __x) { } } 
int f3(int __x) { return __x >= 0 ? __x : -__x; }
int f4(int __x) { return __x >= 0 ? __x+1 : -__x; }
int f5() { int __x; return __x >= 0 ? __x : -__x; }
void f6() { int __x; __x >= 0 ? __x+1 : __x+1; }
int main() { return 0; }
