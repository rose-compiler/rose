 
/* JH (01/03/2006): This is the class for managing the memory for the EasyStorage classes. 
   The memory is organized in blocks of length blockSize. The basic idea is 
   to store the data in a static memory pool and write this pool to disk. 
   Therefore, every memory object contains the position and the size of its 
   data in respect of the static memory pool. 
*/

#ifndef STORAGE_CLASS_MEMORY_MANAGEMENT_H
#define STORAGE_CLASS_MEMORY_MANAGEMENT_H

#define INITIAL_SIZE_OF_MEMORY_BLOCKS 10000
#define MEMORY_BLOCK_LIST_INCREASE 10
#define STORAGE_CLASS_MEMORY_MANAGEMENT_CHECK 1
#define FILE_IO_MARKER 1


template <class TYPE>
class StorageClassMemoryManagement
   {
    protected:
     // Position of the start of data in the static arrays
     unsigned long positionInStaticMemoryPool;
     // amount of data stored 
     long sizeOfData;
     // total amount of data already filled in the static arrays
     static unsigned long filledUpTo;
     /* size of the memory blocks. Remarks:
        * is actually set to INITIAL_SIZE_OF_MEMORY_BLOCKS and can be changed above  
        * is not a constant, since we set it to a new value before we rebuild the 
          data stored. Variable is changed by methods
          * arrangeMemoryPoolInOneBlock()
          * readFromFile (...)
         
     */
     static unsigned long blockSize;
     // the current working pool is located in actualBlock-1 ! 
     static unsigned int actualBlock;
     // number of memory blocks allocated 
     static unsigned int blocksAllocated;
     // the memory pool
     static TYPE **memoryBlockList;
     // pointer to the position that will be filled up next
     static TYPE *actual;

// protected  methods, only used by the derived classes! More comments can be found in 
// StorageClassMemoryManagement.C file 

  // method for initializing the working data before adding 
     long setPositionAndSizeAndReturnOffset (long sizeOfData_);

  // method that returns the a pointer beginning the beginning of the actual memory block
     TYPE* getBeginningOfActualBlock()  const;

  // method that returns the a pointer beginning the beginning of the data block where
  // the data was stored! Only used while rebuilding the data, enables several asserts 
  // and eases the implementation!
     TYPE* getBeginningOfDataBlock()  const;

  // "new-method": this method returns a new memory block. 
     TYPE* getNewMemoryBlock();

   public:
     StorageClassMemoryManagement () { positionInStaticMemoryPool = 0; sizeOfData = 0;  } 

  // "delete-method": delete the static memory pool. 
     static void deleteMemoryPool();

  // This method reorganizes the memory pool to contain all data in one memory block. 
      static void arrangeMemoryPoolInOneBlock();

  // return the amount of the data stored or to be stored
     long getSizeOfData() const;

  // DQ (2/27/2010): Changed name to something easier to find with grep.
  // print data that is stored, only used for debugging
  // void print () const;
     void displayStorageClassData () const;
  
  // method for writing the memoryBlockList to disk. 
     static void writeToFile(std::ostream& out);
 
  // reading the data from a file 
      static void readFromFile (std::istream& in); 
   };

/**************************************************************************************** 
   JH (01/11/2006) EasyStorage classes. These classes represent and declare the storage
   for the non-plain data in IRNodeStroge classes (located in StorageClasses.h/C).
   Remarks:
   * all class specializations have at least two methods
     * storeDataInEasyStorageClass 
     * rebuildDataStoredInEasyStorageClass
     to set and rebuild the data. However, they have the same name, they might have 
     slightly varying interface! 
   * in addition every class has the methods 
     * deleteMemoryPool
     * arrangeMemoryPoolInOneBlock
     and if not found in class body, it is inferited from StorageClassMemoryManagement.
   * there are some furter comments concerning the implemenation and working strategy 
     located in StorageClassMemoryManagement.C
   * the print function was only for debugging and is not located in every class it has 
     to be when called!
****************************************************************************************/

// Prototype for the EasyStorage classes, with no instantiation! 
template <class TYPE>
class EasyStorage;

template <class TYPE>
class EasyStorageList;


// EasyStorage for arrays of chars  
template<>
class EasyStorage<char*>
   : public StorageClassMemoryManagement<char>
   {
     typedef StorageClassMemoryManagement<char> Base;
    public:
     EasyStorage() {}
     void storeDataInEasyStorageClass(const char* data_);
     void storeDataInEasyStorageClass(const char* data_, int sizeOfData_);
     char* rebuildDataStoredInEasyStorageClass() const;
   };

// EasyStorage for the std::string class  
template <> 
class EasyStorage<std::string>  
   : public StorageClassMemoryManagement<char>
   {
     typedef StorageClassMemoryManagement<char> Base;
    public:
     EasyStorage() {}
     void storeDataInEasyStorageClass(const std::string& data_); 
     std::string rebuildDataStoredInEasyStorageClass() const;
   };

// EasyStorage for Sawyer bit vectors
template<>
class EasyStorage<Sawyer::Container::BitVector>: public StorageClassMemoryManagement<Sawyer::Container::BitVector::Word> {
    typedef StorageClassMemoryManagement<Sawyer::Container::BitVector::Word> Base;
public:
    EasyStorage() {}
    void storeDataInEasyStorageClass(const Sawyer::Container::BitVector &data_);
    Sawyer::Container::BitVector rebuildDataStoredInEasyStorageClass() const;
};

