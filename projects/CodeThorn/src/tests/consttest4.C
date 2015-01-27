void foo(int a[1]); // size is irrelevant
void bar(int(&a)[1]); // size is irrelevant
void baz(int(*a)[1]); // size is irrelevant

namespace
{
  const unsigned array_size = 3;
  const unsigned array_size2 = 2;
}

/*
C++ '03 8.3.5/3: ...The type of a function is determined using the
    following rules. The type of each parameter is determined from its
    own decl-specifier-seq and declarator. After determining the type
    of each parameter, any parameter of type "array of T" or "function
    returning T" is adjusted to be "pointer to T" or "pointer to
    function returning T," respectively....
*/

void foo(int a[array_size]) { // decays to a pointer and becomes int* (C inheritage)
  a[0]=a[1]+1;
} 

void bar(int(&a)[array_size2]) { // size is enforced
  a[0]=a[1]+1;
} 

void baz(int(*a)[array_size2]) { // size is enforced
  (*a)[0]=(*a)[1]+1;  
}

int main() {
  int a[2];
  a[1]=1;
  foo(a);
  bar(a);
  baz(&a);
  return 0;
}
