
// This code passes for EDG, but fails for GNU 5.1 (so the identity translator fails in the backend as well).

class myVector
   {
     public:
          myVector operator/( double x) const;

          myVector();

       // Failing code!
       // myVector operator= (enum class vector &x) const;
          myVector operator= (enum vector &x) const;
   };

void foo()
   {
     myVector a;

  // Problem code
     myVector b = a / 1.0;

   }