// EasyStorage concerning STL vectors and lists of plain data, not sets ! 
template <class BASIC_TYPE, template <class A> class CONTAINER >
class EasyStorage <CONTAINER<BASIC_TYPE> > 
   : public StorageClassMemoryManagement<BASIC_TYPE>
   {
     typedef StorageClassMemoryManagement<BASIC_TYPE> Base;
    public:
     EasyStorage() {}
     void storeDataInEasyStorageClass(const CONTAINER<BASIC_TYPE>& data_) ;
     CONTAINER<BASIC_TYPE> rebuildDataStoredInEasyStorageClass() const;
   };


// EasyStorage concerning STL vectors and lists of plain data, not sets ! 
template <class BASIC_TYPE>
class EasyStorage <std::list<BASIC_TYPE> > 
   : public StorageClassMemoryManagement<BASIC_TYPE>
   {
     typedef StorageClassMemoryManagement<BASIC_TYPE> Base;
    public:
     EasyStorage() {}
     void storeDataInEasyStorageClass(const std::list<BASIC_TYPE>& data_) ;
     std::list<BASIC_TYPE> rebuildDataStoredInEasyStorageClass() const;
   };


// EasyStorage concerning STL vectors and lists of plain data, not sets ! 
template <class BASIC_TYPE>
class EasyStorage <std::vector<BASIC_TYPE> > 
   : public StorageClassMemoryManagement<BASIC_TYPE>
   {
     typedef StorageClassMemoryManagement<BASIC_TYPE> Base;
    public:
     EasyStorage() {}
     void storeDataInEasyStorageClass(const std::vector<BASIC_TYPE>& data_) ;
     std::vector<BASIC_TYPE> rebuildDataStoredInEasyStorageClass() const;
   };

// EasyStorage concerning STL sets, separate implementation, due to insert instead of push_back ! 
template <class BASIC_TYPE >
class EasyStorage <std::set<BASIC_TYPE> > 
   : public StorageClassMemoryManagement<BASIC_TYPE>
   {
     typedef StorageClassMemoryManagement<BASIC_TYPE> Base;
    public:
     EasyStorage() {}
     void storeDataInEasyStorageClass(const std::set<BASIC_TYPE>& data_);
     std::set<BASIC_TYPE> rebuildDataStoredInEasyStorageClass() const;
   };

// EasyStorage concerning STL sets, separate implementation, due to insert instead of push_back ! 
template < >
class EasyStorage <SgBitVector> 
   : public StorageClassMemoryManagement<bool>
   {
     typedef StorageClassMemoryManagement<bool> Base;
    public:
     EasyStorage() {}
     void storeDataInEasyStorageClass(const SgBitVector& data_);
     SgBitVector rebuildDataStoredInEasyStorageClass() const;
   };

#if 0
// EasyStorage for storing a list or vector of std::strings 
template <template <class A> class CONTAINER >
class EasyStorage <CONTAINER<std::string> > 
   : public StorageClassMemoryManagement<EasyStorage<std::string> >
   {
     typedef StorageClassMemoryManagement<EasyStorage<std::string> > Base;
    public:
     EasyStorage() {}
     void storeDataInEasyStorageClass(const CONTAINER<std::string>& data_);
  // void print() ;
     void displayEasyStorageData ();
     CONTAINER<std::string> rebuildDataStoredInEasyStorageClass()  const;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);

   };
#endif

#if 0
// EasyStorage for storing a list or vector of std::strings 
template <>
class EasyStorage <std::list<std::string> > 
   : public StorageClassMemoryManagement<EasyStorage<std::string> >
   {
     typedef StorageClassMemoryManagement<EasyStorage<std::string> > Base;
    public:
     EasyStorage() {}
     void storeDataInEasyStorageClass(const std::list<std::string>& data_);
  // void print() ;
     void displayEasyStorageData ();
     std::list<std::string> rebuildDataStoredInEasyStorageClass()  const;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);

   };
#endif

#if 1
// DQ (9/25/2007): Added std::vector version as part of optimization of AST to use std::vector instead of std::list.

// EasyStorage for storing a list or vector of std::strings 
template <>
class EasyStorage <std::vector<std::string> > 
   : public StorageClassMemoryManagement<EasyStorage<std::string> >
   {
     typedef StorageClassMemoryManagement<EasyStorage<std::string> > Base;
    public:
     EasyStorage() {}
     void storeDataInEasyStorageClass(const std::vector<std::string>& data_);

  // void print();
     void displayEasyStorageData ();
         
     std::vector<std::string> rebuildDataStoredInEasyStorageClass()  const;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);

   };
#endif

// EasyStorage for storing only the string contained in an SgName 
// * it has overloaded methods for arrangeMemoryPoolInOneBlock and deleteMemoryPool
template < >
class EasyStorage <SgName> 
   {
    private:
     EasyStorage <std::string> name_data; 
    public: 
     EasyStorage() {}
  // Attention: has no still default constructor ...
     void storeDataInEasyStorageClass(const SgName& name);
     SgName rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };


// Prototype class for storing std::map entires, alias a pair!
template <class A, class B>
class EasyStorageMapEntry;
 
// EasyStorageMapEntry concerning an SgName and a Type T
// * the type T is a pain data type or a pointer address !
// * it has overloaded methods for arrangeMemoryPoolInOneBlock and deleteMemoryPool
template <class T>
class EasyStorageMapEntry <SgName,T> 
   {
    private:
     EasyStorage <SgName> sgNameString; 
     T globalIndex; 
    public: 
     EasyStorageMapEntry() {}
  // Attention: has no still default constructor ...
     void storeDataInEasyStorageClass(std::pair<const SgName, T>& iter);
     std::pair<const SgName,T>  rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };


