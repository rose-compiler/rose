
#ifndef UTIL_H
#define UTIL_H


#include <string>

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
    SourcePosition();
    SourcePosition(const std::string & file);
    SourcePosition(const std::string & file,int line1, int line2);

    std::string file;   ///< Absolute Path of source-file
    int line1;          ///< line number in sourcefile
    int line2;          ///< line number in transformed sourcefile
};
std::ostream& operator<< (std::ostream &os, const SourcePosition & m);



#endif
