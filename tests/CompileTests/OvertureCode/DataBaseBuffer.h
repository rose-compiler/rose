#ifndef DATA_BASE_BUFFER_H
#define DATA_BASE_BUFFER_H

#include "GenericDataBase.h"

class DataBaseBuffer
{
  enum 
  {
    openForReading,
    notOpen,
    openForWriting
  } streamIs;
  
  const float floatMagicNumber;  // these numbers separate entries in the stream buffers
  const int   intMagicNumber;
  const double doubleMagicNumber;
  
 public:
  DataBaseBuffer();
  ~DataBaseBuffer();
  
  bool isOpen() const;

  void openBuffer(GenericDataBase & db, const GenericDataBase::InputOutputMode & mode);
  void closeBuffer(GenericDataBase & db );
  

  int putToBuffer( const int & size, const char *data );
  int putToBuffer( const int & size, const int *data );
  int putToBuffer( const int & size, const float *data );
  int putToBuffer( const int & size, const double *data );
#ifdef OV_BOOL_DEFINED
  int putToBuffer( const int & size, const bool *data );
#endif

  int getFromBuffer( const int & size, char *data );
  int getFromBuffer( const int & size, int *data );
  int getFromBuffer( const int & size, float *data );
  int getFromBuffer( const int & size, double *data );
#ifdef OV_BOOL_DEFINED
  int getFromBuffer( const int & size, bool *data );
#endif

  int fp,ip,dp;        // buffer pointers
  int floatBufferSize, intBufferSize, doubleBufferSize;
  int floatBufferSizeIncrement, intBufferSizeIncrement, doubleBufferSizeIncrement;
  floatSerialArray floatBuffer;
  intSerialArray intBuffer;
  doubleSerialArray doubleBuffer;

};

#endif
