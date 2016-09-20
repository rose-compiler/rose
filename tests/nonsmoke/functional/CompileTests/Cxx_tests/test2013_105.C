class Test
   {
     public:
          Test & operator+=(const Test & x);
          Test & operator&();
   };

void foobar()
   {
     Test s,t;

#if 1
     t += s;
#endif

#if 1
     t.operator+=(s);
#endif

#if 1
     t += s.operator&();
#endif

#if 1
     t.operator+=(s.operator&());
#endif

#if 1
     &s;
#endif

#if 1
     t.operator+=(&s);
#endif

#if 1
  // This case requires the operator "*" to be unparsed before the "s".
     t += &s;
#endif
   }
