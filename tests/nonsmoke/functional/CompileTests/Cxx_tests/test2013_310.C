// This example comes from: http://www.gotw.ca/publications/mill17.htm

// Example 1: Class vs. function template, and overloading 
//

// A class template
template<class T> class X { /*...*/ };      // (a)

// A function template with two overloads
template<class T> void f( T );              // (b)
template<class T> void f( int, T, double ); // (c)

// These unspecialized templates are also called the underlying base templates.

// Further, base templates can be specialized. This is where class templates and function templates diverge further, in ways that will become important later in this article. A class template can be partially specialized and/or fully specialized.[1] A function template can only be fully specialized, but because function templates can overload we can get nearly the same effect via overloading that we could have got via partial specialization. The following code illustrates these differences:

// Example 1, continued: Specializing templates 
//

// A partial specialization of (a) for pointer types 
template<class T> class X<T*> { /*...*/ };

// A full specialization of (a) for int 
template<> class X<int> { /*...*/ };

// A separate base template that overloads (b) and (c) 
// -- NOT a partial specialization of (b), because 
// there's no such thing as a partial specialization 
// of a function template! 
template<class T> void f( T* );             // (d)

// A full specialization of (b) for int 
template<> void f<int>( int );              // (e)

// A plain old function that happens to overload with 
// (b), (c), and (d) -- but not (e), which we'll 
// discuss in a moment 
void f( double );                           // (f)

// Finally, let's focus on function templates only and consider the overloading rules to see which ones get called in different situations. The rules are pretty simple, at least at a high level, and can be expressed as a classic two-class system:
//  Nontemplate functions are first-class citizens. A plain old nontemplate function that matches the parameter types as well as any function template will be selected over an otherwise-just-as-good function template.
//  If there are no first-class citizens to choose from that are at least as good, then function base templates as the second-class citizens get consulted next. Which function base template gets selected depends on which matches best and is the "most specialized" (important note: this use of "specialized" oddly enough has nothing to do with template specializations; it's just an unfortunate colloquialism) according to a set of fairly arcane rules:
//      If it's clear that there's one "most specialized" function base template, that one gets used. If that base template happens to be specialized for the types being used, the specialization will get used, otherwise the base template instantiated with the correct types will be used.
//      Else if there's a tie for the "most specialized" function base template, the call is ambiguous because the compiler can't decide which is a better match. The programmer will have to do something to qualify the call and say which one is wanted.
//      Else if there's no function base template that can be made to match, the call is bad and the programmer will have to fix the code.

// Putting these rules together, here's a sample of what we get:

// Example 1, continued: Overload resolution 
// 
void foobar()
   {
bool b; 
int i; 
double d;

f( b );        // calls (b) with T = bool 
f( i, 42, d ); // calls (c) with T = int 
f( &i );       // calls (d) with T = int 
f( i );        // calls (e) 
f( d );        // calls (f)
   }

// So far I've deliberately chosen simpler cases, because here's where we step off into the deep end of the pool.
