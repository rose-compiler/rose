#ifndef COMPOSITE_TOPOLOGY_H
#define COMPOSITE_TOPOLOGY_H

#include "Mapping.h"

class CompositeSurface;
class UnstructuredMapping;

class CompositeTopology
{
 public:

  enum EdgeCurveStatusEnum
  {
    edgeCurveIsNotMerged=0,  // was 0
    edgeCurveIsSplit,      // was -1 
    edgeCurveIsMerged,     // was 1
    edgeCurveIsRemoved,     // was 2
    edgeCurveIsNotDefined
  };

  CompositeTopology(CompositeSurface & cs);
  ~CompositeTopology();
  
  CompositeTopology & operator =( const CompositeTopology & X );

  int buildEdgeCurves();

  int mergeEdgeCurves();
  
  int triangulate();

  int update();

  int getNumberOfEdgeCurves() const { return numberOfEdgeCurves; }
  
  EdgeCurveStatusEnum getEdgeCurveStatus(int number) const;

  Mapping& getEdgeCurve(int number);
  
  // Mapping** getEdgeCurves() const { return edgeCurve; }
  
  UnstructuredMapping* getTriangulation() const { return globalTriangulation;} 

  bool topologyDetermined(){ return signForNormal.getLength(0)>0; } // ** fix this ***
  
  const IntegerArray & getSignForNormal() const { return signForNormal;} 
  
  
  protected:

  CompositeSurface & cs;

  UnstructuredMapping *globalTriangulation;
  Mapping **edgeCurve;
  int numberOfEdgeCurves;

  real mergeTolerance;
  real deltaS;
  int minNumberOfPointsOnAnEdge;
  
  IntegerArray signForNormal;
  int *edgeCurveStatus;
  

};



#endif
