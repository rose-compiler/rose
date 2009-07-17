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
#include "VariablesType.h"
#include "CStdLibManager.h"
#include "TypeSystem.h"

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

        static RuntimeSystem* instance();
        ~RuntimeSystem() {}


        /// Gets called when a violation is detected
        /// this function decides what to do (message printing, aborting program)
        /// this function does not necessarily stop the program, but may just print a warning
        void violationHandler(RuntimeViolation::Type v, const std::string & desc ="") throw (RuntimeViolation);
        void violationHandler(RuntimeViolation & vio)   throw (RuntimeViolation);


        /// call this function to inform the runtimesystem what the current position in sourcecode is
        /// this information is used for printing errors/warnings
        void checkpoint(const SourcePosition & pos)  { curPos = pos; }

        /// if testing mode is true exceptions are thrown when a violations occurs
        /// otherwise abort is called, default false
        void setTestingMode(bool b) { testingMode = b;}


        CStdLibManager * getCStdLibManager() { return &cstdlibManager; }

        MemoryManager * getMemManager()   { return & memManager;  }
        FileManager   * getFileManager()  { return & fileManager; }
        TypeSystem    * getTypeSystem()   { return & typeSystem;  }

        // ---------------------------------  Register Functions ------------------------------------------------------------

        /// Notifies the runtime-system that a variable was created
        /// creates this variable on the stack and handles allocation right
        /// the VariablesType is deleted when the var goes out of scope
        void createVariable(VariablesType * var);

        void createVariable(addr_type address,
                            const std::string & name,
                            const std::string & mangledName,
                            const std::string & typeString,
                            const std::string & className,
                            const std::string & pointerType="");


        /// Call this function after when a malloc or new occurs in monitored code
        /// @param startAdress  the return value of malloc/new
        /// @param size         size of the allocated memory area
        /// @param pos          position of allocation (needed to inform the user when this allocation is never freed)
        void createMemory(addr_type startAddress, size_t size, bool onStack = false);

        /// Call this function when a free/delete occurs in monitored code
        /// @param startAddress the address to be freed (argument of free/delete)
        void freeMemory(addr_type startAddress, bool onStack = false);


        /// Call this function when the value of a pointer changed i.e. the address a pointer points to
        /// this also includes "pseudo" pointers, for example if on code "int ** a" this function has to be called twice
        /// with var="a" and var="*a"
        /// pointer information is deleted when the variable goes out of scope
        /// @param checks if true, it is checked if the pointer changed the memory area it points to
        ///               which is fine, if pointer was changed by assignment, and which might be an "error"
        ///               if it was changed via pointer arithmetic
        void registerPointerChange(const std::string & var, addr_type targetAddress, bool checks=false);


        /// Each variable is associated with a scope, use this function to create a new scope
        /// @param name  string description of scope, may be function name or "for-loop" ...
        void beginScope(const std::string & name);

        /// Closes a scope and deletes all variables which where created via registerVariable()
        /// from the stack, tests for
        void endScope ();

        //// Call this function if a file is opened
        /// @param openMode combination of FileOpenMode flags
        void registerFileOpen (FILE * file, const std::string & openedFile, int openMode);
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
        /// @param read true if read access, false for write access
        void checkFileAccess(FILE * f, bool read);


        // ------------------- CStdLib delegates ------------------------------
        //
        //  These functions should all delegate to cstdlibManager's function of
        //  the same signature, which in turn should have the same signature as
        //  their corresponding cstdlib function, except with "check_" prefixed
        //  to the function name.
        //
        //  Each function checks that a call to its associated cstdlib function
        //  would be legal, relative certain classes of errors, including:
        //      
        //      1.  Ensuring that when necessary, strings have been properly
        //          initialized and a null terminator exists in the same memory
        //          region that the pointer refers to.
        //
        //      2.  Ensuring that destinations for writes are large enough, and
        //          do not overlap with sources, when doing so is inappropriate.

        void check_memcpy ( void* destination , const void* source , size_t num ) { cstdlibManager.check_memcpy( destination , source , num );};
        void check_memmove ( void* destination , const void* source , size_t num ) { cstdlibManager.check_memmove( destination , source , num );};
        void check_strcpy ( char* destination , const char* source ) { cstdlibManager.check_strcpy( destination , source );};
        void check_strncpy ( char* destination , const char* source , size_t num ) { cstdlibManager.check_strncpy( destination , source , num );};
        void check_strcat ( char* destination , const char* source ) { cstdlibManager.check_strcat( destination , source );};
        void check_strncat ( char* destination , const char* source , size_t num ) { cstdlibManager.check_strncat( destination , source , num );};
        void check_strchr ( const char* str1 , int character ) { cstdlibManager.check_strchr( str1 , character );};
        void check_strpbrk ( const char* str1 , const char* str2 ) { cstdlibManager.check_strpbrk( str1 , str2 );};
        void check_strspn ( const char* str1 , const char* str2 ) { cstdlibManager.check_strspn( str1 , str2 );};
        void check_strstr ( const char* str1 , const char* str2 ) { cstdlibManager.check_strstr( str1 , str2 );};
        void check_strlen ( const char* str ) { cstdlibManager.check_strlen( str );};

        // --------------------------------------------------------------------


        /// Deletes all collected data
        /// normally only needed for debug purposes
        void clearStatus();


        // --------------------------------  State Output Functions ---------------------------------

        /// All printing function go per default to this stream
        void registerOutputStream(std::ostream * os) { defaultOutStr = os; }

        /// Prints all the output on default to this file
        /// creates a file with specified filename, and writes all the output in that file
        void setOutputFile(const std::string & file);


        void log(const std::string & msg)  { (*defaultOutStr) << msg; }
        std::ostream & log()               { return (*defaultOutStr); }

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


        VariablesType * findVarByName(const std::string & name);

        /// Class to track state of memory (which areas are allocated etc)
        MemoryManager memManager;
        /// Class to track all opened files and file-accesses
        FileManager fileManager;
        /// Class to check arguments to certain cstdlib functions   
        CStdLibManager cstdlibManager;
        /// Class for managing all known types
        TypeSystem typeSystem;

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

        /// if true exceptions are thrown when a violations occurs
        /// otherwise abort is called, default false
        bool testingMode;


        //  -----------  Members which are used for output -------------


        SourcePosition curPos;

        std::ostream * defaultOutStr;
        std::ofstream outFile;
};


#endif

// vim:sw=4 ts=4 tw=80 et sta:
