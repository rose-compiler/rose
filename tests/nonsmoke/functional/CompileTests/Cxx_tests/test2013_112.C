
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
     ArgumentListIterator ali;
     for (; ; ++ali)
        {
        }
   }
