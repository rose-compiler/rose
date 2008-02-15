#ifndef GRID_FUNCTION_PARAMETERS_H 
#define GRID_FUNCTION_PARAMETERS_H

class GridFunctionParameters
{
public:

  // Here we define all the enums related to GridFunctionType's
  enum GridFunctionType
  {
    defaultCentering=-1,
    general,
    vertexCentered,
    cellCentered,
    faceCenteredAll,
    faceCenteredAxis1,
    faceCenteredAxis2,
    faceCenteredAxis3
    };

  enum GridFunctionTypeWithComponents
  {
    generalWith0Components,
    generalWith1Component,
    generalWith2Components,
    generalWith3Components,
    generalWith4Components,
    generalWith5Components,

    vertexCenteredWith0Components,
    vertexCenteredWith1Component,
    vertexCenteredWith2Components,
    vertexCenteredWith3Components,
    vertexCenteredWith4Components,
    vertexCenteredWith5Components,

    cellCenteredWith0Components,
    cellCenteredWith1Component,
    cellCenteredWith2Components,
    cellCenteredWith3Components,
    cellCenteredWith4Components,
    cellCenteredWith5Components,

    faceCenteredAllWith0Components,
    faceCenteredAllWith1Component,
    faceCenteredAllWith2Components,
    faceCenteredAllWith3Components,
    faceCenteredAllWith4Components,
    faceCenteredAllWith5Components,

    faceCenteredAxis1With0Components,
    faceCenteredAxis1With1Component,
    faceCenteredAxis1With2Components,
    faceCenteredAxis1With3Components,
    faceCenteredAxis1With4Components,
    faceCenteredAxis1With5Components,

    faceCenteredAxis2With0Components,
    faceCenteredAxis2With1Component,
    faceCenteredAxis2With2Components,
    faceCenteredAxis2With3Components,
    faceCenteredAxis2With4Components,
    faceCenteredAxis2With5Components,

    faceCenteredAxis3With0Components,
    faceCenteredAxis3With1Component,
    faceCenteredAxis3With2Components,
    faceCenteredAxis3With3Components,
    faceCenteredAxis3With4Components,
    faceCenteredAxis3With5Components
    };

  enum faceCenteringType    // Here are some standard types of face centred grid functions
  { 
    none=-1,                // not face centred
    direction0=0,           // all components are face centred along direction (i.e. axis) = 0
    direction1=1,           // all components are face centred along direction (i.e. axis) = 1
    direction2=2,           // all components are face centred along direction (i.e. axis) = 2
    all=-2                  // components are face centred in all directions, positionOfFaceCentering determines
  };                        // the Index position that is used for the "directions"

  GridFunctionType inputType,    // type of the input grid function (for Coefficient matrices)
                   outputType;   // type for the result

  GridFunctionParameters();
  ~GridFunctionParameters();

  // This constructors cast a GridFunctionType into a GridFunctionParameter object with outputType=type
  GridFunctionParameters(const GridFunctionType  & type);
  
  // convert a GridFunctionParameters object into a GridFunctionType
  operator GridFunctionType () const;

};

#endif
