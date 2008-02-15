// 13.3.1.1.2.cc

int f1(int);
int f2(float);
typedef int (*fp1)(int);
typedef int (*fp2)(float);
struct A {
    operator fp1() { return f1; }
    operator fp2() { return f2; }
} a;

// TODO: this does not work because I don't try implicit conversions
// in this context
//int i = a(1);                        // Calls f1 via pointer returned from
                                     // conversion function
