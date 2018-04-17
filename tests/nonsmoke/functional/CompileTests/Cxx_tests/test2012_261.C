// Test code from Milind:

class parent 
   {
     public:
          double x;
          parent operator=(const parent& rhs)
             {
               x = rhs.x;
               return *this;
             }
   };
 
class child: public parent 
   {
     public:
          child operator=(const child& rhs)
             {
               parent::operator=(rhs);
               return *this;
             }
   };

