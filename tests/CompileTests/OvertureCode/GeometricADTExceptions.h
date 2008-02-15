#ifndef GEOMETRIC_ADT_EXCEPTIONS_H
#define GEOMETRIC_ADT_EXCEPTIONS_H

#include "AbstractException.h"
#include "AssertException.h"


//
// Implementation of GeometricADT exceptions
// 


class GeometricADTError : public AbstractException
{
public:

  virtual void debug_print() const { cerr<<"\nGeometricADT Error"; }
  
};


class UnknownError : public GeometricADTError
{
public:
  void debug_print()  const 
  { 
    //    GeometricADTError::debug_print();
    cerr<<": UnknownError : An internal error with the GeometricADT has occurred"; 
  }
};

class VerificationError : public GeometricADTError
{
public:
  void debug_print()  const 
  { 
    //GeometricADTError::debug_print();
    cerr<<": VerificationError : GeometricADT verification has detected an error in the tree structure"; 
  }
};

class InvalidDepth : public GeometricADTError
{
public:
  void debug_print()  const 
  { 
    //GeometricADTError::debug_print();
    cerr<<": InvalidDepth : Requested depth is beyond current depth of ADT"; 
  }
};

class TreeInsertionError : public GeometricADTError
{
public:
  void debug_print()  const 
  { 
    //GeometricADTError::debug_print();
    cerr<<": TreeInsertionError : Errors inserting a new node, either the node is full or the degree is violated"; 
  }
};

class InvalidADTDimension : public GeometricADTError
{
public:
  void debug_print()  const 
  { 
    //GeometricADTError::debug_print();
    cerr<<": InvalidADTDimension : Invalid dimension for GeometricADT"; 
  }
};

class InvalidADTAxis : public GeometricADTError
{
public:
  void debug_print()  const 
  { 
    //GeometricADTError::debug_print();
    cerr<<": InvalidADTAxis : Axis request is beyond dimension of current GeometricADT"; 
  }
};

class NULLADTRoot : public GeometricADTError
{
public:
  void debug_print()  const 
  { 
    //GeometricADTError::debug_print();
    cerr<<": NULLADTRoot : Null tree root"; 
  }
};

class OutOfBoundingBox : public GeometricADTError
{
public:
  void debug_print()  const 
  { 
    //GeometricADTError::debug_print();
    cerr<<": OutOfBoundingBox : Element insertion has gone beyond the GeometricADT's bounding box"; 
  }
};

// GeometricADT iterator exceptions
class GeometricADTIteratorError : public GeometricADTError
{
public:
  virtual void debug_print() const 
  {
    //GeometricADTError::debug_print();
    cerr<<": GeometricADTIteratorError : internal iterator error";
  }
};

// GeometricADT traversor Exceptions
class GeometricADTTraversorError : public GeometricADTError
{
public:
  virtual void debug_print() const 
  {
    //GeometricADTError::debug_print();
    cerr<<": GeometricADTTraversorError : internal traversor error";
  }
};

#endif
