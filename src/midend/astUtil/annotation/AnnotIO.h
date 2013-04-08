#ifndef ANNOT_IO_H
#define ANNOT_IO_H

#include <iostream>
#include <string>
#include "rosedll.h"

bool is_space( char c);
bool is_num( char c);
bool is_id( char c);
int is_operator( char c);

char peek_next(std::istream& in);
std::string peek_next( std::istream& in, unsigned size);

char peek_ch (std::istream& in);
char read_ch( std::istream& in );
void read_ch( std::istream& in, char c);

std::string peek_id( std::istream& in);
std::string read_id( std::istream& in );
void read_id( std::istream& in, const std::string& s);

std::string read_num( std::istream& in );
std::string read_operator( std::istream& in );

ROSE_DLL_API bool DebugAnnot();

class ReadError { 
 public:
  std::string msg;
  ReadError( const std::string& s) : msg(s) {}
};
//! Read a list of items of type ItemGen, they are separated using 'sep'
template <class ItemGen>
bool read_list( std::istream& in, ItemGen& op, char sep )
{
  // Read the first item in the list
  if (! op.read(in))
     return false;
  while (in.good()) {
    char c = peek_ch(in);
    //Stop if the next char is not the delimitor
    if (sep != 0 && c != sep) 
       break;
    // else we keep reading  
    if (sep != 0)
       read_ch(in,c);
    if (!  op.read(in))
       break;
  } 
  return true;
}

template <class Item, class Container>
class ReadContainerWrap
{
  Container& c;
 public:
  ReadContainerWrap( Container& _c) : c(_c) {}
  bool read( std::istream& in) 
   { 
     Item item;
     if (!item.read(in))
         return false;
     c.push_back( item );
     return true;
   }
};

#endif
