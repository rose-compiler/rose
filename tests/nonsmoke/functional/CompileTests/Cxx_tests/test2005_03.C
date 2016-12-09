// Test for calls to templated member functions

// Skip version 4.x gnu compilers
#if ( __GNUC__ == 3 )

// test template member function
template <typename T>
class X
   {
     public:
          void foo(){};
          template <typename S> void foo2 () {};
   };

int main()
   {
     X<int> objectInt;
     objectInt.foo();
     objectInt.foo2<float>();

/*
14.2 - Names of template specializations

   -4- When the name of a member template specialization appears after . or -> in a postfix-expression, 
       or after nested-name-specifier in a qualified-id, and the postfix-expression or qualified-id 
       explicitly depends on a template-parameter (temp.dep), the member template name must be prefixed 
       by the keyword template. Otherwise the name is assumed to name a non-template. [Example:

class X {
public:
	template<size_t> X* alloc();
	template<size_t> static X* adjust();
};
template<class T> void f(T* p)
{
	T* p1 = p->alloc<200>();
		//  ill-formed:  <  means less than

	T* p2 = p->template alloc<200>();
		//  OK:  <  starts template argument list

	T::adjust<100>();
		//  ill-formed:  <  means less than

	T::template adjust<100>();
		//  OK:  <  starts explicit qualification
}

--- end example]
*/

// TOO (2/15/2011): Thrifty (gcc 3.4.4):
// error: `template' (as a disambiguator) is only allowed within templates
#if ( __GNUC_MINOR__ > 4 )
     objectInt.template foo2<float>();
#endif
     return 0;
   }

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

