namespace std
   {
     class X
        {
          public:
               template <typename T>
               friend bool operator== (X,int);
        };

     template <typename T>
     bool operator== (X,int)
        {
          return false;
        }

   }
