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
#include "StackManager.h"
#include "PointerManager.h"

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
        void checkpoint(const SourcePosition & pos) ;

        const SourcePosition & getCodePosition() const {return curPos; }

        /// if testing mode is true exceptions are thrown when a violations occurs
        /// otherwise abort is called, default false
        void setTestingMode(bool b) { testingMode = b;}

        /// Switches the Qt-Debugger on/off (works only if compiled with ROSE_WITH_ROSEQT)
        void setQtDebuggerEnabled(bool b) { qtDebugger = b; }

        /// Changes the violation policy for the given violation type.
        void setViolationPolicy( RuntimeViolation::Type, ViolationPolicy::Type );


        CStdLibManager * getCStdLibManager() { return &cstdlibManager; }

        MemoryManager * getMemManager()     { return & memManager;     }
        FileManager   * getFileManager()    { return & fileManager;    }
        TypeSystem    * getTypeSystem()     { return & typeSystem;     }
        StackManager  * getStackManager()   { return & stackManager;   }
        PointerManager* getPointerManager() { return & pointerManager; }

        // ---------------------------------  Register Functions ------------------------------------------------------------

        /// Notifies the runtime-system that a variable was created
        /// creates this variable on the stack and handles allocation right
        /// the VariablesType is deleted when the var goes out of scope

        void createVariable(addr_type address,
                            const std::string & name,
                            const std::string & mangledName,
                            const std::string & typeString);

        void createVariable(addr_type address,
                            const std::string & name,
                            const std::string & mangledName,
                            RsType *  type);


        void createArray(   addr_type address,
                            const std::string & name,
                            const std::string & mangledName,
                            const std::string & baseType,
                            size_t size);

        void createArray(   addr_type address,
                            const std::string & name,
                            const std::string & mangledName,
                            RsType * baseType,
                            size_t size);

        void createArray(   addr_type address,
                            const std::string & name,
                            const std::string & mangledName,
                            RsArrayType * type);



        /// Call this function after when a malloc or new occurs in monitored code
        /// @param startAdress  the return value of malloc/new
        /// @param size         size of the allocated memory area
        /// @param pos          position of allocation (needed to inform the user when this allocation is never freed)
        void createMemory(addr_type addr, size_t size,bool onStack = false, RsType * type=NULL);
        /// this version creates stackmemory, of given type
        void createStackMemory(addr_type addr, size_t size,const std::string & type);


        /// Call this function when a free/delete occurs in monitored code
        /// @param startAddress the address to be freed (argument of free/delete)
        void freeMemory(addr_type startAddress, bool onStack = false);


        /// Call this function when the value of a pointer changed i.e. the address a pointer points to
        /// this concept applies not only to pointer, but to all memory regions which can be dereferenced
        /// pointer information is deleted when the memory where srcAddress lies in is deleted
        /// @param checkPointerMove if true, it is checked if the pointer changed the memory area it points to
        ///               which is fine, if pointer was changed by assignment, and which might be an "error"
        ///               if it was changed via pointer arithmetic
        /// @param checkMemLeaks if true, and the pointer at sourceAddress
        ///               pointed at some memory, checks are made that pointers
        ///               still exist that reference the old memory
        void registerPointerChange( addr_type sourceAddress, addr_type targetAddress, bool checkPointerMove=false, bool checkMemLeaks=true);
        /// for documentation see PointerManager::registerPointerChange()
        void registerPointerChange( addr_type sourceAddress, addr_type targetAddress, RsType * type, bool checkPointerMove=false, bool checkMemLeaks=true);

        /// Convenience function which takes mangledName instead of sourceAddress
        void registerPointerChange( const std::string & mangledName, addr_type targetAddress, bool checkPointerMove=false, bool checkMemLeaks=true);

        /// Checks if two addresses lie in the same "typed chunk"
        /// equivalent to the check which is done on registerPointerChange
        void checkPointerDereference( addr_type sourceAddress, addr_type derefed_address );



        /// Each variable is associated with a scope, use this function to create a new scope
        /// @param name  string description of scope, may be function name or "for-loop" ...
        void beginScope(const std::string & name);

        /// Closes a scope and deletes all variables which where created via registerVariable()
        /// from the stack, tests for
        void endScope ();



        //// Call this function if a file is opened
        /// @param openMode combination of FileOpenMode flags
        void registerFileOpen (FILE * file, const std::string & openedFile, int openMode);
        void registerFileOpen (FILE * file, const std::string & openedFile, const std::string & openMode);
        void registerFileClose(FILE * file);



        // --------------------------------  Check Functions ------------------------------------------------------------

        /// Checks if a specific memory region can be read (useful to check pointer derefs)
        /// true when region lies in allocated and initialized memory chunk
        void checkMemRead(addr_type addr, size_t length, RsType * t = NULL);

        /// Checks if a specific memory region can be safely written
        /// true when region lies in allocated memory chunk
        void checkMemWrite(addr_type addr, size_t length, RsType * t = NULL);



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
        void printOpenFiles(std::ostream & os) const  { fileManager.print(os);    }
        void printMemStatus(std::ostream & os) const  { memManager.print(os);     }
        void printStack    (std::ostream & os) const  { stackManager.print(os);   }
        void printPointer  (std::ostream & os) const  { pointerManager.print(os); }

        // all functions again, print the status to registered outputstream
        void printOpenFiles() const  { printOpenFiles(*defaultOutStr); }
        void printMemStatus() const  { printMemStatus(*defaultOutStr); }
        void printStack    () const  { printStack    (*defaultOutStr); }
        void printPointer  () const  { printPointer  (*defaultOutStr); }

        // Access to variables/scopes
        int                 getScopeCount()     const;
        const std::string & getScopeName(int i) const;

        typedef std::vector<VariablesType*>::const_iterator VariableIter;
        VariableIter variablesBegin(int scopeId) const;
        VariableIter variablesEnd(int scopeId)   const;

    private:

        static RuntimeSystem* single;
        RuntimeSystem();


        void readConfigFile();

        /// Class to track state of memory (which areas are allocated etc)
        MemoryManager memManager;
        /// Class to track all opened files and file-accesses
        FileManager fileManager;
        /// Class to check arguments to certain cstdlib functions   
        CStdLibManager cstdlibManager;
        /// Class for managing all known types
        TypeSystem typeSystem;
        /// Class which keeps track of all variables and the stack-layout
        StackManager stackManager;
        /// Keeps track of Memory Regions which are dereferentiable
        PointerManager pointerManager;

        /// if true exceptions are thrown when a violations occurs
        /// otherwise abort is called, default false
        bool testingMode;

        /// Store if qt-debugger is enabled (called at every checkpoint)
        bool qtDebugger;


        //  -----------  Members which are used for output -------------

        SourcePosition curPos;

        std::ostream * defaultOutStr;
        std::ofstream outFile;

        /// A @c map of violation types to policies.  Policies include:
        ///
        ///     Exit (default)  -   Report the violation and terminate the
        ///                         process.
        ///     Warn            -   Report the violation and continue.
        ///     Ignore          -   Do nothing about the violation.
        ///
        ///     InvalidatePointer-  Only valid for INVALID_PTR_ASSIGN.  Do
        ///                         not report the violation, but invalidate the
        ///                         pointer, both the real one and our tracked 
        ///                         pointer info, by setting the target to NULL.
        ///                         If an attempt is made to read or write to the
        ///                         pointer without registering a pointer change,
        //                          a different violation will occur.
        ///     
        /// TODO there are certain violations where the RuntimeSystem is in
        ///      inconsistent state after they have occured
        ///      determine which Violations have this problem and prevent switching them off
        std::map<RuntimeViolation::Type, ViolationPolicy::Type> violationTypePolicy;

        ViolationPolicy::Type getPolicyFromString( std::string & name ) const;

    friend class PointerManager;
};


#endif

// vim:sw=4 ts=4 tw=80 et sta:
