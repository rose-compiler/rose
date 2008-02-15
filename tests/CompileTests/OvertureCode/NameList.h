#ifndef NAMELIST_H
#define NAMELIST_H "NameList.h"

#include <A++.h>
#include "aString.H"
#include <string.h>

#include "OvertureTypes.h"

//=============================================================================
//  This class defines routines for inputing Parameters by name
//  in a similar fashion to the namelist facility in Fortran
//
//  Suppose that an application has a list of variables that can
//  be changed by the user. Also suppose that the variables have
//  default values so that the user may only want to selectively
//  change the values of some variables
//
//  Here is how the application could prompt for changes using the
//  NameList class:
//
//    int itest=0, imeth=3;        // Here are some parameters
//    real a=1.,b=2.;
//    IntegerArray c(3,3,3);  c=0.;
//    RealArray d(4,4,4);  d=1.;
//
//    NameList nl;   // create a namelist
//
//    aString answer(80),name(80);
//    int i0,i1,i2,i3;     
//    for( ;; )
//    {
//      cout << "Enter changes to variables, exit to continue" << endl;
//      cin >> answer;
//      if( answer=="exit" ) break;
//  
//      nl.getVariableName( answer, name );   // parse the answer
//      if( name=="itest" )
//        itest=nl.intValue(answer);
//      else if( name=="imeth" )
//        imeth=nl.intValue(answer);
//      else if( name=="a" )
//        a=nl.realValue(answer);
//      else if( name=="b" )
//        b=nl.realValue(answer);
//      else if( name=="c" )
//        nl.getIntArray( answer,c );
//      else if( name=="d" )
//        nl.getRealArray( answer,d,i0,i1,i2 ); // return indices as well
//      else
//        cout << "unknown response: [" << name << "]" << endl;
//    }
//  
//  
// Here is some typical input:
//
//  itest=1
//  c(1,0,1)=55
//  a=55.6
//  d(1,1,2)=33.3
//  exit
//
//============================================================================
class NameList
{
 public:
  
  NameList();
  ~NameList();
  
  void getVariableName( aString & answer, aString & name );

  int intValue( aString & answer );
  
  // add float and double ****************************************************
  real realValue( aString & answer );
  
  int getIntArray( aString & answer, IntegerArray & a );
  int getIntArray( aString & answer, IntegerArray & a, int & i0 );
  int getIntArray( aString & answer, IntegerArray & a, int & i0, int & i1 );
  int getIntArray( aString & answer, IntegerArray & a, int & i0, int & i1, int & i2 );
  int getIntArray( aString & answer, IntegerArray & a, int & i0, int & i1, int & i2, int & i3 );

  // add float and double ****************************************************
  int getRealArray( aString & answer, RealArray & a );
  int getRealArray( aString & answer, RealArray & a, int & i0 );
  int getRealArray( aString & answer, RealArray & a, int & i0, int & i1 );
  int getRealArray( aString & answer, RealArray & a, int & i0, int & i1, int & i2 );
  int getRealArray( aString & answer, RealArray & a, int & i0, int & i1, int & i2, int & i3 );

  int arrayEqualsName( aString & answer, const aString nameList[], IntegerArray & a );
  int arrayEqualsName( aString & answer, const aString nameList[], IntegerArray & a, int & i0 );
  int arrayEqualsName( aString & answer, const aString nameList[], IntegerArray & a, int & i0, int & i1);
  int arrayEqualsName( aString & answer, const aString nameList[], IntegerArray & a, int & i0, int & i1, int & i2);
  int arrayEqualsName( aString & answer, const aString nameList[], IntegerArray & a,
                       int & i0, int & i1, int & i2, int & i3 );

  int arrayOfNameEqualsValue( aString & answer, const aString nameList[], IntegerArray & a );
  int arrayOfNameEqualsValue( aString & answer, const aString nameList[], IntegerArray & a, int & i0 );
  int arrayOfNameEqualsValue( aString & answer, const aString nameList[], IntegerArray & a, int & i0, int & i1);
  int arrayOfNameEqualsValue( aString & answer, const aString nameList[], IntegerArray & a, int & i0, int & i1, int & i2);
  int arrayOfNameEqualsValue( aString & answer, const aString nameList[], IntegerArray & a, 
                              int & i0, int & i1, int & i2, int & i3 );

  int arrayOfNameEqualsValue( aString & answer, const aString nameList[], RealArray & a );
  int arrayOfNameEqualsValue( aString & answer, const aString nameList[], RealArray & a, int & i0 );
  int arrayOfNameEqualsValue( aString & answer, const aString nameList[], RealArray & a, int & i0, int & i1);
  int arrayOfNameEqualsValue( aString & answer, const aString nameList[], RealArray & a, int & i0, int & i1, int & i2);
  int arrayOfNameEqualsValue( aString & answer, const aString nameList[], RealArray & a, 
                              int & i0, int & i1, int & i2, int & i3 );

  int intArrayValue( aString & answer, int & value, int & i0 );
  int intArrayValue( aString & answer, int & value, int & i0, int & i1 );
  int intArrayValue( aString & answer, int & value, int & i0, int & i1, int & i2 );
  int intArrayValue( aString & answer, int & value, int & i0, int & i1, int & i2, int & i3 );

  int realArrayValue( aString & answer, real & value, int & i0 );
  int realArrayValue( aString & answer, real & value, int & i0, int & i1 );
  int realArrayValue( aString & answer, real & value, int & i0, int & i1, int & i2 );
  int realArrayValue( aString & answer, real & value, int & i0, int & i1, int & i2, int & i3 );

  aString getString(aString & answer);

 private:
  
  aString & subString( aString & s, const int i1, const int i2 );
  int intArrayValue( aString & answer, int & value, const int & numberExpected, int & i0, int & i1, int & i2, int & i3 );
  int realArrayValue( aString & answer, real & value, const int & numberExpected, int & i0, int & i1, int & i2, int & i3 );

};


#endif  // "NameList.h"
