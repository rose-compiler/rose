#ifndef CPP_RUNTIMESYSTEM_H
#define CPP_RUNTIMESYSTEM_H

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <cassert>
#include <fstream>


#include "Util.h"
#include "FileManager.h"
#include "MemoryManager.h"


/**
 * TODO
 *  - test FileManager
 *  - write small doxyfile
 *  - handle allocation/vars of stack-arrays (avoid double allocation)
 *  - create extra sourcefiles for MemoryManager
 *  - extend FileManager, checks if file exist, read/write access etc
 */


class VariablesType
{
    public:
        VariablesType(const std::string & _name,
                      const std::string & _mangledName,
                      const std::string & _typeStr,
                      addr_type _address);

        ~VariablesType();

        const std::string & getName()        const  { return name;}
        const std::string & getMangledName() const  { return mangledName; }
        addr_type           getAddress()     const  { return address; }

        /// returns the allocation information for this var
        MemoryType *        getAllocation()  const;

        void print(std::ostream & os) const;

    private:
        std::string name; ///< stack variable name
        std::string mangledName; ///< mangled name

        //FIXME do not store string here but type-enum
        std::string type; ///< string with class name of rose-type

        addr_type address; ///< address of this variable in memory
};


std::ostream& operator<< (std::ostream &os, const VariablesType & m);





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
                DOUBLE_ALLOCATION,   // try to reserve memory with lies in already allocated mem
                INVALID_FREE,        // called free on non allocated address
                MEMORY_LEAK,         // some allocated memory was not freed on program exit
                EMPTY_ALLOCATION,    // trying to get a memory area of size 0
                INVALID_READ,        // trying to read non-allocated or non-initialized mem region
                INVALID_WRITE,       // trying to write to non-allocated mem region
                DOUBLE_FILE_OPEN,    // trying to register the same file-poitner twice
                INVALID_FILE_ACCESS, // trying to access file which is not opened
                UNCLOSED_FILES       // some opened files where not closed before program exit
        };



        static RuntimeSystem* instance();
        ~RuntimeSystem() {}


        /// Gets called when a violation is detected
        /// this function decides what to do (message printing, aborting program)
        /// this function does not necessarily stop the program, but may just print a warning
        void violationHandler(Violation v, const std::string & description ="");

        /// call this function to inform the runtimesystem what the current position in sourcecode is
        /// this information is used for printing errors/warnings
        void checkpoint(const SourcePosition & pos)  { curPos = pos; }

        MemoryManager * getMemManager()  { return &memManager; }


        // ---------------------------------  Register Functions ------------------------------------------------------------

        /// Notifies the runtime-system that a variable was created
        /// creates this variable on the stack and handles allocation right
        /// the VariablesType is deleted when the var goes out of scope
        void createVariable(VariablesType * var);

        void createVariable(addr_type address,
                            const std::string & name,
                            const std::string & mangledName,
                            const std::string & typeString);


        /// Call this function after when a malloc or new occurs in monitored code
        /// @param startAdress  the return value of malloc/new
        /// @param size         size of the allocated memory area
        /// @param pos          position of allocation (needed to inform the user when this allocation is never freed)
        void createMemory(addr_type startAddress, size_t size);

        /// Call this function when a free/delete occurs in monitored code
        /// @param startAddress the address to be freed (argument of free/delete)
        void freeMemory(addr_type startAddress);

        /// Call this function when something is written to a memory region
        /// used to keep track which memory regions are initialized
        /// mb: not needed - use checkMemWrite()
        // void initMemory(addr_type addr, size_t length);


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
        void beginScope(const std::string & name);

        /// Closes a scope and deletes all variables which where created via registerVariable()
        /// from the stack, tests for
        void endScope ();


        /// Tells the runtime-system the actual position in source code
        /// used for user-notification where an error occured
        void registerCurrentSourcePosition(const SourcePosition & pos);

        /// Call this function if a file is opened
	    // tps: should this not be part of the variable?
        // mb : no, memory/variable allocation and files are different resources
        //      instrumented function may just call one function out of convenience, but here its splitted up
        void registerFileOpen (FILE * file, const std::string & openedFile);
        void registerFileClose(FILE * file);


        // --------------------------------  Check Functions ------------------------------------------------------------

        /// Checks if a specific memory region can be read (useful to check pointer derefs)
        /// true when region lies in allocated and initialized memory chunk
        void checkMemRead(addr_type addr, size_t length);

        /// Checks if a specific memory region can be safely written
        /// true when region lies in allocated memory chunk
        void checkMemWrite(addr_type addr, size_t length);


        /// Call this function on program end, to list resources which have not been freed
        void doProgramExitChecks();

        /// Returns true if file is currently open
        void checkFileAccess(FILE * f);


        /// TODO Checks for C-StdLib functions


        // --------------------------------  State Output Functions ---------------------------------

        /// All printing function go per default to this stream
        void registerOutputStream(std::ostream * os) { defaultOutStr = os; }

        /// Prints all the output on default to this file
        /// creates a file with specified filename, and writes all the output in that file
        void setOutputFile(const std::string & file);


        // Printing of RuntimeSystem status
        void printOpenFiles(std::ostream & os) const  { fileManager.print(os); }
        void printMemStatus(std::ostream & os) const  { memManager.print(os);  }
        void printStack    (std::ostream & os) const;

        // all functions again, print the status to registered outputstream
        void printOpenFiles() const  { printOpenFiles(*defaultOutStr); }
        void printMemStatus() const  { printMemStatus(*defaultOutStr); }
        void printStack    () const  { printStack    (*defaultOutStr); }

    private:

        static RuntimeSystem* single;
        RuntimeSystem();



        /// Class to track state of memory (which areas are allocated etc)
        MemoryManager memManager;
        /// Class to track all opened files and file-accesses
        FileManager fileManager;


        //  ------------ Tracking of stack and scope ------------------
        struct ScopeInfo
        {
            ScopeInfo( const std::string & _name, int index)
                : name(_name),stackIndex(index)
            {}

            std::string name;        /// description of scope, either function-name or something like "for-loop"
            int         stackIndex;  /// index in stack-array where this scope starts
        };
        std::vector<ScopeInfo> scope;

        std::vector<VariablesType *> stack;


        //  -----------  Members which are used for output -------------


        SourcePosition curPos;

        std::ostream * defaultOutStr;
        std::ofstream outFile;
};


#endif

