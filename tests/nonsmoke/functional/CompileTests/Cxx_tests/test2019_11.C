class myClass
   {
     public:
       // int i;
          myClass &operator = (const myClass &x)
             {
               return *this;
             }
       // myClass(int j); // : i(j) { }
   };

// myClass &(myClass::*functionPointer)(const myClass &x) = &myClass::operator=;
myClass &(myClass::*functionPointer)(const myClass &x);

