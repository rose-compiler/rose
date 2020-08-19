
class globalID 
   {
  // private:
          friend globalID operator * (const int &, const globalID &) ;

     public:
          inline globalID() {};
   };

void foobar()
   {
     globalID x;
   }
