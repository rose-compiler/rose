
int findme1;

void fooBefore () 
   {}

template <class T >
class simplelist
   {
     public:
          typedef T value_type;
          typedef value_type* pointer;
//        friend bool operator== ( const simplelist & __x, const simplelist & __y );
//        bool operator== ( const simplelist & __y );
//        friend bool equality ( const simplelist & __x, const simplelist & __y );
//        friend bool equality ();
//        friend void equality ();
          void equality ();
          void greater_than ();
   };

void fooAfter () 
   {}

int findme2;
