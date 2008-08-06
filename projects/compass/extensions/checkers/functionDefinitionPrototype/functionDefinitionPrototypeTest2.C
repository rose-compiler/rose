/*
 * Author: Gary Yuan
 * Date: 6/22/2007
 * File: functionDeclarationPrototypeCheckerTest.C
 * Purpose:
 *        This file is a test for the checker 
 *        functionDeclarationPrototypeChecker to catch function declarations
 *        without prototypes and prototypes without function declarations
 */

#include <stdio.h>

class Class
{
  public:
    Class();
    ~Class();
    int foo(){ return 0; }
};

template<class T, class R>
R foo4( const T t, const R r )
{
  return t+r;
}//foo4(), template functions do not incur error

//test for overloading
void foo( double );
int foo( int );

int foo2(); //prototype for foo2()

int foo( char *string )
{
  int i = 0;
  printf( "%s", string );

  return i;
}//foo(), foo has no prototype

int foo2()
{
  int i = 1;
  return i;
}//foo2(), has prototype

int main( int argc, char **argv )
{
  foo( "wow" ); //call foo()
  foo4( 1,3 );
  return 0;
}//main(), should not incur error
