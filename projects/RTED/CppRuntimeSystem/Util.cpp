#include "Util.h"

#include <iostream>
#include <sstream>
#include <cassert>

using namespace std;

// ------------------------------ Source Position -----------------------------------

SourcePosition::SourcePosition()
: file("Unknown"), line1(0), line2(0)
{}

SourcePosition::SourcePosition(rted_SourceInfo si)
: file(si.file), line1(si.src_line), line2(si.rted_line)
{}

std::string SourcePosition::toString() const
{
    stringstream stream;
    stream << *this;
    return stream.str();
}

ostream& operator<< (ostream &os, const SourcePosition & m)
{
    os << m.getFile() << "("
       << m.getLineInOrigFile() << ","
       << m.getLineInTransformedFile() << ")";
    return os;
}

string SourcePosition::getTransformedFile() const
{
    int pos = file.find_last_of(".");
    string fileWithoutSuffix  = file.substr(0,pos);

    string suffix;
    if(pos <= (int)file.size() )
        suffix = file.substr(pos);

    return fileWithoutSuffix + "_rose" + suffix;
}



// ---------------------------- Runtime Violation -----------------------------------


RuntimeViolation::RuntimeViolation(Type _type, const std::string& desc)
    : /* runtime_error("\n" + getShortDescFromType(_type) + "\n" + desc + "\n"), */
      type(_type),
      shortDesc(getShortDescFromType(_type))
{
    longDesc << desc;
}

RuntimeViolation::RuntimeViolation(Type _type, const std::stringstream & descStream)
    : /* runtime_error("\n" + getShortDescFromType(_type) + "\n" + descStream.str() + "\n"), */
      type(_type),
      shortDesc(getShortDescFromType(_type))
{
    longDesc << descStream.str();
}

RuntimeViolation::RuntimeViolation(const RuntimeViolation & other)
    : pos(other.pos),
      type(other.type),
      shortDesc(other.shortDesc)
{
    longDesc << other.longDesc.str();
}


std::ostream& operator<< (std::ostream &os, const RuntimeViolation & m)
{
	os << "Violation: ";
    os << m.getShortDesc() << " at " << m.getPos() << endl;
    os << m.descStream().str() << endl;
    return os;
}


std::string RuntimeViolation::getShortDescFromType(Type type)
{
    switch(type)
    {
        case DOUBLE_ALLOCATION:       return "Double Allocation fault";
        case INVALID_FREE:            return "Invalid Free";
        case MEMORY_LEAK:             return "Memory Leaks";
        case EMPTY_ALLOCATION:        return "Empty allocation";
        case INVALID_READ:            return "Invalid Read";
        case INVALID_WRITE:           return "Invalid Write";
        case INVALID_FILE_OPEN:       return "fopen returned NULL";
        case DOUBLE_FILE_OPEN:        return "FileHandle registered twice";
        case INVALID_FILE_CLOSE:      return "Invalid File close";
        case INVALID_FILE_ACCESS:     return "Invalid File Access";
        case UNCLOSED_FILES:          return "Open Files at end of program";
        case INVALID_PTR_ASSIGN:      return "An invalid address was assigned to pointer";
        case MEM_WITHOUT_POINTER:     return "A Memory Region cannot be reached by any pointer";
        case POINTER_CHANGED_MEMAREA: return "Pointer changed Target-MemoryRegion";
		case INVALID_MEM_OVERLAP:	  return "Illegal Memory Overlap";
	    case INVALID_TYPE_ACCESS:     return "Invalid access to \"typed\" memory";
		case UNEXPECTED_FUNCTION_SIGNATURE:	return "Incorrect types at function call";
        case NONE:                    return "Not a violation";
        default:
            // handle all possible violations!
            assert(false);
    }
}


RuntimeViolation::Type RuntimeViolation::getViolationByString(const string & s)
{
    if       (s == "DOUBLE_ALLOCATION")        return DOUBLE_ALLOCATION;
    else if  (s == "INVALID_FREE")             return INVALID_FREE;
    else if  (s == "MEMORY_LEAK")              return MEMORY_LEAK;
    else if  (s == "EMPTY_ALLOCATION")         return EMPTY_ALLOCATION;
    else if  (s == "INVALID_READ")             return INVALID_READ;
    else if  (s == "INVALID_WRITE")            return INVALID_WRITE;
    else if  (s == "INVALID_FILE_OPEN")        return INVALID_FILE_OPEN;
    else if  (s == "DOUBLE_FILE_OPEN")         return DOUBLE_FILE_OPEN;
    else if  (s == "INVALID_FILE_CLOSE")       return INVALID_FILE_CLOSE;
    else if  (s == "INVALID_FILE_ACCESS")      return INVALID_FILE_ACCESS;
    else if  (s == "UNCLOSED_FILES")           return UNCLOSED_FILES;
    else if  (s == "INVALID_PTR_ASSIGN")       return INVALID_PTR_ASSIGN;
    else if  (s == "MEM_WITHOUT_POINTER")      return MEM_WITHOUT_POINTER;
    else if  (s == "POINTER_CHANGED_MEMAREA")  return POINTER_CHANGED_MEMAREA;
    else if  (s == "INVALID_MEM_OVERLAP")      return INVALID_MEM_OVERLAP;
    else if  (s == "INVALID_TYPE_ACCESS")      return INVALID_TYPE_ACCESS;
    else if  (s == "UNEXPECTED_FUNCTION_SIGNATURE") return UNEXPECTED_FUNCTION_SIGNATURE;
    else                                       return UNKNOWN_VIOLATION;
}
