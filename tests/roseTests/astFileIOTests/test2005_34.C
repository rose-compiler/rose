// Original Code:
//     std::string arg2 = (std::string) (std::string)std::string("") ;
// Generated Code:
//     std::string arg2(std::basic_string < char , std::char_traits< char > , std::allocator< char > > (std::basic_string < char , std::char_traits< char > , std::allocator< char > > ((""))));

#include<string>

void foo()
   {
#if 1
  // std::string arg2 = (std::string) (std::string)std::string("") ;
     std::string arg2 = "";
     std::string arg3 = std::string("");
     std::string arg4 = (std::string)std::string("");
#endif

     std::string arg5 = (std::string) (std::string)std::string("");

#if 1
     std::string arg6 = std::string (std::string(""));
     std::string arg7 = std::string( std::string (std::string("")));
     std::string arg8(std::string(""));
  // std::string arg9(std::string (std::string("")));
  // std::string arg10(std::string( std::string (std::string(""))));
#endif
   }

class X
   {
     public: 
          X(int) {}
          X(const X & x) {}
   };

class Y
   {
     public: 
       // Y(int) {}
          Y(const X & x) {}
   };

void foobar()
   {
      X x1 = 1;
      X x2 = X(1);
      X x3 = (X)X(1);
      X x4(1);
      X x5(X(1));

   // Y y0 = 1;  // illegal C++ code: no autopromotion via X type
      Y y1 = X(1);

   // This fails to compile with g++ which expressed (unparsed) as "class Y y2(X(X(1)));"
   // Y y2 = X(X(1));
   }

