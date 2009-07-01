#ifndef CPP_RUNTIMESYSTEM_H
#define CPP_RUNTIMESYSTEM_H

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <list>
#include <vector>

typedef unsigned long addr_type;

template <class T>
struct PointerCmpFunc
{
    bool operator() (const T* o1, const T* o2)  {
        return (*o1 < *o2);
    }
};






/**
 * This class represents a position in a sourcefile
 */
struct SourcePosition
{
    SourcePosition(const std::string & _file,int _line1, int _line2)
      : file(_file), line1(_line1), line2(_line2)
    {}
    std::string file;  ///< Absolute Path of source-file
    int line1;          ///< line number in sourcefile
    int line2;          ///< line number in transformed sourcefile
};
std::ostream& operator<< (std::ostream &os, const SourcePosition & m);


/**
 * This class represents a memory allocation, made by malloc/new or just on the stack
 */
class MemoryType
{
    public:
        MemoryType(addr_type addr, size_t size, const SourcePosition & pos);
        MemoryType(addr_type addr, size_t size,
                   const std::string & file, int line1, int line2);

        // constructor which initialized only the address, used for comparison purposes
        MemoryType(addr_type addr);

        ~MemoryType() {}
        /// Checks if an address lies in this memory chunk
        bool containsAddress(addr_type addr);
        /// Checks if a memory area is part of this allocation
        bool containsMemArea(addr_type addr, size_t size);
        /// Prints info about this allocation
        void print(std::ostream & os) const;

        /// Less operator uses startAdress
        bool operator< (const MemoryType & other) const;


        addr_type              getAdress() const { return startAddress; }
        size_t                 getSize()   const { return size; }
        const SourcePosition & getPos()    const { return allocPos; }

        bool  isInitialized(int offsetFrom, int offsetTo) const;

    private:
        addr_type         startAddress; ///< address where memory chunk starts
        size_t            size;         ///< Size of allocation
        SourcePosition    allocPos;     ///< Position in source file where malloc/new was called
        std::vector<bool> initialized;  ///< stores for every byte if it was initialized
};
std::ostream& operator<< (std::ostream &os, const MemoryType & m);


class MemoryManager
{
    public:
        MemoryManager() {};

        /// Destructor checks if there are still allocations which are not freed
        ~MemoryManager();

        /// Adds new allocation, the MemoryType structure is freed by this class
        void allocateMemory(MemoryType * alloc);

        /// Frees allocated memory, throws error when no allocation is managed at this addr
        void freeMemory(addr_type addr);


        /// Prints information about all currently allocated memory areas
        void print(std::ostream & os) const;

        /// Check if memory region is allocated and initialized
        /// @param size     size=sizeof(DereferencedType)
        bool checkRead  (addr_type addr, size_t size);

        /// Checks if memory at position can be safely written, i.e. is allocated
        bool checkWrite (addr_type addr, size_t size);


    private:

        MemoryType * findMem(addr_type addr, size_t size);

        void checkForNonFreedMem() const;

        typedef std::set<MemoryType*,PointerCmpFunc<MemoryType> > MemoryTypeSet;
        MemoryTypeSet mem;
};

std::ostream& operator<< (std::ostream &os, const MemoryManager & m);


class VariablesType 
{
    public:
      std::string name; // stack variable name
      std::string mangled_name; // mangled name
      std::string type;

      // the following line will be removed later on?
      struct ArraysType* arrays; // exactly one array

      // tps: what is address here?
      addr_type   address;
};

/**
 * RuntimeSystem is responsible for keeping track of all variable allocations and memory operations
 *
 * there a two types of functions:
 *      - Register Functions: have to be called when specific memory operations are called in instrumented code
 *                            especially (de-)allocations, and writes to memory
 *      - Check Functions:    check if certain memory allocations are safe/valid
 *
 * Singleton
 */
class RuntimeSystem
{
    public:
        enum Violation
        {
                DOUBLE_ALLOCATION, // try to reserve memory with lies in already allocated mem
                MEMORY_LEAK
        };



