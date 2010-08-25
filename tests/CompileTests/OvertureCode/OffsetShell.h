#ifndef OFFSET_SHELL_H
#define OFFSET_SHELL_H

#include "Mapping.h"

class MatrixTransform;
class DataPointMapping;
class HyperbolicMapping;

class OffsetShell
{
 public:
  enum OffsetTypeEnum
  {
    translationOffset,
    normalOffset
  };
  

  OffsetShell();
  ~OffsetShell();
  
  int createOffsetMappings( MappingInformation & mapInfo );
  
  int buildOffsetMappings( GenericGraphicsInterface & gi, 
                           GraphicsParameters & parameters, MappingInformation & mapInfo );

  int generateVolumeGrids( GenericGraphicsInterface & gi, 
                           GraphicsParameters & parameters, MappingInformation & mapInfo );

 protected:

  int buildOffsetSurface(MappingInformation & mapInfo );

  real rOffset[2][2];  // rOffset[side][axis] distance from boundary of the curve on the unit square
  real cornerShift;   // move the corner point in by this factor
  real cornerOffset; // stop the straight line segment this factor from the corner

  real cornerStretchExponent[4];  // for stretching points at corners.
  

  OffsetTypeEnum offsetType;
  real shift[3];     // offset shift for a translation offset.
  real offsetDistance;  // for a normal offset
  
  int numberOfMatchingPoints;
  int numberOfEdgeLines[3];
  real normalDistance;

  Mapping *referenceSurface;
  Mapping *offsetSurface;              // surface offset from reference surface
  DataPointMapping *edgeSurface;       // edge surface that joins the ref surface to the offset surface
  HyperbolicMapping *edgeVolume;       // volume grid for the edge surface
  HyperbolicMapping *referenceVolume;  // volume grid on reference surface
  HyperbolicMapping *offsetVolume;     // volume grid on offset surface

  
};

#endif
