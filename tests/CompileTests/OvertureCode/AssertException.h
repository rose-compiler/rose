#ifndef __ASSERT_EXCEPTION__
#define __ASSERT_EXCEPTION__

// assertions based on exception handling
// similar to Stroustroup, "The C++ Programming Language" section 24.3.7.2
// the following usefull definition is ifdef'd out since the Sun compilers cannot understand it
#if 0
template<class Except, class Arg>
inline 
void
AssertException(Arg assertion) 
{

  if ( !assertion ) throw Except();

}

#endif

template<class Except, class Arg>
inline 
void
AssertException(Arg assertion, Except exception) 
{

  if ( !assertion ) throw exception;

}

#endif
