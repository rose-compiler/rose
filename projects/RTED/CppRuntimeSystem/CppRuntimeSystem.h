#ifndef CPP_RUNTIMESYSTEM_H
#define CPP_RUNTIMESYSTEM_H

#include <iostream>
#include <string>
#include <map>
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
 * @brief Main API of the runtimesystem.  Provides direct and indirect access to
 * PointerManager, MemoryManager, StackManager, CStdLibManager, and FileManager.
 * Also provides access to TypeSystem.
 *
 * RuntimeSystem is responsible for keeping track of all variable allocations,
 * memory and file operations.  Although most of the implementation exists in
 * the various managers, the primary user-visible API is provided by this class.
 *
 * There a two classes of functions:
 *      - Register Functions:   have to be called when specific memory operations
 *                              are called in instrumented code especially
 *                              (de-)allocations, and writes to memory
 *      - Check Functions:      check if certain memory allocations are safe/valid
 *
 * Features of RuntimeSystem include:
 *      - @b Checking @b memory @b reads.
 *              Reads can be checked to ensure memory is readable (i.e. an
 *              address refers to allocated memory), initialized, and
 *              type-consistent.
 *      - @b Checking @b memory @b writes.
 *              Writes can be checked to ensure memory is allocated, and that
 *              the write is type-consistent (e.g. ensuring that one doesn't
 *              write a float to a memory location to which one had previously
 *              written an int).
 *      - @b Checking @b pointer @b manipulations.
 *              Pointer increments can be checked to ensure that they stay
 *              within the same sort of memory.  The canonical example is array
 *              bounds checking.
 *              @n@n
 *              Memory leaks can be checked at pointer assignment as well.  If
 *              some pointer φ refers to some allocated memory μ and is assigned
 *              to some other address, and no other pointer points to μ, an
 *              error can be raised.  This has false positives in the sense that
 *              one might be able to reliably compute the address of a memory
 *              location without having a pointer to that location.
 *              @n
 *      - @b Stack @b Management.
 *              The runtime system can track stack variables and automatically
 *              deallocate them (and check for corresponding memory leaks) when
 *              stack frames are removed.
 *      - @b Miscellaneous
 *              The runtime system can also verify function signatures (only
 *              applicable to C) and check for consistent memory
 *              allocation/deallocation (e.g. raising an exception if memory
 *              allocated with @c malloc is freed with @c delete).
 *
 * @remarks RuntimeSystem provides an API for managing pointers, checking for
 * memory leaks and so on.  There are some important implementation details,
 * namely:
 *      -#  An assumption is made that all pointers are of the same size
 *          (specifically, @c sizeof(void*)).
 *      -#  That size is determined when the @em RuntimeSystem is compiled.
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
        void setCodePosition(SourcePosition &  sp) {curPos=sp; }

        /// if testing mode is true exceptions are thrown when a violations occurs
        /// otherwise abort is called, default false
        void setTestingMode(bool b) { testingMode = b;}

        /// Switches the Qt-Debugger on/off (works only if compiled with ROSE_WITH_ROSEQT)
        void setQtDebuggerEnabled(bool b) { qtDebugger = b; }

        bool isQtDebuggerEnabled() { return qtDebugger; }

        /// Changes the violation policy for the given violation type.
        void setViolationPolicy( RuntimeViolation::Type, ViolationPolicy::Type );


        // access a specific manager

        CStdLibManager * getCStdLibManager() { return &cstdlibManager; }

        MemoryManager * getMemManager()     { return & memManager;     }
        FileManager   * getFileManager()    { return & fileManager;    }
        TypeSystem    * getTypeSystem()     { return & typeSystem;     }
        StackManager  * getStackManager()   { return & stackManager;   }
        PointerManager* getPointerManager() { return & pointerManager; }

        // ---------------------------------  Register Functions ------------------------------------------------------------

        /** Notifies the runtime-system that a variable was created.
         * Creates this variable on the stack and handles allocation right.
         * The VariablesType is deleted when the var goes out of scope.
         *
         * @c createVariable should not be used for stack arrays.  Instead, use
         * @ref createArray.  @c createVariable @e can be used for pointer
         * types, and will automatically be registered with the pointer manager
         * when a pointer change is registered.  See @ref registerPointerChange.
         *
         * The following example shows how to properly register a local
         * variable:
         @code
            int foo() {
                int x;

                RuntimeSystem* rs = RuntimeSystem::instance();
                rs -> createVariable(
                    &x,
                    "x",
                    "mangled_x",
                    "SgTypeInt"
                );
            }
         @endcode
         *
         * Using a std::string for the type is a convenience.  Calling
         * createVariable( Address, const std::string&, const std::string&, RsType*)
         * is preferred.
         *
         * @param address       The (stack) address of the variable.
         * @param name          The simple name of the variable.  Only used for the
         *                      debugger and/or output, not to detect violations.
         * @param typeString    A valid RsType string.
         */
        void createVariable(Address address,
                            const std::string & name,
                            const std::string & mangledName,
                            const std::string & typeString);

        void createVariable(Address address,
                            const std::string & name,
                            const std::string & mangledName,
                            RsType *  type);


        void createArray(   Address address,
                            AddressDesc desc,
                            const std::string & name,
                            const std::string & mangledName,
                            const std::string & baseType,
                            size_t size);

        void createArray(   Address address,
                            AddressDesc desc,
                            const std::string & name,
                            const std::string & mangledName,
                            RsType * baseType,
                            size_t size);

        void createArray(   Address address,
                            AddressDesc desc,
                            const std::string & name,
                            const std::string & mangledName,
                            RsArrayType * type);

        /** Notifies the RTS that a C++ object has been created.  This will
         * typically be called in the constructor, so that the RTS is aware of
         * the memory during the constructor.  If the object is on the stack,
         * the variable will have to be created (and associated with this
         * memory) later, at the right scope.
         *
         * The behaviour of createObject is intended to handle subtypes and
         * nested types.  If createObject is called for an existing address,
         * usually nothing will happen.  However, if the address is exactly the
         * same (and not an offset in an existing @c MemoryType) and the type is
         * a subtype, then the type of the memory layout is updated.
         */
        void createObject(  Address address,
                            RsClassType* type );



        /** Notify the runtime system that memory has been allocated, usually
         * via @c malloc or @c new.
         *
         * @param addr          The base address of the newly allocated memory.
         * @param size          The size of the newly allocated memory.
         * @param onStack       Whether the memory is on the stack or not.  @ref
         *                      createMemory should be called for stack
         *                      variables, in which case @c onStack should be @c
         *                      true.  The runtime system will detect invalid
         *                      frees to stack memory.
         * @param fromMalloc    Whether the memory was created via a C-style
         *                      allocation such as @c malloc, as opposed to a
         *                      C++ style allocation such as @c new.  Ignored if
         *                      onstack is true.
         * @param type          What type information is known about the memory,
         *                      if any.  If and when memory becomes typed, calls
         *                      to @ref checkMemRead and @ref checkMemWrite can
         *                      verify that memory is used in a type-consistent
         *                      way.
         */
        void createMemory(Address addr, size_t size, MemoryType::AllocKind kind, RsType * type=NULL);

        /// this version creates stackmemory, of given type
        void createStackMemory(Address addr, size_t size,const std::string & type);


        /** Symmetric to @ref createMemory.
         */
        void freeMemory(Address startAddress, MemoryType::AllocKind kind);


        /** Registers that a pointer, at address @c sourceAddress has just been
         * updated to point to @c targetAddress.  If this is the first time
         * registerPointerChange() has been called for @c sourceAddress it will
         * be automatically registered with the PointerManager.
         *
         * @param checkPointerMove
         *      If @b true, treat this pointer change as a "move" and report as
         *      violations escaping array bounds, even if @c targetAddress is
         *      valid memory.  The canonical example of such a violation is the
         *      following:
         @code
            struct Type {
                int x[ 2 ];
                int y;
            } var;
           int* p = &var.x;
           ++p;     // okay
           ++p;     // now we point to &v.y, an illegal "pointer move"
         @endcode
         *      @c checkPointerMove is expected to be @b true for pointer
         *      arithmetic, and @b false for other assignment.
         *
         * @param checkMemLeaks
         *      If @b true, and the pointer at @c sourceAddress previously
         *      pointed to some memory @c x, ensure that there is still a
         *      known pointer that refers to @c x.  This check @em can result in
         *      false positives, as there is no runtime dataflow analysis and so
         *      will report as a leak cases when an address is reliably
         *      computable (e.g. if one stores an int with a fixed offset from
         *      the address).
         */
        void registerPointerChange( Address     src,
                                    AddressDesc src_desc,
                                    Address     target,
                                    AddressDesc tgt_desc,
                                    bool        checkPointerMove,
                                    bool        checkMemLeaks
                                  );

        /// for documentation see PointerManager::registerPointerChange()
        void registerPointerChange( Address     src,
                                    AddressDesc src_desc,
                                    Address     tgt,
                                    AddressDesc tgt_desc,
                                    RsType*     type,
                                    bool        checkPointerMove, //=false,
                                    bool        checkMemLeaks //=true
                                  );

