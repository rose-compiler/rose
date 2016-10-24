
class globalID 
   {
     public:
          inline explicit globalID(int i);
          inline globalID operator * (const int i) const;
          inline globalID operator + (const int i) const;
   };


int main(int argc, char *argv[])
   {
  // The error is that this is unparsed as: "class globalID value = (globalID::globalID(100)7) + 5;"
     globalID value = globalID(100) * 7 + 5 ;

     return 0 ;
   }

