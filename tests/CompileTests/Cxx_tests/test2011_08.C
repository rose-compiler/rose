// const int arraySize = 16;
// int valueArray[arraySize+10];

class X
   {
     public:
       // int A() const;

          int getValue() const
             {
//             return valueArray[int(A())];
               return valueArray[0];
               return 42;
             }

       // static const int arraySize = 16;
          static const int arraySize = 16;

       // This causes an assertion error: previousInitializedName->get_parent() != __null
       // static const int valueArray[arraySize+10];
       // static const int valueArray[16+10];
          static const int valueArray[arraySize];
   };
