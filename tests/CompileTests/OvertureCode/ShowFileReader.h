#ifndef SHOW_FILE_READER_H
#define SHOW_FILE_READER_H

#include "PlotStuff.h"
#include "HDF_DataBase.h"

// -------------------------------------------------------------------
// This class can be used to access solutions from a show file.
//
// This class is used by plotStuff. It is also used by solvers to 
// read in initial conditions from a show file.
// -------------------------------------------------------------------

// extern const aString *pointerToAConstString;

class ShowFileReader
{
 public:

  enum ReturnType
  {
    notFound=0,
    solutionFound=1,
    gridFound=2,
    solutionAndGridFound=3
  };

  enum GridLocationEnum
  {
    useDefaultLocation=-2,
    useCurrentFrame=-1
  };
  

  ShowFileReader(const aString & nameOfShowFile=nullString);
  ~ShowFileReader();
      
  int open(const aString & nameOfShowFile);
  int close();
  
  int getNumberOfFrames() const;
  int getNumberOfSolutions() const;
  int getNumberOfSequences() const;
  

  ReturnType getAGrid(MappedGrid & cg, 
                      int & solutionNumber, 
                      int frameForGrid=useDefaultLocation);

  ReturnType getAGrid(GridCollection & cg, 
                      int & solutionNumber, 
                      int frameForGrid=useDefaultLocation);

  ReturnType getASolution(int & solutionNumber,
			  MappedGrid & cg,
			  realMappedGridFunction & u);

  ReturnType getASolution(int & solutionNumber,
			  GridCollection & cg,
			  realGridCollectionFunction & u);
  
  int getSequenceNames(aString *name, int maximumNumberOfNames);
  
  int getSequence(int sequenceNumber,
		  aString & name, realArray & time, realArray & value, 
		  aString *componentName1, int maxcomponentName1,
		  aString *componentName2, int maxcomponentName2);
  
  // return a pointer to a frame (by default the current frame)
  HDF_DataBase* getFrame(int solutionNumber=-1);  

  bool isAMovingGrid();

  // get header comments for the last grid or solution that was found
  const aString* getHeaderComments(int & numberOfHeaderComments);

 private:
  bool showFileIsOpen;
  GenericDataBase **showFile;
  int numberOfFrames;
  int numberOfSolutions;
  int numberOfSequences;

  int numberOfShowFiles;
  int numberOfOpenFiles;
  int frameNumberForGrid;
  int maximumNumberOfHeaderComments;
  int numberOfHeaderComments;        // for last solution/grid found
  
  int maxNumberOfShowFiles;     
  int maxNumberOfOpenFiles;     // there is some limit like 30-40
  aString *headerComment;        // for last solution/grid found
  aString nameOfShowFile;
  bool movingGridProblem;

  int streamMode;

  HDF_DataBase currentFrame;

  int openShowFile(const int n);
      
  int getNumberOfValidFiles();
  
  int countNumberOfFramesAndSolutions();
  
  void checkSolutionNumber(const aString & routineName, int & solutionNumber );
};


#endif





      
      
  