        static RuntimeSystem* instance();
        ~RuntimeSystem() {}


        /// Gets called when a violation is detected
        /// this function decides what to do (message printing, aborting program)
        void violationHandler(Violation v, const std::string & description ="");


        // ---------------------------------  Register Functions ------------------------------------------------------------
        /// Notifies the runtime-system that a variable was created
        /// creates this variable on the stack and handles allocation right
        /// @param name     (mangled?) name of variable
        /// @param address  memory address of this variable
        void createVariable(addr_type address, const std::string & name);

        /// Call this function after when a malloc or new occurs in monitored code
        /// @param startAdress  the return value of malloc/new
        /// @param size         size of the allocated memory area
        /// @param pos          position of allocation (needed to inform the user when this allocation is never freed)
        void createMemory(addr_type startAddress, size_t size, const SourcePosition & pos);

        /// Call this function when a free/delete occurs in monitored code
        /// @param startAddress the address to be freed (argument of free/delete)
        void deleteMemory(addr_type startAddress);

        /// Call this function when something is written to a memory region
        /// used to keep track which memory regions are initialized
        void initMemory(addr_type addr, size_t length);


        /// Call this function when the value of a pointer changed i.e. the address a pointer points to
        /// this also includes "pseudo" pointers, for example if on code "int ** a" this function has to be called twice
        /// with var="a" and var="*a"
        // tps: void createVariable ??
        // void createPointer(const std::string & var, addr_type targetAddress);

        /// Call this function to register constant initalized arrays like "int a[2][2] = { {1,2},{3,4} }"
        /// this informs the runtime-system that there is memory allocated from a to a+4
        /// @param var  name of the array
        /// @param dim  list of dimensions, the actual allocated size is the product of all entries * sizeof(type)
        // TODO may be needed for printing a warning at code like
        /// f(int b[2][20) {}
        ///
        /// int a[10][10];
        /// f(a);
        ///b
        /// normally you don't need that, because this case is caught via memory checking
        //void registerArray(const std::string & var, std::vector<int> dim);



        /// Each variable is associated with a scope, use this function to create a new scope
        /// @param name  string description of scope, may be function name or "for-loop" ...
        void beginScope(std::string & name);

        /// Closes a scope and deletes all variables which where created via registerVariable()
        /// from the stack, tests for
        void endScope ();


        /// Tells the runtime-system the actual position in source code
        /// used for user-notification where an error occured
        void registerCurrentSourcePosition(const SourcePosition & pos);


        /// Call this function if a file is opened
	// tps: should this not be part of the variable?
        void registerFileOpen (FILE * file);
        void registerFileClose(FILE * file);


        // --------------------------------  Check Functions ------------------------------------------------------------

        /// Checks if a specific memory region can be read (useful to check pointer derefs)
        /// true when region lies in allocated and initialized memory chunk
        bool checkMemRead(addr_type addr, size_t length);

        /// Checks if a specific memory region can be safely written
        /// true when region lies in allocated memory chunk
        /// TODO specify if registerMemoryWrite() is called automatically
        bool checkMemWrite(addr_type addr, size_t length);

        /// Returns true if file is currently open
        bool checkFileAccess(FILE * f);


    private:

        static RuntimeSystem* single;

        RuntimeSystem() {};

        /// Class to track state of memory (which areas are allocated etc)
        MemoryManager memManager;

        struct ScopeInfo
        {
            std::string name;        /// description of scope, either function-name or something like "for-loop"
            int         stackIndex;  /// index in stack-array where this scope starts
        };
        std::vector<ScopeInfo> scope;


        std::vector<VariablesType> stack;

        /// Tracking for opened files
        std::set<FILE*> openFiles;
};





