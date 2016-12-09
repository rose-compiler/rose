class A
   {
#if 1
  // Note that this member function is required to deomnstrate the bug.
     public: 
          void biz() {} 
#endif
};

// The bug is that this is unparsed as "extern" instead of "extern "C""
// extern "C" int printf (const char* fmt, ...);
extern "C" int printf (const char* fmt);
