#ifndef OVERTURE_INIT_H
#define OVERTURE_INIT_H

class floatMappedGridFunction;
class doubleMappedGridFunction;
class intMappedGridFunction;
// class realMappedGridFunction;
#ifdef OV_USE_DOUBLE
#define realMappedGridFunction doubleMappedGridFunction
#define realGridCollectionFunction doubleGridCollectionFunction
#else
#define realMappedGridFunction floatMappedGridFunction
#define realGridCollectionFunction floatGridCollectionFunction
#endif

class floatGridCollectionFunction;
class doubleGridCollectionFunction;
class intGridCollectionFunction;

class MappingParameters;
class BoundaryConditionParameters;
class GraphicsParameters;
class PlotStuff;
class ListOfMappingRC;

class Overture
{
 public:
 static int start(int argc, char *argv[]);
 static int finish();

  static floatSerialArray & nullFloatArray();
  static doubleSerialArray & nullDoubleArray();
  static RealArray & nullRealArray();
  static intSerialArray & nullIntArray();
	 
  static floatDistributedArray & nullFloatDistributedArray();
  static doubleDistributedArray & nullDoubleDistributedArray();
  static RealDistributedArray & nullRealDistributedArray();
  static IntegerDistributedArray & nullIntegerDistributedArray();
	 
  static MappingParameters & nullMappingParameters();
	 
  static floatMappedGridFunction & nullFloatMappedGridFunction();
  static doubleMappedGridFunction & nullDoubleMappedGridFunction();
  static realMappedGridFunction & nullRealMappedGridFunction();
  static intMappedGridFunction & nullIntMappedGridFunction();
	 
  static floatGridCollectionFunction & nullFloatGridCollectionFunction();
  static realGridCollectionFunction & nullRealGridCollectionFunction();
  static doubleGridCollectionFunction & nullDoubleGridCollectionFunction();
  static intGridCollectionFunction & nullIntGridCollectionFunction();
	 
  static BoundaryConditionParameters & defaultBoundaryConditionParameters();
  static GraphicsParameters & defaultGraphicsParameters();
  static void setDefaultGraphicsParameters( GraphicsParameters *gp =NULL );

  static PlotStuff* getGraphicsInterface();
  static void setGraphicsInterface( PlotStuff *ps);

  static ListOfMappingRC* getMappingList();
  static void setMappingList(ListOfMappingRC *list);

 protected:
  static PlotStuff *pPlotStuff;
  static ListOfMappingRC *pMappingList;

 private:

  static floatSerialArray *pNullFloatArray;  
  static doubleSerialArray *pNullDoubleArray;  
  static RealArray *pNullRealArray;  
  static intSerialArray  *pNullIntArray;

  static floatDistributedArray *pNullFloatDistributedArray;  
  static doubleDistributedArray *pNullDoubleDistributedArray;  
  static RealDistributedArray *pNullRealDistributedArray;  
  static IntegerDistributedArray  *pNullIntegerDistributedArray;


  static MappingParameters *pNullMappingParameters;

  static floatMappedGridFunction  *pNullFloatMappedGridFunction;
  static doubleMappedGridFunction *pNullDoubleMappedGridFunction;
  static intMappedGridFunction    *pNullIntMappedGridFunction;
  static realMappedGridFunction   *pNullRealMappedGridFunction;

  static floatGridCollectionFunction  *pNullFloatGridCollectionFunction;
  static doubleGridCollectionFunction *pNullDoubleGridCollectionFunction;
  static intGridCollectionFunction    *pNullIntGridCollectionFunction;
  static realGridCollectionFunction   *pNullRealGridCollectionFunction;

  static BoundaryConditionParameters *pDefaultBoundaryConditionParameters;

  static GraphicsParameters *pDefaultGraphicsParameters;  // used for default arguments
  static GraphicsParameters *pCurrentGraphicsParameters;

};

#undef realMappedGridFunction
#undef realGridCollectionFunction
#endif