#if OBSOLETE_CODE
        /// Convenience function which takes mangledName instead of sourceAddress
        /// void registerPointerChange( const std::string & mangledName, Address targetAddress, bool checkPointerMove=false, bool checkMemLeaks=true);
#endif /* OBSOLETE_CODE */

        /// Checks if two addresses lie in the same "typed chunk"
        /// equivalent to the check which is done on registerPointerChange
        void checkPointerDereference( Address src, AddressDesc src_desc, Address derefed_address, AddressDesc derefed_desc );
        void checkIfThisisNULL(void* thisExp);


        /// Each variable is associated with a scope, use this function to create a new scope
        /// @param name  string description of scope, may be function name or "for-loop" ...
        void beginScope(const std::string & name);

        /// Closes a scope and deletes all variables which where created via registerVariable()
        /// from the stack, testing for memory leaks (@ref registerPointerChange).
        void endScope ();



        //// Call this function if a file is opened
        /// @param openMode combination of FileOpenMode flags
        void registerFileOpen (FILE * file, const std::string & openedFile, int openMode);
        void registerFileOpen (FILE * file, const std::string & openedFile, const std::string & openMode);
        void registerFileClose(FILE * file);
        void registerFileOpen (std::fstream& file, const std::string & openedFile, int openMode);
        void registerFileOpen (std::fstream& file, const std::string & openedFile, const std::string & openMode);
        void registerFileClose(std::fstream& file);


        /** This function be called if a function call is about to be made whose
         * signature could not be verified at compile time (e.g. a c program
         * without the function definition, which may have an incorrect or
         * missing function prototype).
         *
         * @param name     The name of the function that is to be called.
         * @param types    The type signature of the function, including return
         *                 type.  The first item should be the return type
         *                 (SgVoidType if void), and the remaining, the types
         *                 of the parameters.
         */
        void expectFunctionSignature( const std::string & name, const std::vector< RsType* > types );
        /** This function should be called at all function definitions, to
         * verify the signature of separately compiled callsites, if necessary.
         * Only applicable to C programs.
         */
        void confirmFunctionSignature( const std::string & name, const std::vector< RsType* > types );

        // --------------------------------  Check Functions ------------------------------------------------------------

        /// Checks if a specific memory region can be read (useful to check pointer derefs)
        /// true when region lies in allocated and initialized memory chunk
        void checkMemRead(Address addr, AddressDesc desc, size_t size, RsType* t = NULL);

        /// Checks if a specific memory region can be safely written
        /// true when region lies in allocated memory chunk
        void checkMemWrite(Address addr, AddressDesc desc, size_t size, RsType* t = NULL);



        /// Call this function on program end, to list resources which have not been freed
        void doProgramExitChecks();

        /// Returns true if file is currently open
        /// @param read true if read access, false for write access
        void checkFileAccess(FILE * f, bool read);
        void checkFileAccess(std::fstream& f, bool read);


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

        void check_memcpy ( const void* destination , const void* source , size_t num ) { cstdlibManager.check_memcpy( destination , source , num );};
        void check_memmove ( const void* destination , const void* source , size_t num ) { cstdlibManager.check_memmove( destination , source , num );};
        void check_strcpy ( const char* destination , const char* source ) { cstdlibManager.check_strcpy( destination , source );};
        void check_strncpy ( const char* destination , const char* source , size_t num ) { cstdlibManager.check_strncpy( destination , source , num );};
        void check_strcat ( const char* destination , const char* source ) { cstdlibManager.check_strcat( destination , source );};
        void check_strncat ( const char* destination , const char* source , size_t num ) { cstdlibManager.check_strncat( destination , source , num );};
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

        void printMessage(std::string message);

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

        /** A @c map of violation types to policies.  Policies include:
         *
         *      - @b Exit (default).    Report the violation and terminate the
         *                              running process.
         *      - @b Warn.              Report the violation and continue.
         *      - @b Ignore.            Do nothing.
         *      - @b InvalidatePointer. Only valid for
         *                              RuntimeViolation::INVALID_PTR_ASSIGN.
         *                              Do not report the violation, but
         *                              invalidate the pointer, both the real
         *                              one and our tracked pointer info, by
         *                              setting the target to @c NULL.  If an
         *                              attempt is made to read or write to the
         *                              pointer without registering a pointer
         *                              change, a different violation will
         *                              occur.
         *
         * @todo there are certain violations where the RuntimeSystem is in
         * inconsistent state after they have occured.
         */
        std::map<RuntimeViolation::Type, ViolationPolicy::Type> violationTypePolicy;

        ViolationPolicy::Type getPolicyFromString( std::string & name ) const;

        // members related to function signature verification
        std::string nextCallFunctionName;
        std::vector< RsType* > nextCallFunctionTypes;

    friend class PointerManager;
};


#endif

// vim:sw=4 ts=4 tw=80 et sta:
