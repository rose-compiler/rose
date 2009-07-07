
#ifndef UTIL_H
#define UTIL_H


#include <string>



typedef unsigned long addr_type;



/// Compare Functor, which compares two pointer by comparing the objects they point to
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
    SourcePosition();
    SourcePosition(const std::string & file);
    SourcePosition(const std::string & file,int line1, int line2);

    std::string file;   ///< Absolute Path of source-file
    int line1;          ///< line number in sourcefile
    int line2;          ///< line number in transformed sourcefile
};
std::ostream& operator<< (std::ostream &os, const SourcePosition & m);







class RuntimeViolation
{
    public:

        enum Type
        {
                DOUBLE_ALLOCATION,   // try to reserve memory with lies in already allocated mem
                INVALID_FREE,        // called free on non allocated address
                MEMORY_LEAK,         // some allocated memory was not freed on program exit
				INVALID_MEM_OVERLAP, // some memory chunk overlaps with some
									 // other memory chunk illegaly, e.g. in arguments to memcpy
                EMPTY_ALLOCATION,    // trying to get a memory area of size 0
                INVALID_READ,        // trying to read non-allocated or non-initialized mem region
                INVALID_WRITE,       // trying to write to non-allocated mem region
                DOUBLE_FILE_OPEN,    // trying to register the same file-pointer twice
                INVALID_FILE_CLOSE,  // trying to close a file which has already been closed / not opened
                INVALID_FILE_ACCESS, // trying to access file which is not opened
                UNCLOSED_FILES       // some opened files where not closed before program exit
        };


        explicit RuntimeViolation(Type type, const std::string& desc = "");
        explicit RuntimeViolation(Type type, const std::stringstream & descStream);

        virtual ~RuntimeViolation() throw() {}


        const std::string & getShortDesc() const { return shortDesc; }
        const std::string & getLongDesc()  const { return longDesc; }

        void setPosition(const SourcePosition & sourcePos)  { pos=sourcePos; }
        const SourcePosition & getPos() const               { return pos; }


        Type getType() const { return type; }
    protected:

        static std::string getShortDescFromType(Type type);

        SourcePosition pos;
        Type type;
        std::string shortDesc;
        std::string longDesc;
};

std::ostream& operator<< (std::ostream &os, const RuntimeViolation & m);





#endif
