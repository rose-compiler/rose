
class ArgumentListIterator
   {
     public:
       // Postfix operator
       // ArgumentListIterator & operator++(int i);

       // Prefix operator.
          ArgumentListIterator & operator++();
   };

void foobar()
   {
     ArgumentListIterator ali_1;
     ArgumentListIterator ali_2;
     for (; ; ++ali_1,++ali_2)
        {
        }
   }