// EasyStorage for storing the rose_hash_multimap  
// * it has overloaded methods for arrangeMemoryPoolInOneBlock and deleteMemoryPool
template < >
class EasyStorage <rose_hash_multimap*> : 
    public StorageClassMemoryManagement <EasyStorageMapEntry<SgName,SgSymbol*> > 
   {
     typedef StorageClassMemoryManagement <EasyStorageMapEntry<SgName,SgSymbol*> > Base;
    private:
     unsigned long parent;
    public: 
     EasyStorage() {parent = 0;}
     void storeDataInEasyStorageClass(rose_hash_multimap* data_);
     rose_hash_multimap* rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };

// #if !OLD_GRAPH_NODES


// DQ (8/19/2008): Added support for new graph IR nodes. 
// EasyStorage for storing the rose_hash_multimap  
// * it has overloaded methods for arrangeMemoryPoolInOneBlock and deleteMemoryPool
#if 0
template < >
class EasyStorage <rose_graph_hash_multimap*> : 
    public StorageClassMemoryManagement <EasyStorageMapEntry<std::string,SgGraphNode*> > 
   {
     typedef StorageClassMemoryManagement <EasyStorageMapEntry<std::string,SgGraphNode*> > Base;
    private:
     unsigned long parent;
    public: 
     EasyStorage() {parent = 0;}
     void storeDataInEasyStorageClass(rose_graph_hash_multimap* data_);
     rose_graph_hash_multimap* rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };
#else
template < >
class EasyStorage <rose_graph_hash_multimap> : 
    public StorageClassMemoryManagement <EasyStorageMapEntry<std::string,SgGraphNode*> > 
   {
     typedef StorageClassMemoryManagement <EasyStorageMapEntry<std::string,SgGraphNode*> > Base;
    private:
     unsigned long parent;
    public: 
     EasyStorage() {parent = 0;}
     void storeDataInEasyStorageClass(const rose_graph_hash_multimap& data_);
     rose_graph_hash_multimap rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };
#endif

// DQ (5/1/2009): Added support for rose_graph_integer_node_hash_map
template < >
class EasyStorage <rose_graph_integer_node_hash_map> : 
    public StorageClassMemoryManagement <EasyStorageMapEntry<int,SgGraphNode*> > 
   {
     typedef StorageClassMemoryManagement <EasyStorageMapEntry<int,SgGraphNode*> > Base;
    private:
  // DQ (3/25/2017): Remove to avoid Clang warning about unused private variable.
  // int value;
     unsigned long parent;
    public: 
     EasyStorage() {parent = 0;}
     void storeDataInEasyStorageClass(const rose_graph_integer_node_hash_map& data_);
     rose_graph_integer_node_hash_map rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };

// DQ (5/1/2009): Added support for rose_graph_integer_edge_hash_map
template < >
class EasyStorage <rose_graph_integer_edge_hash_map> : 
    public StorageClassMemoryManagement <EasyStorageMapEntry<int,SgGraphEdge*> > 
   {
     typedef StorageClassMemoryManagement <EasyStorageMapEntry<int,SgGraphEdge*> > Base;
    private:
  // DQ (3/25/2017): Remove to avoid Clang warning about unused private variable.
  // int value;
     unsigned long parent;
    public: 
     EasyStorage() {parent = 0;}
     void storeDataInEasyStorageClass(const rose_graph_integer_edge_hash_map& data_);
     rose_graph_integer_edge_hash_map rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };
 
// DQ (5/2/2009): Added support for rose_graph_integer_edge_hash_multimap
template < >
class EasyStorage <rose_graph_integer_edge_hash_multimap> : 
    public StorageClassMemoryManagement <EasyStorageMapEntry<int,SgGraphEdge*> > 
   {
     typedef StorageClassMemoryManagement <EasyStorageMapEntry<int,SgGraphEdge*> > Base;
    private:
  // DQ (3/25/2017): Remove to avoid Clang warning about unused private variable.
  // int value;
     unsigned long parent;
    public: 
     EasyStorage() {parent = 0;}
     void storeDataInEasyStorageClass(const rose_graph_integer_edge_hash_multimap& data_);
     rose_graph_integer_edge_hash_multimap rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };
 
// DQ (5/1/2009): Added support for rose_graph_string_integer_hash_multimap
template < >
class EasyStorage <rose_graph_string_integer_hash_multimap> : 
    public StorageClassMemoryManagement <EasyStorageMapEntry<std::string,int> > 
   {
     typedef StorageClassMemoryManagement <EasyStorageMapEntry<std::string,int> > Base;
    private:
     EasyStorage <std::string> name_data; 
  // DQ (3/25/2017): Remove to avoid Clang warning about unused private variable.
  // int value;
    public: 
     EasyStorage() {}
     void storeDataInEasyStorageClass(const rose_graph_string_integer_hash_multimap& data_);
     rose_graph_string_integer_hash_multimap rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };
 
// DQ (5/1/2009): Added support for rose_graph_integerpair_edge_hash_multimap
template < >
class EasyStorage <rose_graph_integerpair_edge_hash_multimap> : 
    public StorageClassMemoryManagement <EasyStorageMapEntry<EasyStorageMapEntry<int,int>,SgGraphEdge*> > 
   {
     typedef StorageClassMemoryManagement <EasyStorageMapEntry<EasyStorageMapEntry<int,int>,SgGraphEdge*> > Base;
    private:
  // EasyStorage <std::string> name_data; 

  // DQ (3/25/2017): Remove to avoid Clang warning about unused private variable.
  // int value;
    public: 
     EasyStorage() {}
     void storeDataInEasyStorageClass(const rose_graph_integerpair_edge_hash_multimap & data_);
     rose_graph_integerpair_edge_hash_multimap rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock();
     static void deleteMemoryPool();

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };
 
