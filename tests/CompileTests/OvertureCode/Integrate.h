#ifndef INTEGRATE_H
#define INTEGRATE_H "Integrate.h"

// =======================================================================================
//    Use this class to integrate functions on overlapping grids, both volume
//  and surface integrals.
// =======================================================================================

#include "Overture.h"
#include "BodyDefinition.h"

class Oges;

class Integrate
{
 public:

  Integrate();
  Integrate( CompositeGrid & cg );
  ~Integrate();
  
  int numberOfFacesOnASurface(int surfaceNumber) const;
  int getFace(int surfaceNumber,int face, int & side, int & axis, int & grid) const;

  const BodyDefinition & getBodyDefinition() const;
  
  int defineSurface( const int & surfaceNumber, const int & numberOfFaces_, IntegerArray & boundary ); 

  real surfaceIntegral(const RealCompositeGridFunction & u, const int & surfaceNumber = -1 );

  int surfaceIntegral(const RealCompositeGridFunction & u, 
                      const Range & C, 
                      RealArray & integral,
                      const int & surfaceNumber = -1 );

  int updateToMatchGrid( CompositeGrid & cg );

  real volumeIntegral( const RealCompositeGridFunction & u );
  
  RealCompositeGridFunction & integrationWeights();
  RealCompositeGridFunction & leftNullVector();
 
  static int debug;

 protected:

  BodyDefinition bd;

  int initialize();
  int computeWeights();
  int computeSurfaceWeights(int surfaceNumber=-1 );
  
  int computeLeftNullVector();
  int surfaceIndex( int surfaceNumber );

  CompositeGrid cg;

  int orderOfAccuracy;
//  int numberOfSurfaces,maximumNumberOfFaces;
//  IntegerArray surfaceIdentifier, numberOfFaces;
//  IntegerArray boundaryFaces, boundaryHasOverlap;
  IntegerArray boundaryHasOverlap;
  IntegerArray faceWeightsDefined, surfaceWeightsDefined;
  
  realCompositeGridFunction weights, nullVector;
  
  Oges *solver;
  bool weightsComputed, leftNullVectorComputed;
  bool allFaceWeightsDefined, weightsUpdatedToMatchGrid;
  
};

#endif
