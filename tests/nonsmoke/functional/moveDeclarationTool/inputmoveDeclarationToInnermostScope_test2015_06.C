namespace std {
   template<typename _CharT> class basic_string;
   typedef basic_string<char> string;

   template<typename _CharT >
   class basic_string
    {
    public:
      basic_string(const _CharT* __s);
     ~basic_string() { }
    };

   string grouping() { return ""; }

#if 1
  void foo()
     {
       int x;
       if (1)
          {
            x = 4;
          }
     }

}  // closing brace for namespace statement

#endif

