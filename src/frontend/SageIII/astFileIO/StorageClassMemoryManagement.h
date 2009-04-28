 
/* JH (01/03/2006): This is the class for managing the memory for the EasyStorage classes. 
   The memory is organized in blocks of length blockSize. The basic idea is 
   to store the data in a static memory pool and write this pool to disk. 
   Threrefore, every memory object contains the position and the size of its 
   data in resspect of the static memory pool. 
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
        * is acutally set to INITIAL_SIZE_OF_MEMORY_BLOCKS and can be changed above  
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

// protected  methods, only used by the derived classes! More commets can be found in 
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

  // print data that is stored, only used for debugging
     void print () const;
  
  // method for writing the memoryBlockList to disk. 
     static void writeToFile(std::ostream& out);
 
  // reading the data from a file 
      static void readFromFile (std::istream& in); 
   };

/**************************************************************************************** 
   JH (01/11/2006) EasyStorage classes. These classes represent and declare the storage
   for the non-plain data in IRNodeStroge classes (located in StorageClasses.h/C).
   Remarks:
   * all class specialtizations have at least two methods
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

// Prototype for the EasyStorage classes, with no instatntiation! 
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

// EasyStorage concerning STL sets, separate imlementation, due to insert insead of push_back ! 
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

// EasyStorage concerning STL sets, separate imlementation, due to insert insead of push_back ! 
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
     void print() ;
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
     void print() ;
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
     void print() ;
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
#ifdef ROSE_USE_NEW_GRAPH_NODES

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
 
// DQ (8/19/2008): Added support for new graph IR nodes. 
// EasyStorage for storing the rose_hash_multimap  
// * it has overloaded methods for arrangeMemoryPoolInOneBlock and deleteMemoryPool
#if 0
template < >
class EasyStorage <rose_directed_graph_hash_multimap*> : 
    public StorageClassMemoryManagement <EasyStorageMapEntry<std::string,SgDirectedGraphEdge*> > 
   {
     typedef StorageClassMemoryManagement <EasyStorageMapEntry<std::string,SgDirectedGraphEdge*> > Base;
    private:
     unsigned long parent;
    public: 
     EasyStorage() {parent = 0;}
     void storeDataInEasyStorageClass(rose_directed_graph_hash_multimap* data_);
     rose_directed_graph_hash_multimap* rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };
#else
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
template < >
class EasyStorage <rose_undirected_graph_hash_multimap*> : 
    public StorageClassMemoryManagement <EasyStorageMapEntry<std::string,SgUndirectedGraphEdge*> > 
   {
     typedef StorageClassMemoryManagement <EasyStorageMapEntry<std::string,SgUndirectedGraphEdge*> > Base;
    private:
     unsigned long parent;
    public: 
     EasyStorage() {parent = 0;}
     void storeDataInEasyStorageClass(rose_undirected_graph_hash_multimap* data_);
     rose_undirected_graph_hash_multimap* rebuildDataStoredInEasyStorageClass() const;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);
   };
#else
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

#endif

/* EasyStorageMapEntry concerning an std::string and an AstAttribut
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


// EasyStorage for storing a list or vector of std::strings 
template <template <class A> class CONTAINER >
class EasyStorage <CONTAINER<PreprocessingInfo*> > 
   : public StorageClassMemoryManagement<EasyStorage<PreprocessingInfo*> >
   {
     typedef StorageClassMemoryManagement<EasyStorage<PreprocessingInfo*> > Base;
    public:
     EasyStorage() {}
     void storeDataInEasyStorageClass(const CONTAINER<PreprocessingInfo*>& data_);
     void print() ;
     CONTAINER<PreprocessingInfo*> rebuildDataStoredInEasyStorageClass() const ;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);

   };


// EasyStorage for storing a list or vector of std::strings 
template <>
class EasyStorage <std::vector<PreprocessingInfo*> > 
   : public StorageClassMemoryManagement<EasyStorage<PreprocessingInfo*> >
   {
     typedef StorageClassMemoryManagement<EasyStorage<PreprocessingInfo*> > Base;
    public:
     EasyStorage() {}
     void storeDataInEasyStorageClass(const std::vector<PreprocessingInfo*>& data_);
     void print() ;
     std::vector<PreprocessingInfo*> rebuildDataStoredInEasyStorageClass() const ;
     static void arrangeMemoryPoolInOneBlock() ;
     static void deleteMemoryPool() ;

     static void writeToFile(std::ostream& out);
     static void readFromFile (std::istream& in);

   };


// EasyStorage for storing a list or vector of std::strings 
template <>
class EasyStorage <AttachedPreprocessingInfoType*> 
   : public StorageClassMemoryManagement<EasyStorage<PreprocessingInfo*> >
   {
     typedef StorageClassMemoryManagement<EasyStorage<PreprocessingInfo*> > Base;
    public:
     EasyStorage() {}
     void storeDataInEasyStorageClass(AttachedPreprocessingInfoType* data_);
     void print() ;
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

// DQ (10/6/2006): Added to support maping of SgNode* to strings (std::string) for name namgling support.
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


// DQ (10/6/2006): Added to support maping of SgNode* to strings for name namgling support
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



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif  // STORAGE_CLASS_MEMORY_MANAGEMENT_H
