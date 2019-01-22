struct ints 
   {
     int first;
     int second;
     int getfirst() const volatile 
        {
         return first;
        }

     int getsecond() const volatile 
        {
          return second;
        }
      // note that you could also overload on volatile-ness, just like
      // with const-ness
   };

// could also be mapped by the linker. 
void foobar()
   {
     ints const volatile &p = *reinterpret_cast<ints*>(0x378L);
   }
