class classType
   {
     public:
//        double publicDoubleValue;

     protected:
          static double protectedStaticDoubleValue;
   };

// Error associated with assoignment of integer (or so it seems!)
// double classType::protectedStaticDoubleValue = 0;

// Another error identified (isolated from test2004_30.C)
double classType::protectedStaticDoubleValue;

// Need something else in the file (such as the code below) else the traversal fails!
// int x;
// classType X;


