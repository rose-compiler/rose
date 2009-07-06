#include "Util.h"

#include <iostream>
#include <sstream>
#include <cassert>

using namespace std;

// ------------------------------ Source Position -----------------------------------

SourcePosition::SourcePosition()
    : file("Unknown"),
      line1(-1),
      line2(-1)
{

}

SourcePosition::SourcePosition(const std::string & _file)
    : file(_file),
      line1(-1),
      line2(-1)
{}

SourcePosition::SourcePosition(const std::string & _file,int _line1, int _line2)
    : file(_file),
      line1(_line1),
      line2(_line2)
{}




ostream& operator<< (ostream &os, const SourcePosition & m)
{
    os << m.file << "(" << m.line1 << "," << m.line2 << ")";
    return os;
}



// ---------------------------- Runtime Violation -----------------------------------


RuntimeViolation::RuntimeViolation(Type _type, const std::string& desc)
    : /* runtime_error("\n" + getShortDescFromType(_type) + "\n" + desc + "\n"), */
      type(_type),
      shortDesc(getShortDescFromType(_type)),
      longDesc(desc)
{

}

RuntimeViolation::RuntimeViolation(Type _type, const std::stringstream & descStream)
    : /* runtime_error("\n" + getShortDescFromType(_type) + "\n" + descStream.str() + "\n"), */
      type(_type),
      shortDesc(getShortDescFromType(_type)),
      longDesc(descStream.str())
{
}


std::ostream& operator<< (std::ostream &os, const RuntimeViolation & m)
{
    os << m.getShortDesc() << " at " << m.getPos() << endl;
    os << m.getLongDesc() << endl;
    return os;
}


std::string RuntimeViolation::getShortDescFromType(Type type)
{
    switch(type)
    {
        case DOUBLE_ALLOCATION:     return "Double Allocation fault";
        case INVALID_FREE:          return "Invalid Free";
        case MEMORY_LEAK:           return "Memory Leaks";
        case EMPTY_ALLOCATION:      return "Empty allocation";
        case INVALID_READ:          return "Invalid Read";
        case INVALID_WRITE:         return "Invalid Write";
        case DOUBLE_FILE_OPEN:      return "FileHandle registered twice";
        case INVALID_FILE_CLOSE:    return "Invalid File close";
        case INVALID_FILE_ACCESS:   return "Invalid File Access";
        case UNCLOSED_FILES:        return "Open Files at end of program";

        default:
            // handle all possible violations!
            assert(false);
    }
}



