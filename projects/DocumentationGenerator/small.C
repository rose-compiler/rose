// #include "rose.h"
// #include <vector>

typedef unsigned int size_t;

namespace std
{
   class string {};
}

class SgNode {};
class SgType {};
class SgExpression {};
class SgCopyHelp {};
#define ROSE_DEPRECATED_FUNCTION
class Sg_File_Info {};
#define NULL 0L

// Class Definition for SgUnaryOp
class SgUnaryOp : public SgExpression
   {
     public:

      virtual SgNode* copy ( SgCopyHelp& help) const; 

          int length() const ROSE_DEPRECATED_FUNCTION;
          bool empty() const ROSE_DEPRECATED_FUNCTION;
       // void operator_unparse( const char *, Unparse_Info &, ostream & );

          SgExpression *get_next( int& n) const ROSE_DEPRECATED_FUNCTION;
          int replace_expression( SgExpression *, SgExpression *) ROSE_DEPRECATED_FUNCTION;


          SgExpression* get_operand() const;
          void set_operand (SgExpression * exp);
          SgType* get_type() const;

       // Enums are not properly scoped in the class declaration and are assigned 
       // to a global scope so we have to change the name to make it unique across 
       // different grammars
      /*! \brief Enum value defines operators as prefix or postfix, as appropriate, e.g. operator++().

          This enum type provides support for specification of prefix/postfix operators such 
          as operator++() using a simpler mechanism thatn that provided by C++.  See C++
          standard for details of specification of prefix and postfix operators.

          \internal There should be an additional value added to support unknown setting (with value 0)
       */
          enum Sgop_mode
             {
               prefix=0 /*! Prefix Operator Specifier */, 
               postfix  /*! Postfix Operator Specifier */
             };

//  virtual unsigned int cfgIndexForEnd() const;
//        virtual std::vector<VirtualCFG::CFGEdge> cfgOutEdges(unsigned int index);
//        virtual std::vector<VirtualCFG::CFGEdge> cfgInEdges(unsigned int index);


// *** COMMON CODE SECTION BEGINS HERE ***

    public:

       // DQ (3/25/3006): I put this back in because it had the logic for where the copy function required 
       // and not required which is required to match the other aspects of the copy mechanism code generation.
       // Specifically it is a problem to declare the copy function everywhere because it is not implemented 
       // for the SgSymbol IR nodes. I'm not clear why it is not implemented for these IR nodes.
      /*! \brief Copies AST (whole subtree, depending on the SgCopyHelp class */
       // virtual SgNode* copy ( const SgCopyHelp & help) const;

      /*! \brief returns a string representing the class name */
          virtual std::string class_name() const;

      /*! \brief returns new style SageIII enum values */
//        virtual VariantT variantT() const; // MS: new variant used in tree traversal

       /* the generated cast function */
      /*! \brief Casts pointer from base class to derived class */
          friend       SgUnaryOp* isSgUnaryOp(       SgNode * s );
      /*! \brief Casts pointer from base class to derived class (for const pointers) */
          friend const SgUnaryOp* isSgUnaryOp( const SgNode * s );

     public:
       // DQ (9/21/2005): Support for memory pools
#if defined(INLINE_FUNCTIONS)
      /*! \brief returns pointer to newly allocated IR node */
          inline void *operator new (size_t size);
#else
      /*! \brief returns pointer to newly allocated IR node */
          void *operator new (size_t size);
#endif
      /*! \brief deallocated memory for IR node (returns memory to memory pool for reuse) */
          void operator delete (void* pointer, size_t size);

       // DQ (1/2/2006): Compute the amount of memory allocated for this type within the AST.
      /*! \brief Returns the total number of IR nodes of this type */
          static int numberOfNodes ();
      /*! \brief Returns the size in bytes of the total memory allocated for all IR nodes of this type */
          static int memoryUsage ();

      // End of scope which started in IR nodes specific code 
      /*}*/

      /* \name Internal Functions
          \brief Internal functions ... incomplete-documentation

          These functions have been made public as part of the design, but they are suggested for internal use 
          or by particularly knowledgable users for specialized tools or applications.

          \internal We could not make these private because they are required by user for special purposes. And 
              it would be unwieldy to have many of the internal classes in ROSE be explicitly defined as friends.
          {
       */
       // MS: 02/12/02 container of pointers to AST successor nodes used in the traversal
       // overriden in every class by *generated* implementation
//        virtual std::vector<SgNode*> get_traversalSuccessorContainer();
       // MS: 06/28/02 container of names of variables or container indices 
       // used used in the traversal to access AST successor nodes
       // overriden in every class by *generated* implementation
//        virtual std::vector<std::string> get_traversalSuccessorNamesContainer();

       // MS: 08/16/2002 method for generating RTI information
//        virtual RTIReturnType roseRTI();	
      /*}*/



      /* \name Deprecated Functions
          \brief Deprecated functions ... incomplete-documentation

          These functions have been deprecated from use.
       */
      /*{*/
      /*! returns a C style string (char*) representing the class name */
          virtual const char* sage_class_name() const ROSE_DEPRECATED_FUNCTION;

      /*! returns old style Sage II enum values */
          virtual int variant() const ROSE_DEPRECATED_FUNCTION;
      /*! returns old style Sage II enum values */
          virtual int getVariant() const ROSE_DEPRECATED_FUNCTION;
      /*}*/




     public:
      /* \name Traversal Support Functions
          \brief Traversal support functions ... incomplete-documentation

          These functions have been made public as part of the design, but they are suggested for internal use 
          or by particularly knowledgable users for specialized tools or applications.
       */
      /*{*/
       // DQ (11/26/2005): Support for visitor pattern mechanims
       // (inferior to ROSE traversal mechanism, experimental).
      /*! \brief \b FOR \b INTERNAL \b USE Support for visitor pattern.
       */
//        void executeVisitorMemberFunction (ROSE_VisitorPattern & visitor);

       // DQ (12/26/2005): Support for traversal based on the memory pool
      /*! \brief \b FOR \b INTERNAL \b USE Support for visitor pattern.
       */
//        static void traverseMemoryPoolNodes(ROSE_VisitTraversal & visit);

      /*! \brief \b FOR \b INTERNAL \b USE Support for visitor pattern.
       */
//        static void traverseMemoryPoolVisitorPattern(ROSE_VisitorPattern & visitor);

       // DQ (2/9/2006): Added to support traversal over single representative of each IR node
       // This traversal helps support intrnal tools that call static member functions.
       // note: this function operates on the memory pools.
      /*! \brief \b FOR \b INTERNAL \b USE Support for type-based traversal.
       */
//        static void visitRepresentativeNode (ROSE_VisitTraversal & visit);
      /*}*/




     public:
      /* \name Memory Allocation Functions
          \brief Memory allocations functions ... incomplete-documentation

          These functions have been made public as part of the design, but they are suggested for internal use 
          or by particularly knowledgable users for specialized tools or applications.
       */
      /*{*/
      /*! \brief \b FOR \b INTERNAL \b USE This is used in internal tests to verify that all IR nodes are allocated from the heap.

          The AST File I/O depends upon the allocation of IR nodes being from the heap, stack based or global IR nodes should
          not appear in the AST if it will be written out to a file and read back in.  To enforce this concept, this function
          implements a test to verify that the IR node can be found on the heap and is part of a larger test of the whole AST.
          This test must pass before the AST can be written out to a file.  This is part of a compromise in the design of the 
          AST File I/O to support binary streaming of data to files; for performance.  It is also rather difficult, but possible, 
          to build a useful AST with IR nodes allocated on the stack or frm global scope, this test filters out such cased from
          being used with the AST File I/O mechanism.
       */
          virtual bool isInMemoryPool();

      /*! \brief \b FOR \b INTERNAL \b USE This is used in internal tests to verify that all IR nodes are allocated from the heap.

          The AST File I/O depends upon the allocation of IR nodes being from the heap, stack based or global IR nodes should
          not appear in the AST if it will be written out to a file and read back in.  To enforce this concept, this function
          implements a test to verify that the IR node can be found on the heap and is part of a larger test of the whole AST.
          This test must pass before the AST can be written out to a file.  This is part of a compromise in the design of the 
          AST File I/O to support binary streaming of data to files; for performance.  It is also rather difficult, but possible, 
          to build a useful AST with IR nodes allocated on the stack or frm global scope, this test filters out such cased from
          being used with the AST File I/O mechanism.
       */
          virtual void checkDataMemberPointersIfInMemoryPool();

      // DQ (4/30/2006): Modified to be a const function.
      /*! \brief \b FOR \b INTERNAL \b USE Returns STL vector of pairs of SgNode* and strings for use in AST tools

          This functions is part of general support for many possible tools to operate 
          on the AST.  The forms a list of ALL IR node pointers used by each IR node,
          and is a supperset of the get_traversalSuccessorContainer().  It is (I think)
          less than the set of pointers used by the AST file I/O. This is part of
          work implemented by Andreas, and support tools such as the AST graph generation.

          \warning This function can return unexpected data members and thus the 
                   order and the number of elements is unpredicable and subject 
                   to change.

          \returns STL vector of pairs of SgNode* and strings
       */
//        virtual std::vector<std::pair<SgNode*,std::string> > returnDataMemberPointers() const;

      /*! \brief \b FOR \b INTERNAL \b USE Returns STL vector of pairs of references to SgNode* and strings for use in AST tools

          This functions similar to returnDataMemberPointers() except that it containes pointers to SgNode* instead 
          of SgNode pointers as values.  As a result there is FAR more damage that can be done by using this function.
          This is provided for support of internal tools that operate on the AST, e.g the AST Merge mechanism.

          \warning This function can return unexpected data members and thus the 
                   order and the number of elements is unpredicable and subject 
                   to change.

          \returns STL vector of pairs of SgNode** and strings
       */
//        virtual std::vector<std::pair<SgNode**,std::string> > returnDataMemberReferenceToPointers();

      /*! \brief Constructor for use by AST File I/O Mechanism

          This constructor permits the IR node to be rebuilt from the contiguously arranged data in memory 
          which obtained via fast binary file I/O from disk.
       */
//        SgUnaryOp( SgUnaryOpStorageClass& source );





 // JH (10/24/2005): methods added to support the ast file IO
    private:

      /* \name AST Memory Allocation Support Functions
          \brief Memory allocations support....

          These functions handle the low leve support of the memory allocation scheme which permits IR nodes to be allocated 
          in large contiguous blocks to reduce memory fragmentation, improve performance, support specialized access (AST traversals),
          and support the AST File I/O Mechanism.
       */
      /*{*/

       /* JH (10/24/2005): Two typefefs. The TestType notes the type every pointer is cast to before
          we compare them. Since I had several problems with this, I decided to make a typdef to ensure
          that I use the same type everywhere, if any changes are made. THe second one declares the type
          (actually unsigned long) where teh pointer address gets converted to. On 64 bit platforms this
          might got changed, but unfortunatly, the return types are still unsigned longs. There exists
          a correspinding one in the AST_FILE_IO class!
       */
      /*! \brief Typedef used for low level memory access.
       */
          typedef unsigned char* TestType;
      /*! \brief Typedef used to hold memory addresses as values.
       */
          typedef unsigned long  AddressType;


       // DQ (4/6/2006): Newer code from Jochen
       // Methods to find the pointer to a global and local index
          static SgUnaryOp* getPointerFromGlobalIndex ( unsigned long globalIndex ) ;
//        static SgUnaryOp* getPointerFromGlobalIndex ( AstSpecificDataManagingClass* astInPool, unsigned long globalIndex ) ;

       // Methods for computing the total size of the memory pool. It actually returns the
       // size of the whole blocks allocated, no matter they contain valid pointers or not
          static unsigned long getNumberOfValidNodesAndSetGlobalIndexInFreepointer( unsigned long );
          static void clearMemoryPool ( );
          static void extendMemoryPoolForFileIO ( );
//        static unsigned long initializeStorageClassArray( SgUnaryOpStorageClass *storageArray );
          static void resetValidFreepointers( );
          static unsigned long getNumberOfLastValidPointer();

       // necessary, to have direct access to the p_freepointer and the private methods !
          friend class AST_FILE_IO;
          friend class SgUnaryOpStorageClass;
          friend class AstSpecificDataManagingClass;
          friend class AstSpecificDataManagingClassStorageClass;

    public:
          SgUnaryOp( const SgUnaryOpStorageClass& source );

     private:
       /*
          \name AST Memory Allocation Support Variables
          Memory allocations support variables 

          These variables handle the low leve support of the memory allocation scheme which permits IR nodes to be allocated 
          in large contiguous blocks to reduce memory fragmentation, improve performance, support specialized access (AST traversals),
          and support the AST File I/O Mechanism.
       */
      /*{*/

       // DQ (9/21/2005): Static variables supporting memory pools
      /*! \brief \b FOR \b INTERNAL \b USE Number of objects allocated within each block of objects forming a memory pool for this IR node.

          \internal This is part of the support for memory pools within ROSE.
               This can be removed and should be replaced by a function that evaluates to an increasing 
               value so that each block in a memory pool is larger than the last (e.g. factor of 2).
       */
          static int CLASS_ALLOCATION_POOL_SIZE;        // = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;

      /*! \brief \b FOR \b INTERNAL \b USE Current object (within a block) which will be returned from the new operator

          \internal This is part of the support for memory pools withn ROSE.
       */
          static SgUnaryOp* Current_Link;              // = NULL;

       // DQ (12/15/2005): This is Jochen's implementation of the memory allocation pools.
       // This is was one of the things on the todo list (above).

      /*! \brief \b FOR \b INTERNAL \b USE This is the STL vector containing the pointers to the blocks in an IR node's memory pool.

          \internal This is part of the support for memory pools within ROSE.
       */
//        static std::vector < unsigned char* > Memory_Block_List;
      /*}*/

    public:

// *** COMMON CODE SECTION ENDS HERE ***

     // the generated cast function
     // friend SgUnaryOp* isSgUnaryOp ( SgNode* s );


          void post_construction_initialization();
     public: 
         SgExpression* get_operand_i() const;
         void set_operand_i(SgExpression* operand_i);
     public: 
         SgUnaryOp::Sgop_mode get_mode() const;
         void set_mode(SgUnaryOp::Sgop_mode mode);


     public: 
         virtual ~SgUnaryOp();

     public: 
         SgUnaryOp(Sg_File_Info* startOfConstruct = NULL, SgExpression* operand_i = NULL, SgType* expression_type = NULL); 

     protected: 
//        SgExpression* p_operand_i;
          SgType* p_expression_type;
          SgUnaryOp::Sgop_mode p_mode;
          


   };

// postdeclarations for SgUnaryOp
