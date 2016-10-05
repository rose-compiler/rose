// This example comes from: http://www.gotw.ca/publications/mill17.htm

// Example 2: Explicit specialization 
// 
template<class T> // (a) a base template 
void f( T );

template<class T> // (b) a second base template, overloads (a) 
void f( T* );     //     (function templates can't be partially 
                  //     specialized; they overload instead)

template<>        // (c) explicit specialization of (b) 
void f<>(int*);

// ...

void foobar()
   {
int *p; 
f( p );           // calls (c)
   }


