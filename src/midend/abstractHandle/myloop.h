/*
 * A toy loop data structure demonstrating a thin client of abstract handles:
 * A simplest loop tool which keeps a flat list of loop information in a file
 */
#ifndef my_loop_INCLUDED
#define my_loop_INCLUDED

#include <string>
class MyLoop 
{
public:  
  std::string sourceFileName;
  size_t line_number;
  std::string loop_code;
};

#endif
