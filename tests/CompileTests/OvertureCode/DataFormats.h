#ifndef DATA_FORMATS_H
#define DATA_FORMATS_H

// The DataFormats class handles IO and conversion between different
// data formats such as Plot3d and ???

#include "Overture.h"
class DataPointMapping;
class UnstructuredMapping;
class GenericGraphicsInterface;


class DataFormats
{
 public:

  enum DataFormatsEnum
  {
    Plot3d,
    IGES,
    Ingrid,
    Ply
  };


  DataFormats(GenericGraphicsInterface *ggiPointer=NULL );
  virtual ~DataFormats();
  
  virtual int setGraphicsInterface( GenericGraphicsInterface *ggiPointer=NULL ); 
  
  // read in a single grid from a file, optionally read a mask (iblank) array
  virtual int readPlot3d(DataPointMapping & dpm,
                         int gridToRead=-1,  
                         const aString & gridFileName=nullString,
                         IntegerArray *maskPointer=NULL );

  // read in all grids from a file.
  virtual int readPlot3d(MappingInformation & mapInfo, 
                         const aString & gridFileName=nullString,
                         IntegerArray *maskPointer=NULL );

  // read in a solution array and parameters from a file
  virtual int readPlot3d(RealArray & q, RealArray & par, 
                         const aString & qFileName=nullString);

  // read in an unstructured Mapping from an Ingrid file
  virtual int readIngrid(UnstructuredMapping &map,
                         const aString & gridFileName=nullString);

#if 0
  // read in an unstructured mesh into a mapped grid 
  virtual int readIngrid(MappedGrid &mg,
                         const aString & gridFileName=nullString);
#endif
  
  // read in an unstructured Mapping from a Ply polygonal file (Stanford/graphics)
  virtual int readPly(UnstructuredMapping &map, 
                      const aString &gridFileName =nullString);

  // save a Mapping in plot3d format
  virtual int writePlot3d(Mapping & map,
                          const aString & gridFileName=nullString );
  
  // save a MappedGrid in plot3d format
  virtual int writePlot3d(MappedGrid & mg,
                          const aString & gridFileName=nullString );
  
  // save a CompositeGrid in plot3d format
  virtual int writePlot3d(CompositeGrid & cg,
                          const aString & gridFileName=nullString,
                          const aString & interpolationDataFileName=nullString );

  // write a Mapping to an Ingrid file
  virtual int writeIngrid(Mapping &map,
			  const aString & gridFileName=nullString);

  // write a ComposteGrid to an Ingrid file
  virtual int writeIngrid(CompositeGrid & cg,
                          const aString & gridFileName=nullString);

 protected:
  
  int readPlot3d(MappingInformation & mapInfo, 
                 int gridToRead,
		 const aString & plot3dFileName,
		 DataPointMapping *dpmPointer,
		 RealArray *qPointer,
		 RealArray & par = Overture::nullRealArray(),
		 IntegerArray *maskPointer=NULL );

  GenericGraphicsInterface *giPointer;
  
};

#endif
