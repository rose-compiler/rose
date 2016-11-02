// This example comes from: http://www.gotw.ca/publications/mill17.htm

// Consider now the following code, put in this form by Peter Dimov and Dave Abrahams:

// Example 3: The Dimov/Abrahams Example 
// 
template<class T> // (a) same old base template as before 
void f( T );

template<>        // (c) explicit specialization, this time of (a)
void f<>(int*);

template<class T> // (b) a second base template, overloads (a) 
void f( T* );

// ...

void foobar()
   {
int *p; 
f( p );           // calls (b)! overload resolution ignores 
                  // specializations and operates on the base 
                  // function templates only
   }

// If this surprises you, you're not alone; it has surprised a lot of experts in its time. The key to understanding this is simple, and here it is: Specializations don't overload.
// Only the base templates overload (along with nontemplate functions, of course). Consider again the salient part from the summary I gave above of the overload resolution rules, this time with specific words highlighted:

