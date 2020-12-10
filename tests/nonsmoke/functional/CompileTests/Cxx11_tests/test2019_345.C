

class myVector
   {
     public:
          myVector operator/( double x) const;

          myVector();

          myVector operator= (enum class vector &x) const;
       // myVector operator= (enum vector &x) const;
   };

void foo()
   {
     myVector a;

  // Problem code
     myVector b = a / 1.0;

   }
