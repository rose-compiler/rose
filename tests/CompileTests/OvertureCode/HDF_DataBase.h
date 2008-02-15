#ifndef HDF_DATABASE_H 
#define HDF_DATABASE_H "HDF_DataBase.h"

#include "GenericDataBase.h"
#include "ListOfHDF_DataBaseRCData.h"
#include "ReferenceCounting.h"

//--------------------------------------------------------------------------
//
//  HDF_DataBase: This is a class to support access to and from a data-base.
//    This class knows how to get and put the types
//       o int, float, double, aString
//    as well as A++ arrays
//       o intArray, floatArray, doubleArray
//    as well as
//       o "c" arrays of Strings.
//
//  This implementation acts as a front end to the HDF Library from NCSA
//
//  Notes:
//    In HDF directories (nodes) in the hierarchy are called vgroups.
//    Every time we open a vgroup we need to close it. Therefore we keep
//   a list of vgroup's that are open and we keep a reference count of
//   how many data-bases are referencing that vgroup. When the reference
//   count goes to zero we can close the vgroup. The list is called dbList
//   and the nodes in the list are reference-counted objects of type
//   HDF_DataBaseRCData (RCdata=ReferenceCountedData). Every element in the
//   list will refer to a different vgroup so that the vgroup_id's will
//   be different for all elements in the list.
//   
//--------------------------------------------------------------------------

// Since we need to keep track of properly closing the Vgroup that was
// opened we keep a reference count to the vgroup using the following class:
class HDF_DataBaseRCData : public ReferenceCounting 
{
  friend class HDF_DataBase;
  int vgroup_id;
  // int vgroup_ref;

 public:
  HDF_DataBaseRCData(){};
  ~HDF_DataBaseRCData(){};
  
};


class DataBaseBuffer;

// include "tlist.h"
// typedef List<HDF_DataBaseRCData> ListOfHDF_DataBaseRCData;

class HDF_DataBase : public GenericDataBase
{
protected:
  int    file_id, sd_id;  // file identifiers, two types needed
  // dbList: a list of all the data-bases associated with a given file
  // we need this list so we can unmount the file
  ListOfHDF_DataBaseRCData *dbList;

  HDF_DataBaseRCData *rcData;
  
  enum modesOfAccess
  {
    none,
    write,
    read
  } accessMode;


  // close the directory, flush data
  int close();
  void reference( const HDF_DataBase & db );
  void breakReference();
  void destroy();

public:
  HDF_DataBase();
  HDF_DataBase(const HDF_DataBase &);
  HDF_DataBase(const GenericDataBase &);
  virtual GenericDataBase* virtualConstructor() const;
  ~HDF_DataBase();
  HDF_DataBase & operator=(const HDF_DataBase & );
  GenericDataBase & operator=(const GenericDataBase & );

  // open a data-base file
  int mount(const aString & fileName, const aString & flags = "I");
  // flush data, close the file
  int unmount();
  // flush the data to the file 
  int flush();
  
  virtual int getID() const;  // get the identifier for this directory

  // build a directory with the given ID
  virtual int build(GenericDataBase & db, int id);

  int isNull() const;

  // create a sub-directory
  int create(GenericDataBase & db, const aString & name, const aString & dirClassName );
  // find a sub-directory (crash if not found)
  int find(GenericDataBase & db, const aString & name, const aString & dirClassName ) const;
  // locate a sub-directory
  int locate(GenericDataBase & db, const aString & name, const aString & dirClassName ) const;

  // find the names of all objects with a given class-name
  virtual int find(aString *name, const aString & dirClassName, 
                      const int & maxNumber, int & actualNumber) const;
  // find all sub-directories (and their names) with a given class-name
  virtual int find(GenericDataBase *db, aString *name, const aString & dirClassName, 
                      const int & maxNumber, int & actualNumber) const;

  // put a float/int/double/aString
  int put( const float & x, const aString & name );
  int put( const double & x, const aString & name );
  int put( const int & x, const aString & name );
  int put( const aString & x, const aString & name );
#ifdef  OV_BOOL_DEFINED
  int put( const bool & x, const aString & name );
#endif

  // get a float/int/double/aString
  int get( float & x, const aString & name ) const;
  int get( double & x, const aString & name ) const;
  int get( int & x, const aString & name ) const;
  int get( aString & x, const aString & name ) const;
#ifdef  OV_BOOL_DEFINED
  int get( bool & x, const aString & name ) const;
#endif

  // put a float/int/double A++ array
  virtual int put( const floatSerialArray & x, const aString & name );
  virtual int put( const doubleSerialArray & x, const aString & name );
  virtual int put( const intSerialArray & x, const aString & name );
#ifdef USE_PPP
  virtual int put( const floatArray & x, const aString & name );
  virtual int put( const doubleArray & x, const aString & name );
  virtual int put( const intArray & x, const aString & name );
#endif

  // get a float/int/double A++ array
  virtual int get( floatSerialArray & x, const aString & name ) const;
  virtual int get( doubleSerialArray & x, const aString & name ) const;
  virtual int get( intSerialArray & x, const aString & name ) const;
#ifdef USE_PPP
  virtual int get( floatArray & x, const aString & name ) const;
  virtual int get( doubleArray & x, const aString & name ) const;
  virtual int get( intArray & x, const aString & name ) const;
#endif

  // put/get a "c" array of float/int/double/Strings 
  int put( const int    x[],const aString & name, const int number ); 
  int put( const float  x[],const aString & name, const int number ); 
  int put( const double x[],const aString & name, const int number ); 
  int put( const aString x[],const aString & name, const int number ); 

  int get( int    x[], const aString & name, const int number ) const;
  int get( float  x[], const aString & name, const int number ) const;
  int get( double x[], const aString & name, const int number ) const;
  int get( aString x[], const aString & name, const int number ) const;

  // output statistics on the file such as number of vgroups, vdatas
  void printStatistics() const;

  virtual void setMode(const InputOutputMode & mode=normalMode);

  int putSM( const floatArray & x, const aString & name );
  int getSM( floatArray & x, const aString & name );

//  int putSM( const float & x, const aString & name );
//  int getSM( float & x, const aString & name );

  
	
static int debug;

 protected:
  DataBaseBuffer *dataBaseBuffer;
  bool bufferWasCreatedInThisDirectory; // indicates when stream buffers were really opened
  void closeStream() const;  // close the stream buffers, if they are open (not really const!)

};


#endif