/*
// Runtime System
struct RuntimeSystem* rtsi();
// Constructor - Destructor
void RuntimeSystem_Const_RuntimeSystem();
void RuntimeSystem_roseRtedClose();

// helper functions
char* RuntimeSystem_findLastUnderscore(char* s);
const char* RuntimeSystem_resBool(int val);
const char* RuntimeSystem_roseConvertIntToString(int t);
int RuntimeSystem_isInterestingFunctionCall(const char* name);
int RuntimeSystem_getParamtersForFuncCall(const char* name);
int getSizeOfSgType(const char* type);

// memory handling
void RuntimeSystem_increaseSizeMemory();
struct MemoryVariableType* RuntimeSystem_findMemory(long int address);
struct MemoryType* RuntimeSystem_AllocateMemory(long int address, int sizeArray, struct RuntimeVariablesType* var);
void RuntimeSystem_increaseSizeMemoryVariables(  int pos);
void RuntimeSystem_RemoveVariableFromMemory(long int address, struct RuntimeVariablesType* runtimevar);
int checkMemoryLeakIssues(int pos, int address, const char* filename, const char* line, const char* stmtStr, enum Error msg);


// array functions
int RuntimeSystem_findArrayName(const char* mangled_name);
//void RuntimeSystem_increaseSizeArray();                                               
void RuntimeSystem_roseCreateArray(const char* name, const char* mangl_name, int dimension,// int stack, 
				   long int sizeA, long int sizeB, int ismalloc, const char* filename, 
				   const char* line, const char* lineTransformed);

void RuntimeSystem_roseArrayAccess(const char* name, int posA, int posB, const char* filename, 
				   const char* line, const char* lineTransformed, const char* stmtStr);

// function calls 
const char* RuntimeSystem_findVariablesOnStack(const char* name);
void RuntimeSystem_increaseSizeRuntimeVariablesOnStack();                                               
void RuntimeSystem_roseCallStack(const char* name, const char* mangl_name, const char* beforeStr,const char* filename, const char* line);

void RuntimeSystem_handleSpecialFunctionCalls(const char* funcname,const char** args, int argsSize, const char* filename, const char* line, 
					      const char* lineTransformed, const char* stmtStr, const char* leftHandSideVar);
void RuntimeSystem_handleIOFunctionCall(const char* funcname,const char** args, int argsSize, const char* filename, const char* line, 
					const char* lineTransformed, const char* stmtStr, const char* leftHandSideVar);
void RuntimeSystem_roseFunctionCall(int count, ...);
int  RuntimeSystem_isSizeOfVariableKnown(const char* name);
int  RuntimeSystem_isModifyingOp(const char* name);
int RuntimeSystem_isFileIOFunctionCall(const char* name);

// handle scopes (so we can detect when locals go out of scope, free up the
// memory and possibly complain if the local was the last var pointing to some
// memory)
void RuntimeSystem_roseEnterScope();
void RuntimeSystem_roseExitScope( const char* filename, const char* line, const char* stmtStr);
void RuntimeSystem_expandScopeStackIfNecessary();


// function used to indicate error
void RuntimeSystem_callExit(const char* filename, const char* line, const char* reason, const char* stmtStr);

// functions dealing with variables
void RuntimeSystem_roseCreateVariable(const char* name, const char* mangled_name, const char* type, int init,
				      const char* fOpen, const char* filename, const char* line, const char* lineTransformed);
void RuntimeSystem_increaseSizeRuntimeVariables();
struct RuntimeVariablesType* RuntimeSystem_findVariables(const char* name);
int RuntimeSystem_findVariablesPos(const char* mangled_name, int* isarray);
void RuntimeSystem_roseInitVariable(const char* name,
				    const char* mangled_name,
				    const char* typeOfVar2,
				    const char* baseType2,
				    unsigned long long address,
				    unsigned long long value,
				    int ismalloc,
				    const char* filename, 
				    const char* line, const char* lineTransformed, 
				    const char* stmtStr);
void RuntimeSystem_roseAccessVariable( const char* name,
				       const char* mangled_name,
				       const char* filename, const char* line, 
				       const char* lineTransformed,
				       const char* stmtStr);

// USE GUI for debugging
void Rted_debugDialog(const char* filename, int line, int lineTransformed);
*/


#endif

