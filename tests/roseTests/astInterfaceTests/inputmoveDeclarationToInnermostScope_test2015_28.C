namespace std {
   template<typename _CharT >
   class basic_string
    {
    public:
      basic_string(const _CharT* __s);
     ~basic_string() { }
    };

   typedef basic_string<char> string;

   string grouping() { return ""; }

   void foo()
     {
       int x;
       if (1)
          {
            x = 4;
          }
     }
}
