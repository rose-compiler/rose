class Test
   {
     public:
          Test & operator+=(const Test & x);
          Test & operator*();
   };

void foobar()
   {
     Test s,t;

#if 1
     t += s.operator*();

     t.operator+=(s.operator*());
#endif

#if 1
     *s;
#endif

#if 1
     t.operator+=(*s);

  // This case requires the operator "*" to be unparsed before the "s".
     t += *s;
#endif
   }

