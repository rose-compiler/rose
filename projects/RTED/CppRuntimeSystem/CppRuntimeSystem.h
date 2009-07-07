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

        CStdLibManager * getCStdLibManager() { return &cstdlibManager; }

        MemoryManager * getMemManager()   { return & memManager;  }
        FileManager   * getFileManager()  { return & fileManager; }

        // ---------------------------------  Register Functions ------------------------------------------------------------

        /// Notifies the runtime-system that a variable was created
        /// creates this variable on the stack and handles allocation right
        /// the VariablesType is deleted when the var goes out of scope
        void createVariable(VariablesType * var);

        void createVariable(addr_type address,
                            const std::string & name,
                            const std::string & mangledName,
                            const std::string & typeString,
                            size_t size);


        /// Call this function after when a malloc or new occurs in monitored code
        /// @param startAdress  the return value of malloc/new
        /// @param size         size of the allocated memory area
        /// @param pos          position of allocation (needed to inform the user when this allocation is never freed)
        void createMemory(addr_type startAddress, size_t size);

        /// Call this function when a free/delete occurs in monitored code
        /// @param startAddress the address to be freed (argument of free/delete)
        void freeMemory(addr_type startAddress);


        /// Call this function when the value of a pointer changed i.e. the address a pointer points to
        /// this also includes "pseudo" pointers, for example if on code "int ** a" this function has to be called twice
        /// with var="a" and var="*a"
        /// pointer information is deleted when the variable goes out of scope
        void createPointer(const std::string & var, addr_type targetAddress);

        /// Call this function if a pointer was manipulated via arithmetic operations
        /// when this function is called and the pointer changes the memory-chunk
        /// it points to, a violation/warning is printed
        void registerPointerChange(const std::string & var, addr_type newAddress);


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


#define COMMA ,
#define C_FUNCTION(x, params, args) void check_##x ( params ) { cstdlibManager.check_##x( args );}
#define C_FN_DSN(x) C_FUNCTION( x, \
                        void* destination COMMA const void* source COMMA size_t num, \
                        destination COMMA source COMMA num)
#define C_FN_CH_DSN(x) C_FUNCTION( x, \
                        char* destination COMMA const char* source COMMA size_t num, \
                        destination COMMA source COMMA num)
#define C_FN_CH_DS(x) C_FUNCTION( x, \
                        char* destination COMMA const char* source, \
                        destination COMMA source)
#define C_FN_SS( x) C_FUNCTION( x, \
                        const char* str1 COMMA const char* str2, \
                        str1 COMMA str2)
#define C_FN_SI( x) C_FUNCTION( x, \
                        const char* str1 COMMA int character, \
                        str1 COMMA character)
#define C_FN_S( x) C_FUNCTION( x, const char* str, str)
        C_FN_DSN( memcpy);
        C_FN_DSN( memmove);
        C_FN_CH_DS( strcpy);
        C_FN_CH_DSN( strncpy);
        C_FN_CH_DS( strcat);
        C_FN_CH_DSN( strncat);
        C_FN_SI( strchr);
        C_FN_SS( strpbrk);
        C_FN_SS( strspn);
        C_FN_SS( strstr);
        C_FN_S( strlen);

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

// vim:sw=4 ts=4 tw=80 et sta:
