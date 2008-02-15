#ifndef BODY_DEFINITION_H
#define BODY_DEFINITION_H "BodyDefinition.h"

// =======================================================================================
//    Use this class to define the relationship between a body and a grid.
// =======================================================================================

#include "Overture.h"

class BodyDefinition 
{
 public:

  BodyDefinition();
  BodyDefinition( const BodyDefinition & bd );
  
  ~BodyDefinition();
  
  BodyDefinition & operator =( const BodyDefinition & x0 );

  int totalNumberOfSurfaces() const;
  int numberOfFacesOnASurface(int surfaceNumber) const;
  int getFace(int surfaceNumber,int face, int & side, int & axis, int & grid) const;

  int defineSurface( const int & surfaceNumber, const int & numberOfFaces_, IntegerArray & boundary ); 

  int getSurface(  const int & surfaceNumber, int & numberOfFaces_, IntegerArray & boundary ) const;

 protected:

  int initialize();
  int surfaceIndex( int surfaceNumber ) const;

  int numberOfSurfaces,maximumNumberOfFaces;
  IntegerArray surfaceIdentifier, numberOfFaces;
  IntegerArray boundaryFaces;
  
  friend class Integrate;  // fix this
  friend class CutCells;
};

#endif
