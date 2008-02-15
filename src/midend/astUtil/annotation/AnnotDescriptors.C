#include <general.h>

#include <AnnotDescriptors.h>
#include <sstream>
#include <list>

#ifndef ANNOT_DESCRIPTORS_C
#define ANNOT_DESCRIPTORS_C

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

#ifndef TEMPLATE_ONLY

bool TypeDescriptor:: read(istream& in)
{
  if (!StringDescriptor::read(in))
        return false;
  if (get_name() == "const")
     get_name() = get_name() + read_id(in);
  char c = peek_ch(in);
  while ( c == ':' && peek_next(in,2) == "::") {
     read_ch(in,':');
     read_ch(in,':');
     get_name() = get_name() + "::" + read_id(in);
     c = peek_ch(in);
  } 
  if (c == '&' || c == '*') {
    read_ch(in,c);
    get_name().push_back(c);
  }
  return true;
}
#endif

#endif
