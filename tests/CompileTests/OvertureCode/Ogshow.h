#ifndef OGSHOW_H
#define OGSHOW_H "Ogshow.h"

#include "CompositeGrid.h"
// include "MultigridCompositeGrid.h"
#include "CompositeGridFunction.h"
// include "MultigridCompositeGridFunction.h"
class HDF_DataBase;

class Ogshow
{
 public:

  Ogshow();
  Ogshow(const aString & nameOfShowFile, 
         const aString & nameOfDirectory = ".",
         int useStreamMode=false );
  ~Ogshow();

  // return a pointer to the current frame
  HDF_DataBase* getFrame();  

  int open(const aString & nameOfShowFile, 
           const aString & nameOfDirectory = ".",
	   int useStreamMode=false );

  int close();

  int saveGeneralComment( const aString & comment );
  int saveComment( const int commentNumber, const aString & comment );

  int saveSequence( const aString & name,
		    const RealArray & time, 
		    const RealArray & value, 
		    aString *name1=NULL, 
		    aString *name2=NULL);

  enum GridLocationEnum
  {
    useDefaultLocation=-2,
    useCurrentFrame=-1
  };
  
  int getNumberOfFrames() const;
  
  int saveSolution( realGridCollectionFunction & u, const aString & name="u", int frameForGrid=useDefaultLocation );

  int saveSolution( realMappedGridFunction & u, const aString & name="u", int frameForGrid=useDefaultLocation );

  int startFrame( const int frame=-1 );  // start a new frame or write to an existing one
  int endFrame();  // end the frame, close sub-files if appropriate.
  
  void setMovingGridProblem( const bool trueOrFalse );
  
  // indicate that the file should be flushed every so often:
  void setFlushFrequency( const int flushFrequency = 5  );
  int getFlushFrequency() const;

  // flush the data in the file  *** this does not work ***
  void flush();

 protected:
  void initialize();
  int cleanup();
  int createShowFile( const aString & nameOfShowFile, const aString & nameOfDirectory );
  int saveGeneralCommentsToFile();
  int saveCommentsToFile();
  
  aString showFileName, showFileDirectory;
  GenericDataBase *showFile;        // here is the show file
  GenericDataBase *showDir;         // Here is where we save the showfile information
  GenericDataBase *frame;           // current frame;
  
  
  int showFileCounter; // counts number of solutions saved so far  
  int showFileCreated; // TRUE if a show file is mounted
  int numberOfDimensions;
  int numberOfComponentGrids;  
  int numberOfComponents;
  int numberOfGeneralComments;
  int generalCommentsSaved;
  int numberOfComments;
  int commentsSaved;

  int numberOfGeneralCommentsAllocated;
  aString *generalComment;
  int numberOfCommentsAllocated;
  aString *comment;
  
  bool movingGridProblem;  // true if grids are moving
  int numberOfFrames;
  int frameNumber;            // current frame
  int numberOfFramesPerFile;  // This many frames per sub file
  int magicNumber;            // unique identifier saved with the file
  
  int sequenceNumber;         // number of seqences saved in the show file.

  int streamMode;             // if true, save file in stream mode (compressed).
};


#endif  
