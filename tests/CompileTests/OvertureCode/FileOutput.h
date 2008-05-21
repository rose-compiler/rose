#ifndef FILE_OUTPUT_H
#define FILE_OUTPUT_H

#include "PlotStuff.h"
#include "displayLowercase.h"

// This class is used for outputing Overture data in ascii format to a file.

class FileOutput
{
 public:
  
  FileOutput();
  ~FileOutput();

  aString getFileName(){return outputFileName;} 
  int save( realGridCollectionFunction & u, const aString & label=nullString );
  int update( realGridCollectionFunction & u, GenericGraphicsInterface & ps );

  enum GeometryArraysEnum
  {
    mask=0,
    vertex,
    numberOfGeometryArrays
  };


 protected:

  int debug;
  aString outputFileName;
  aString format;
  bool setValueAtUnusedPoints;
  real valueForUnusedPoints;
  int numberOfGhostLines;
  bool addLabels;

  FILE *outputFile;
  DisplayParameters dp;

  IntegerArray saveComponent;
  IntegerArray saveGeometry;
  
  IntegerArray saveGrids;    // which grids to save.
  IntegerArray saveGridFace; // saveGridLines(n,[0,1,2])=[grid,dir1,dir1Value]  : save face dir1=dir1Value 
  
  int saveData( realMappedGridFunction & u, int grid, int side =-1 , int axis = -1 );
  int updateParameterArrays( realGridCollectionFunction & u );
};

#endif
