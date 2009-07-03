#include "Util.h"

#include <iostream>

using namespace std;


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

