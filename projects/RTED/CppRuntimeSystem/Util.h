
#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <sstream>

// typedef unsigned long addr_type;


/**
 * This class represents a position in a sourcefile
 */
class SourcePosition
{
    public:
        SourcePosition();
        SourcePosition(const std::string & file);
        SourcePosition(const std::string & file,int line1, int line2);

        std::string toString() const;

        std::string getTransformedFile() const;

        const std::string & getFile() const { return file; }

        int getLineInOrigFile() const        { return line1; }
        int getLineInTransformedFile() const { return line2; }

    protected:
        std::string file;   ///< Absolute Path of source-file
        int line1;          ///< line number in sourcefile
        int line2;          ///< line number in transformed sourcefile
};
std::ostream& operator<< (std::ostream &os, const SourcePosition & m);


struct PointerCompare
{
  template <class T>
  bool operator()(const T* lhs, const T* rhs) const
  {
    return (*lhs) < (*rhs);
  }
};

class RuntimeViolation
{
    public:

        enum Type
        {
                DOUBLE_ALLOCATION,       // try to reserve memory with lies in already allocated mem
                INVALID_FREE,            // called free on non allocated address
                MEMORY_LEAK,             // some allocated memory was not freed on program exit
                EMPTY_ALLOCATION,        // trying to get a memory area of size 0
                INVALID_READ,            // trying to read non-allocated or non-initialized mem region
                INVALID_WRITE,           // trying to write to non-allocated mem region
                INVALID_FILE_OPEN,       // fopen could not open file, fileHandle==NULL
                DOUBLE_FILE_OPEN,        // trying to register the same file-pointer twice
                INVALID_FILE_CLOSE,      // trying to close a file which has already been closed / not opened
                INVALID_FILE_ACCESS,     // trying to access file which is not opened
                UNCLOSED_FILES,          // some opened files where not closed before program exit
                INVALID_PTR_ASSIGN,      // a invalid address (not allocated) is assigned to a pointer
                MEM_WITHOUT_POINTER,     // the last pointer which has pointed to a mem-location
                                         // has been deregistered
                POINTER_CHANGED_MEMAREA, // a pointer changed the memory area which it points to
                                         // (may be an error if changed by pointer arithmetic)
                INVALID_MEM_OVERLAP,     // some memory chunk overlaps with some
                                         // other memory chunk illegaly, e.g. in arguments to memcpy
                INVALID_TYPE_ACCESS,     // invalid access to "typed" memory
                UNEXPECTED_FUNCTION_SIGNATURE,  // a c program compiled with a missing or wrong
					// prototype gave the wrong types at the callsite
                NONE,                   // no violation
		UNKNOWN_VIOLATION
        };


        explicit RuntimeViolation(Type type, const std::string& desc = "");
        explicit RuntimeViolation(Type type, const std::stringstream & descStream);
        RuntimeViolation(const RuntimeViolation & other);

        virtual ~RuntimeViolation() throw() {}


        const std::string & getShortDesc() const { return shortDesc; }
        std::string getLongDesc()  const { return longDesc.str(); }

        std::stringstream & descStream()             { return longDesc; }
        const std::stringstream & descStream() const { return longDesc; }

        void setPosition(const SourcePosition & sourcePos)  { pos=sourcePos; }
        const SourcePosition & getPos() const               { return pos; }


        Type getType() const { return type; }


        static Type getViolationByString(const std::string & s);

    protected:

        static std::string getShortDescFromType(Type type);

        SourcePosition pos;
        Type type;
        std::string shortDesc;
        std::stringstream longDesc;
};

std::ostream& operator<< (std::ostream &os, const RuntimeViolation & m);


class ViolationPolicy {
    public:
        enum Type {
            Exit,
            Warn,
            Ignore,
            InvalidatePointer,

            Invalid
        };
};


template <class T>
inline
void unused(const T&) {}

#endif