// DQ (8/19/2008): Added support for new graph IR nodes. 
// EasyStorage for storing the rose_hash_multimap  
// * it has overloaded methods for arrangeMemoryPoolInOneBlock and deleteMemoryPool
#if 0
// DQ (4/30/2009): Removed these in favor of the hash_multimap using the SgGraphEdge class.
template < >
class EasyStorage <rose_directed_graph_hash_multimap> : 
    public StorageClassMemoryManagement <EasyStorageMapEntry<std::string,SgDirectedGraphEdge*> > 
   {
     typedef StorageClassMemoryManagement <EasyStorageMapEntry<std::string,SgDirectedGraphEdge*> > Base;
    private:
     unsigned long parent;
    public: 
     EasyStorage() {parent = 0;}
     void storeDataInEasyStorageClass(const rose_directed_graph_hash_multimap& data_);
     rose_directed_graph_hash_multimap rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };
#endif

// DQ (8/19/2008): Added support for new graph IR nodes. 
// EasyStorage for storing the rose_hash_multimap  
// * it has overloaded methods for arrangeMemoryPoolInOneBlock and deleteMemoryPool
#if 0
// DQ (4/30/2009): Removed these in favor of the hash_multimap using the SgGraphEdge class.
// DQ (4/25/2009): Remove the pointer to improve the interface...
template < >
class EasyStorage <rose_undirected_graph_hash_multimap> : 
    public StorageClassMemoryManagement <EasyStorageMapEntry<std::string,SgUndirectedGraphEdge*> > 
   {
     typedef StorageClassMemoryManagement <EasyStorageMapEntry<std::string,SgUndirectedGraphEdge*> > Base;
    private:
     unsigned long parent;
    public: 
     EasyStorage() {parent = 0;}
     void storeDataInEasyStorageClass(const rose_undirected_graph_hash_multimap& data_);
     rose_undirected_graph_hash_multimap rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };
#endif

// DQ (4/27/2009): Added type to support maps from nodes to edges...
template < >
class EasyStorage <rose_graph_node_edge_hash_multimap> : 
    public StorageClassMemoryManagement <EasyStorageMapEntry<SgGraphNode*,SgGraphEdge*> > 
   {
     typedef StorageClassMemoryManagement <EasyStorageMapEntry<SgGraphNode*,SgGraphEdge*> > Base;
    private:
     unsigned long parent;
    public: 
     EasyStorage() {parent = 0;}
     void storeDataInEasyStorageClass(const rose_graph_node_edge_hash_multimap& data_);
     rose_graph_node_edge_hash_multimap rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };



/* EasyStorageMapEntry concerning an std::string and an AstAttribute
  Remarks: 
*/
template <>
class EasyStorageMapEntry <std::string,AstAttribute*> 
   {
    private:
     EasyStorage <std::string> mapString; 
     EasyStorage <char*> attributeData; 
     EasyStorage <std::string> attributeName; 
    public: 
     EasyStorageMapEntry () {} 
     void storeDataInEasyStorageClass (const std::string& name, AstAttribute* attr);
     std::pair<std::string, AstAttribute*>  rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };


// EasyStorage for storing the AstAttributeMechanism
// * it has overloaded methods for arrangeMemoryPoolInOneBlock and deleteMemoryPool
template < >
class EasyStorage <AstAttributeMechanism*> : 
    public StorageClassMemoryManagement < EasyStorageMapEntry<std::string,AstAttribute*> > 
   {
     typedef StorageClassMemoryManagement < EasyStorageMapEntry<std::string,AstAttribute*> > Base;
    public: 
     void storeDataInEasyStorageClass(AstAttributeMechanism* data_);
  // is only used while using the old ROSE ....
  // void rebuildDataStoredInEasyStorageClass(AstAttributeMechanism& attr) ; 
     AstAttributeMechanism* rebuildDataStoredInEasyStorageClass( ) const ;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };

// EasyStorage for storing the PreprocessingInfo
template <>
class EasyStorage<PreprocessingInfo*>  : 
      public StorageClassMemoryManagement <char>
   {
     typedef StorageClassMemoryManagement <char> Base;
  // JH (04/21/2006) : Adding an unsigned long for the additional pointer
     unsigned long fileInfoIndex;

    public: 
     void storeDataInEasyStorageClass(PreprocessingInfo* info);
     PreprocessingInfo* rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };


// EasyStorage for storing a stl-like container of ProprocessingInfo
template <template <class A> class CONTAINER >
class EasyStorage <CONTAINER<PreprocessingInfo*> > 
   : public StorageClassMemoryManagement<EasyStorage<PreprocessingInfo*> >
   {
     typedef StorageClassMemoryManagement<EasyStorage<PreprocessingInfo*> > Base;
    public:
     EasyStorage() {}
     void storeDataInEasyStorageClass(const CONTAINER<PreprocessingInfo*>& data_);
  // void print() ;
     void displayEasyStorageData ();
     CONTAINER<PreprocessingInfo*> rebuildDataStoredInEasyStorageClass() const ;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);

   };


// EasyStorage for storing a vector of PreprocessingInfo
template <>
class EasyStorage <std::vector<PreprocessingInfo*> > 
   : public StorageClassMemoryManagement<EasyStorage<PreprocessingInfo*> >
   {
     typedef StorageClassMemoryManagement<EasyStorage<PreprocessingInfo*> > Base;
    public:
     EasyStorage() {}
     void storeDataInEasyStorageClass(const std::vector<PreprocessingInfo*>& data_);
  // void print() ;
     void displayEasyStorageData ();
     std::vector<PreprocessingInfo*> rebuildDataStoredInEasyStorageClass() const ;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);

   };


// EasyStorage for storing AttachedProprocessingInfoType
template <>
class EasyStorage <AttachedPreprocessingInfoType*> 
   : public StorageClassMemoryManagement<EasyStorage<PreprocessingInfo*> >
   {
     typedef StorageClassMemoryManagement<EasyStorage<PreprocessingInfo*> > Base;
    public:
     EasyStorage() {}
     void storeDataInEasyStorageClass(AttachedPreprocessingInfoType* data_);
  // void print() ;
     void displayEasyStorageData ();
     AttachedPreprocessingInfoType* rebuildDataStoredInEasyStorageClass() const ;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);

   };

// EasyStorage for storing the ROSEAttributesList (just a vector of PreprocessingInfo*)
// * it is not inherited, has its own arrangeMemoryPoolInOneBlock and deleteMemoryPool
template <>
class EasyStorage<ROSEAttributesList> 
   {
    private:
     EasyStorage < std::vector<PreprocessingInfo*> > preprocessingInfoVector; 
     EasyStorage < std::string > fileNameString;
     int index;
    public: 
     void storeDataInEasyStorageClass ( ROSEAttributesList* info);
     ROSEAttributesList* rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock();
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };

#if 0  // still in use in this version
// EasyStorage for ROSEAttributesListContainerPtr (PreprocessingInfo*)
// * it has overloaded methods for arrangeMemoryPoolInOneBlock and deleteMemoryPool
template <>
class EasyStorage <ROSEAttributesListContainerPtr> 
   : public StorageClassMemoryManagement< EasyStorage<ROSEAttributesList> >
   {
     typedef StorageClassMemoryManagement< EasyStorage<ROSEAttributesList> > Base;
    public:
     void storeDataInEasyStorageClass(ROSEAttributesListContainerPtr data_) ;
     ROSEAttributesListContainerPtr rebuildDataStoredInEasyStorageClass() const ;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
};
#endif 

// EasyStorageMapEntry concerning a std::string and a RoseAttributeList
// * it has overloaded methods for arrangeMemoryPoolInOneBlock and deleteMemoryPool
template < >
class EasyStorageMapEntry <std::string,ROSEAttributesList> 
   {
    private:
     EasyStorage <std::string> nameString; 
     EasyStorage<ROSEAttributesList> attributesList; 
    public: 
     EasyStorageMapEntry () {}
     void storeDataInEasyStorageClass(const std::pair<std::string, ROSEAttributesList*>& iter);
     std::pair<std::string, ROSEAttributesList*>  rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };

/* new EasyStorageClass implementation for the new verion of the ROSEAttributesListContainer  
 * Use this one, and comment out the other one!
 */
#if 1
// EasyStorage for ROSEAttributesListContainerPtr (PreprocessingInfo*)
// * it has overloaded methods for arrangeMemoryPoolInOneBlock and deleteMemoryPool
template <>
class EasyStorage <ROSEAttributesListContainerPtr> 
   : public StorageClassMemoryManagement< EasyStorageMapEntry<std::string,ROSEAttributesList> >
   {
     typedef StorageClassMemoryManagement< EasyStorageMapEntry<std::string,ROSEAttributesList> > Base;
    public:
     void storeDataInEasyStorageClass(ROSEAttributesListContainerPtr data_) ;
     ROSEAttributesListContainerPtr rebuildDataStoredInEasyStorageClass() const ;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
};
#endif

// EasyStorage for storing a set of PreprocessingInfo*
template < >
class EasyStorage <std::set<PreprocessingInfo*> > 
   : public StorageClassMemoryManagement<EasyStorage<PreprocessingInfo*> >
   {
     typedef StorageClassMemoryManagement<EasyStorage<PreprocessingInfo*> > Base;
    public:
     EasyStorage() {}
     void storeDataInEasyStorageClass(const std::set<PreprocessingInfo*>& data_);
     void displayEasyStorageData ();
     std::set<PreprocessingInfo*> rebuildDataStoredInEasyStorageClass() const ;
     static void arrangeMemoryPoolInOneBlock();
     static void deleteMemoryPool();

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);

   };

template < >
class EasyStorageMapEntry <std::string, std::set<PreprocessingInfo*> > 
   {
    private:
     EasyStorage <std::string> nameString; 
     EasyStorage <std::set<PreprocessingInfo*> > preprocessingInfoSet; 
    public: 
     EasyStorageMapEntry () {}
     void storeDataInEasyStorageClass(const std::pair<std::string, std::set<PreprocessingInfo*> >& iter);
     std::pair<std::string, std::set<PreprocessingInfo*> >  rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;
        
     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };

// EasyStorage for a map of sets of PreprocessingInfo*
template < >
class EasyStorage <std::map<std::string, std::set<PreprocessingInfo*> > > 
   : public StorageClassMemoryManagement< EasyStorageMapEntry<std::string, std::set<PreprocessingInfo*> > >
   {
     typedef StorageClassMemoryManagement< EasyStorageMapEntry<std::string, std::set<PreprocessingInfo*> > > Base;
    public:
     void storeDataInEasyStorageClass(const std::map<std::string, std::set<PreprocessingInfo*> >& data) ;
     std::map<std::string, std::set<PreprocessingInfo*> > rebuildDataStoredInEasyStorageClass() const ;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;
        
     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
};

