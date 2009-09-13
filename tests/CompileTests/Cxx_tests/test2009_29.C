// DQ (9/12/2009): Bug reported by Greg Bronevetsky.
// struct char_traits<char> : std::char_traits<char> { char newline(); };

#include <iostream>
#include <streambuf>
#include <sstream>

#if 0
namespace namespace1
   {
     namespace namespace2
        {
           namespace namespace3
              {
             // class chainbuf : public std::basic_stringbuf<char> {};
                class chainbuf : public std::basic_stringbuf<char> {};
              }
        }
   }
#endif

#if 0
namespace namespace3
   {
  // class chainbuf : public std::basic_stringbuf<char> {};
     class chainbuf : public std::basic_stringbuf<char> {};
   }
#endif

// DQ (9/12/2009): This is the simplist example of the bug that remains (the other associated bugs have been fixed)
class chainbuf : public std::basic_stringbuf<char> {};
