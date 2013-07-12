namespace protobuf {

class DescriptorBuilder 
   {
  // Ths scope of this non-defining declaration is not correct (set incorrectly to the outer scope).
  // EDG reports the scope of the secondary declaration to be the namespace protobuf!!!
//     friend class OptionInterpreter;

     public:
          class OptionInterpreter 
             {
               void foo (DescriptorBuilder & y)
                  {
                    int a = y.x;
                  }
             };

     private:
          int x;
   };

}  // namespace protobuf