// EasyStorageMapEntry concerning an SgName and a Type T
// * it has overloaded methods for arrangeMemoryPoolInOneBlock and deleteMemoryPool
template < >
class EasyStorageMapEntry <std::string,int> 
   {
    private:
     EasyStorage <std::string> nameString; 
     int index; 
    public: 
     EasyStorageMapEntry () { index = 0 ; }
     void storeDataInEasyStorageClass(const std::pair<std::string, const int >& iter);
     std::pair<std::string, int >  rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };


// EasyStorageMapEntry concerning an SgName and a Type T
// * it has overloaded methods for arrangeMemoryPoolInOneBlock and deleteMemoryPool
template < >
class EasyStorageMapEntry <int, std::string> 
   {
    private:
     int index; 
     EasyStorage <std::string> nameString;
    public: 
     EasyStorageMapEntry () { index = 0 ; }
     void storeDataInEasyStorageClass(const std::pair<const int, std::string>& iter);
     std::pair<int, std::string>  rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };



// EasyStorage for ROSEAttributesListContainerPtr (PreprocessingInfo*)
// * it has overloaded methods for arrangeMemoryPoolInOneBlock and deleteMemoryPool
template <>
class EasyStorage < std::map<int,std::string> > 
   : public StorageClassMemoryManagement< EasyStorageMapEntry<int, std::string> >
   {
     typedef StorageClassMemoryManagement< EasyStorageMapEntry<int, std::string> > Base;
    public:
     void storeDataInEasyStorageClass(const std::map<int,std::string>& data_);
     std::map<int,std::string> rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock();
     static void deleteMemoryPool();

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };

#if 1
// DQ (5/1/2009): Added support to this as required for support of rose_graph_integer_node_hash_map
// DQ (4/30/2009): Added support for std::map<int,SgGraphNode*>
template < >
class EasyStorageMapEntry <int, SgGraphNode*> 
   {
    private:
     int index; 
     int global_id; // this is the global id for the SgGraphNode*
    public: 
     EasyStorageMapEntry () { index = 0 ; }
     void storeDataInEasyStorageClass(const std::pair<const int, SgGraphNode*>& iter);
     std::pair<int, SgGraphNode*>  rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };
#endif

#if 0
// DQ (4/30/2009): Added support for std::map<int,SgGraphNode*>
// EasyStorage for ROSEAttributesListContainerPtr (PreprocessingInfo*)
// * it has overloaded methods for arrangeMemoryPoolInOneBlock and deleteMemoryPool
template <>
class EasyStorage < std::map<int,SgGraphNode*> > 
   : public StorageClassMemoryManagement< EasyStorageMapEntry<int, SgGraphNode*> >
   {
     typedef StorageClassMemoryManagement< EasyStorageMapEntry<int, SgGraphNode*> > Base;
    public:
     void storeDataInEasyStorageClass(const std::map<int,SgGraphNode*>& data_);
     std::map<int,SgGraphNode*> rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock();
     static void deleteMemoryPool();

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };
#endif

#if 1
// DQ (5/1/2009): Added support to this as required for support of rose_graph_integer_node_hash_map
// DQ (4/30/2009): Added support for std::map<int,SgGraphEdge*>
template < >
class EasyStorageMapEntry <int, SgGraphEdge*> 
   {
    private:
     int index; 
     int global_id; // this is the global id for the SgGraphEdge*
    public: 
     EasyStorageMapEntry () { index = 0 ; }
     void storeDataInEasyStorageClass(const std::pair<const int, SgGraphEdge*>& iter);
     std::pair<int, SgGraphEdge*>  rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };
#endif

#if 0
// DQ (4/30/2009): Added support for std::map<int,SgGraphEdge*>
// EasyStorage for ROSEAttributesListContainerPtr (PreprocessingInfo*)
// * it has overloaded methods for arrangeMemoryPoolInOneBlock and deleteMemoryPool
template <>
class EasyStorage < std::map<int,SgGraphEdge*> > 
   : public StorageClassMemoryManagement< EasyStorageMapEntry<int, SgGraphEdge*> >
   {
     typedef StorageClassMemoryManagement< EasyStorageMapEntry<int, SgGraphEdge*> > Base;
    public:
     void storeDataInEasyStorageClass(const std::map<int,SgGraphEdge*>& data_);
     std::map<int,SgGraphEdge*> rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock();
     static void deleteMemoryPool();

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };
#endif


// EasyStorage for ROSEAttributesListContainerPtr (PreprocessingInfo*)
// * it has overloaded methods for arrangeMemoryPoolInOneBlock and deleteMemoryPool
template <>
class EasyStorage < std::map<std::string, int> > 
   : public StorageClassMemoryManagement< EasyStorageMapEntry<std::string, int> >
   {
     typedef StorageClassMemoryManagement< EasyStorageMapEntry<std::string, int> > Base;
    public:
     void storeDataInEasyStorageClass(const std::map<std::string,int>& data_);
     std::map<std::string,int> rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock();
     static void deleteMemoryPool();

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };

