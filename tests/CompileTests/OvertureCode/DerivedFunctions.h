#ifndef DERIVED_FUNCTIONS_H
#define DERIVED_FUNCTIONS_H

#include "ShowFileReader.h"
#include "GenericGraphicsInterface.h"

// Derive new functions from old ones. This class is used by plotStuff to
// create derived quantities such as vorticity, mach-number, derivatives etc.

class DerivedFunctions
{
 public:

  enum DerivedItems
  {
    vorticity=0,
    xVorticity,
    yVorticity,
    zVorticity,
    divergence,
    machNumber,
    pressure,
    temperature,
    speed,
    schlieren,
    minimumScale,
    r1MinimumScale,
    r2MinimumScale,
    r3MinimumScale,
    xDerivative,
    yDerivative,
    zDerivative,
    xxDerivative,
    yyDerivative,
    zzDerivative,
    laplaceDerivative,
    numberOfDerivedItems
  };


  DerivedFunctions(ShowFileReader & showFileReader );
  ~DerivedFunctions();
  

  int add( int derivative, const aString & name_, int n1=0, int n2=0 );

  int getASolution(int & solutionNumber,
                   MappedGrid & cg,
		   realMappedGridFunction & u);

  int getASolution(int & solutionNumber,
                   CompositeGrid & cg,
		   realCompositeGridFunction & u);

  int numberOfDerivedTypes() const { return numberOfDerivedFunctions; }
  
  int remove( int i );

  // update current list of derived grid functions
  int update( GenericGraphicsInterface & gi, 
	      int numberOfComponents, 
	      aString *componentNames );


 protected:

  int computeDerivedFunctions( realCompositeGridFunction & u );
  int getComponent( int & c, const aString & cName );

  ShowFileReader *showFileReader;
  int numberOfDerivedFunctions;
  IntegerArray derived;
  aString *name;
  Interpolant *interpolant;
  
};

#endif
