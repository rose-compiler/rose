#ifndef KKC_UGEN

#include "AdvancingFront.h"
#include "TriangleWrapper.h"
#include "MappingInformation.h"
#include "PlotStuff.h"
#include "AbstractException.h"
#include "Ogen.h"

class Ugen 
{

public:
  
  // helps keep track of which mesh generator is currently being used
  enum GeneratorType {
    TriWrap,
    AdvFront
  };

  Ugen();
  Ugen(PlotStuff &ps_);

  ~Ugen();

  // build a hybrid grid interactively
  void updateHybrid(CompositeGrid & cg, MappingInformation & mapInfo );

  // build a hybrid grid automatically
  void updateHybrid(CompositeGrid & cg);

  void buildHybridInterfaceMappings(MappingInformation &mapInfo, 
				    CompositeGrid &cg,
				    Ugen::GeneratorType genWith,
				    intArray * & gridIndex2UnstructuredVertex,
				    intArray   & unstructuredVertex2GridIndex,
				    intArray * & gridVertex2UnstructuredVertex,
				    intArray   & initialFaceZones);


protected:

  int initialize();

  void plot(const aString & title, CompositeGrid &cg, bool plotComponentGrids, bool plotTriangle, bool plotAdvFront);

  //  void buildHybridInterfaceMappings(CompositeGrid &cg);

  void preprocessCompositeGridMasks(CompositeGrid &cg);

  void initializeGeneration(CompositeGrid &cg, intArray *vertexIndex, 
			    intArray &numberOfVertices, intArray *vertexIDMap, 
			    intArray &vertexGridIndexMap, intArray *gridIndexVertexMap, 
			    intArray &initialFaces, intArray &initialFaceZones,
			    realArray & xyz_initial);


  void removeHangingFaces(CompositeGrid &cg);

  void buildHybridVertexMappings(CompositeGrid &cg, intArray *vertexIndex, intArray &numberOfVertices, intArray *vertexIDMap, intArray &vertexGridIndexMap, intArray *gridIndexVertexMap, realArray & xyz_initial);

  //void generateInitialFaceList(CompositeGrid &cg, intArray *vertexIndex, intArray &numberOfVertices, intArray *vertexIDMap, intArray &vertexGridIJKMap, intArray &initialFaces);
  void generateInitialFaceList(CompositeGrid &cg, intArray *vertexIndex, 
                               intArray &numberOfVertices, intArray *vertexIDMap, 
			       intArray &vertexGridIndexMap, intArray *gridIndexVertexMap, 
			       intArray &initialFaces, intArray &initialFaceZones);

  //void generateBoundaryMappings(CompositeGrid &cg);

  void computeZoneMasks(CompositeGrid &cg, intArray * &zoneMasks, intArray &numberOfMaskedZones);

  void generateSpacingControlMesh(CompositeGrid &cg, const intArray & initialFaceZones, const realArray & xyz_initial);

  void enlargeHole(CompositeGrid &cg, intArray &vertexGridIndexMap);
  void generateHoleLists(CompositeGrid &cg, intArray * vertexIndex, intArray &numberOfVertices);

  void sealHoles( CompositeGrid &cg, intArray *gridIndexVertexMap, intArray &initialFaces, realArray &xyz_initial);

  void generateWithAdvancingFront();
  void generateWithTriangle();

  AdvancingFront advancingFront;
  TriangleWrapper triangleWrapper;
  UnstructuredMapping delaunayMesh;

  PlotStuff *ps;
  PlotStuffParameters psp;

};
  

class UnstructuredGeneratorError : public AbstractException 
{
public:
  virtual void debug_print() const { cerr << "\nUnstructuredGeneratorError"; }
};

class PreProcessingError : public UnstructuredGeneratorError
{
public:
  void debug_print() const
  {
    cerr<<": PreProcessingError : problems were found pre-processing the CompositeGrid";
  }
};

#endif