#if 0
// DQ (4/30/2009): Added std::multimap<std::string, int> type to ROSETTA (expressed using EasyStorageMapEntry). 
// EasyStorage for ROSEAttributesListContainerPtr (PreprocessingInfo*)
// * it has overloaded methods for arrangeMemoryPoolInOneBlock and deleteMemoryPool
template <>
class EasyStorage < std::multimap<std::string, int> > 
   : public StorageClassMemoryManagement< EasyStorageMapEntry<std::string, int> >
   {
     typedef StorageClassMemoryManagement< EasyStorageMapEntry<std::string, int> > Base;
    public:
     void storeDataInEasyStorageClass(const std::multimap<std::string,int>& data_);
     std::multimap<std::string,int> rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock();
     static void deleteMemoryPool();

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };
#endif

// DQ (10/4/2006): Added to support maping of SgNode* to integers for name namgling support
// EasyStorageMapEntry concerning an SgName and a Type T
// * it has overloaded methods for arrangeMemoryPoolInOneBlock and deleteMemoryPool
template < >
class EasyStorageMapEntry <SgNode*,int> 
   {
    private:
  // DQ: Not sure how to convert this to a global id number
  // EasyStorage <SgNode*> nameString;
     int global_id; // this is the global id for the SgNode*
     int index; // this is the index
    public: 
     EasyStorageMapEntry () { global_id = 0; index = 0 ; }
     void storeDataInEasyStorageClass(const std::pair<SgNode*, const int >& iter);
     std::pair<SgNode*, int >  rebuildDataStoredInEasyStorageClass() const;
  // std::pair<unsigned long, int >  rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };


// DQ (10/4/2006): Added to support maping of SgNode* to integers for name namgling support
// EasyStorage for ROSEAttributesListContainerPtr (PreprocessingInfo*)
// * it has overloaded methods for arrangeMemoryPoolInOneBlock and deleteMemoryPool
template <>
class EasyStorage < std::map<SgNode*, int> > 
   : public StorageClassMemoryManagement< EasyStorageMapEntry<SgNode*, int> >
   {
     typedef StorageClassMemoryManagement< EasyStorageMapEntry<SgNode*, int> > Base;
    public:
     void storeDataInEasyStorageClass(const std::map<SgNode*,int>& data_);
     std::map<SgNode*,int> rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock();
     static void deleteMemoryPool();

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };

// DQ (10/6/2006): Added to support mapping of SgNode* to strings (std::string) for name mangling support.
// EasyStorageMapEntry concerning an SgName and a Type T
// * it has overloaded methods for arrangeMemoryPoolInOneBlock and deleteMemoryPool
template < >
class EasyStorageMapEntry <SgNode*,std::string> 
   {
     private:
       // DQ: Not sure how to convert this to a global id number
          int global_id; // this is the global id for the SgNode*
          EasyStorage < std::string > nameString; // this is the mangled name
    public: 
          EasyStorageMapEntry () { global_id = 0; }
          void storeDataInEasyStorageClass(const std::pair<SgNode*, const int >& iter);
          std::pair<SgNode*, std::string >  rebuildDataStoredInEasyStorageClass() const;
          static void arrangeMemoryPoolInOneBlock() ;
          static void deleteMemoryPool() ;

          static void writeToFile(std::ostream& out);
          static void readFromFile (std::istream& in);
   };


// DQ (10/6/2006): Added to support mapping of SgNode* to strings for name mangling support
// EasyStorage for ROSEAttributesListContainerPtr (PreprocessingInfo*)
// * it has overloaded methods for arrangeMemoryPoolInOneBlock and deleteMemoryPool
template <>
class EasyStorage < std::map<SgNode*, std::string> > 
   : public StorageClassMemoryManagement< EasyStorageMapEntry<SgNode*, std::string> >
   {
     typedef StorageClassMemoryManagement< EasyStorageMapEntry<SgNode*, std::string> > Base;
    public:
     void storeDataInEasyStorageClass(const std::map<SgNode*,std::string>& data_);
     std::map<SgNode*,std::string> rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock();
     static void deleteMemoryPool();

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };

// DQ (3/13/2019): Added to support mapping of SgNode* to a map of SgNode* to strings for name mangling support
// EasyStorage for ROSEAttributesListContainerPtr (PreprocessingInfo*)
// * it has overloaded methods for arrangeMemoryPoolInOneBlock and deleteMemoryPool
template <>
class EasyStorage < std::map<SgNode*, std::map<SgNode*, std::string> > > 
   : public StorageClassMemoryManagement< EasyStorageMapEntry<SgNode*, std::map<SgNode*, std::string> > >
   {
     typedef StorageClassMemoryManagement< EasyStorageMapEntry<SgNode*, std::map<SgNode*, std::string> > > Base;
    public:
     void storeDataInEasyStorageClass(const std::map<SgNode*,std::map<SgNode*, std::string> >& data_);
     std::map<SgNode*,std::map<SgNode*,std::string> > rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock();
     static void deleteMemoryPool();

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };

// Liao 1/23/2013, placeholder for storing std::map <SgSymbol*, std::vector <std::pair <SgExpression*, SgExpression*> > >
// this is used for representing array dimension information of the map clause.
// TODO: provide real storage support once the OpenMP Accelerator Model is standardized.
template < >
class EasyStorageMapEntry <SgSymbol*, std::vector <std::pair <SgExpression*, SgExpression*> > > 
   {
     private:
          int global_id; // this is the global id for the SgNode*
          EasyStorage < std::string > nameString; // this is the mangled name
    public: 
          EasyStorageMapEntry () { global_id = 0; }
          void storeDataInEasyStorageClass(const std::pair<SgNode*, const int >& iter) {  };
          std::pair<SgSymbol*, std::vector <std::pair <SgExpression*, SgExpression*> > >  rebuildDataStoredInEasyStorageClass() const {
            std::vector <std::pair <SgExpression*, SgExpression*> > vec;
            SgSymbol* s = NULL; 
            return std::make_pair (s, vec);
            };
          static void arrangeMemoryPoolInOneBlock(){} ;
          static void deleteMemoryPool(){} ;

          static void writeToFile(std::ostream& out) {};
          static void readFromFile (std::istream& in) {};
   };

template <>
class EasyStorage < std::map< SgSymbol*,  std::vector < std::pair <SgExpression*, SgExpression*> > > > 
   : public StorageClassMemoryManagement< EasyStorageMapEntry< SgSymbol*, std::vector <std::pair <SgExpression*, SgExpression*> > > >
   {
     typedef StorageClassMemoryManagement< EasyStorageMapEntry<SgSymbol*, std::vector <std::pair <SgExpression*, SgExpression*> > > > Base;
    public:
     void storeDataInEasyStorageClass(const std::map<SgSymbol*, std::vector <std::pair <SgExpression*, SgExpression*> > >& data_) 
     {  };
     std::map< SgSymbol*, std::vector <std::pair <SgExpression*, SgExpression*> > > rebuildDataStoredInEasyStorageClass() const
     {
       std::map< SgSymbol*, std::vector <std::pair <SgExpression*, SgExpression*> > > rt;
       return rt;
     };
     static void arrangeMemoryPoolInOneBlock() {};
     static void deleteMemoryPool(){ };

     static void writeToFile(std::ostream& out) { };
     static void readFromFile (std::istream& in) {};
   };

// Liao 11/11/2015,  dist_data policy information associated with SgOmpMapClause

template < >
class EasyStorageMapEntry <SgOmpClause::omp_map_dist_data_enum, SgExpression*> 
   {
    private:
     SgOmpClause::omp_map_dist_data_enum dd_enum; 
     SgExpression* exp; 
    public: 
     EasyStorageMapEntry () { dd_enum = (SgOmpClause::omp_map_dist_data_enum) 0 ; exp =NULL; }
     void storeDataInEasyStorageClass(const std::pair<SgOmpClause::omp_map_dist_data_enum, const SgExpression* >& iter);
     std::pair<SgOmpClause::omp_map_dist_data_enum, SgExpression* >  rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };


template <>
class EasyStorage < std::map< SgSymbol*,  std::vector < std::pair <SgOmpClause::omp_map_dist_data_enum, SgExpression*> > > > 
   : public StorageClassMemoryManagement< EasyStorageMapEntry< SgSymbol*, std::vector <std::pair <SgOmpClause::omp_map_dist_data_enum, SgExpression*> > > >
   {
     typedef StorageClassMemoryManagement< EasyStorageMapEntry<SgSymbol*, std::vector <std::pair <SgOmpClause::omp_map_dist_data_enum, SgExpression*> > > > Base;
    public:
     void storeDataInEasyStorageClass(const std::map<SgSymbol*, std::vector <std::pair <SgOmpClause::omp_map_dist_data_enum, SgExpression*> > >& data_) 
     {  };
     std::map< SgSymbol*, std::vector <std::pair <SgOmpClause::omp_map_dist_data_enum, SgExpression*> > > rebuildDataStoredInEasyStorageClass() const
     {
       std::map< SgSymbol*, std::vector <std::pair <SgOmpClause::omp_map_dist_data_enum, SgExpression*> > > rt;
       return rt;
     };
     static void arrangeMemoryPoolInOneBlock() {};
     static void deleteMemoryPool(){ };

     static void writeToFile(std::ostream& out) { };
     static void readFromFile (std::istream& in) {};
   };



/** Maps SgSharedVector to/from file representation. This is almost exactly the same as the
 *  vector of Sg object pointers specialization except the rebuildDataStoredInEasyStorageClass() constructs the SgSharedVector
 *  in a different manner. In an original AST all SgSharedVector objects probably pointed to a common underlying storage pool
 *  which was the contents of the entire binary file.  This sharing is currently lost in the file representation and the
 *  reconstructed SgSharedVector objects will each get their own pool. FIXME [RPM 2010-06-15] */
template<class BASIC_TYPE>
class EasyStorage<SgSharedVector<BASIC_TYPE> >: public StorageClassMemoryManagement<BASIC_TYPE> {
    typedef StorageClassMemoryManagement<BASIC_TYPE> Base;
  public:
    EasyStorage() {}
    void storeDataInEasyStorageClass(const SgSharedVector<BASIC_TYPE>& data_);
    SgSharedVector<BASIC_TYPE> rebuildDataStoredInEasyStorageClass() const;
};

/** Maps an ExtentMap to/from file representation. */
template<>
class EasyStorage<ExtentMap>: public StorageClassMemoryManagement<rose_addr_t> {
    typedef StorageClassMemoryManagement<rose_addr_t> Base;
  public:
    EasyStorage() {}
    void storeDataInEasyStorageClass(const ExtentMap&);
    ExtentMap rebuildDataStoredInEasyStorageClass() const;
};

/** Maps an AddressIntervalSet to/from file representation. */
template<>
class EasyStorage<AddressIntervalSet>: public StorageClassMemoryManagement<rose_addr_t> {
    typedef StorageClassMemoryManagement<rose_addr_t> Base;
  public:
    EasyStorage() {}
    void storeDataInEasyStorageClass(const AddressIntervalSet&);
    AddressIntervalSet rebuildDataStoredInEasyStorageClass() const;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif  // STORAGE_CLASS_MEMORY_MANAGEMENT_H
