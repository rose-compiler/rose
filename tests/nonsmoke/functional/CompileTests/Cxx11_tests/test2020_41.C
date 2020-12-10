
struct A
   {
     int foo(char i) { return int(i)*1; }
     int foo(int i) { return int(i)*2; }
     int foo(long i) { return int(i)*3; }
     int foo(double i) { return int(i)*4; }
     int foo(char *i) { return int(*i)*5; }
     int foo(int i, double d) { return int(i)*double(d)*6; }
   };

typedef int (A::*PFC_)(char);
typedef int (A::*PFI_)(int);
typedef int (A::*PFL_)(long);
typedef int (A::*PFD_)(double);
typedef int (A::*PFPC_)(char*);
typedef int (A::*PFID_)(int, double);

void foobar()
   {
     A a;
     (a.*PFC_(&A::foo))(1);
     (a.*PFI_(&A::foo))(1);
     (a.*PFL_(&A::foo))(1);
     (a.*PFD_(&A::foo))(1);
     static char s0[] = "0";
     (a.*PFPC_(&A::foo))(s0);
     (a.*PFID_(&A::foo))(1, 2);
   }

