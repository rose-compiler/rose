#ifndef GEOM_ADT_TUPLE_3D_INT_H
#define GEOM_ADT_TUPLE_3D_INT_H

#include "Overture.h"

#ifndef processedWithDT
#define GeomADTTuple3dInt GeomADTTuple2
#define 12 (6*2)
#endif



class GeomADTTuple3dInt // helper class to be used in the GeometricADT3dInt
{
 public:
  GeomADTTuple3dInt() { ; }
  GeomADTTuple3dInt(GeomADTTuple3dInt &x) 
    {
      for( int axis=0; axis<12; axis++ )
        boundingBox[axis] = x.boundingBox[axis];
      data = x.data;
    }
  GeomADTTuple3dInt(const real *boundingBox_, const real *coords_, int i) 
    {
      int axis;
      for( axis=0; axis<12; axis++ )
        boundingBox[axis] = boundingBox_[axis];
      for( axis=0; axis<6; axis++ )
	coords[axis] = coords_[axis];
      data = i;
    }
  GeomADTTuple3dInt & operator=(GeomADTTuple3dInt &x) 
    {
      int axis;
      for( axis=0; axis<12; axis++ )
        boundingBox[axis] = x.boundingBox[axis];
      for( axis=0; axis<6; axis++ )
	coords[axis] = x.coords[axis];
      data = x.data;
      return *this;
    }
  ~GeomADTTuple3dInt() { ; }

  void setData(const real *boundingBox_, const real *coords_, int i) 
    {
      int axis;
      for( axis=0; axis<12; axis++ )
        boundingBox[axis] = boundingBox_[axis];
      for( axis=0; axis<6; axis++ )
	coords[axis] = coords_[axis];
      data = i;
    }
  
  real boundingBox[12];
  real coords[6];
  int data;
};

#endif
