#ifndef POST_PRE_PROC_H_
#define POST_PRE_PROC_H_

#include <iostream>
#include <fstream>
#include <string>


class PostPreProc
{
 public:
   PostPreProc();
   void process(std::istream & in_stream, std::ostream & out_stream);

};

#endif
