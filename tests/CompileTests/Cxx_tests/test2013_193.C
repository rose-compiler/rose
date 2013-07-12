#include "string"

std::string SimpleItoa(unsigned int i) 
   {
     char* x;
  // char buffer[kFastToBufferSize];
     char buffer[42];
  // return string(buffer, FastUInt32ToBufferLeft(i, buffer));
  // return string(buffer);

  // This is unparsed as: return std::basic_string< char ,class std::char_traits< char  > ,class std::allocator< char  > > ::basic_string < char * > (buffer,x);
  // Should be unparsed as: return std::basic_string< char ,class std::char_traits< char  > ,class std::allocator< char  > > ::basic_string(buffer,x);
     return std::string(buffer,x);
   }
