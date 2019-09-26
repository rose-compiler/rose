
// ROSE-1900: (C++03) extern added to friend function inside extern C++ block
extern "C++" {
     void current_exception();

     class exception_ptr 
        {
       // Unparses as: "public: friend extern void ::current_exception();"
       // "extern" should not be unparsed.  Maybe "friend" and "extern" should noever go together.
          friend void current_exception();
        };
   }
