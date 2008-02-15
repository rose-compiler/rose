// cc.in75
// need to relax checking between decl and defn for array types

extern int arr[];

int arr[3] = {1,2,3};


extern int blah[][4];

int blah[5][4] = { 0 };


extern int borg[3];
//ERROR(1): int borg[4] = { 7 };   // mismatch

class Foo {
public:
  int f(int arr2[]);
};

int Foo::f(int arr2[5])
{
  return 6;
}
