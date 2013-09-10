class Argument 
   {
     public:
   };

class ArgumentListIterator
   {
     public:
       // Postfix operator
       // ArgumentListIterator & operator++(int i);

       // Prefix operator.
          ArgumentListIterator & operator++();

          Argument* current();

   };

void foobar()
   {
     ArgumentListIterator ali;
     for (; ; ++ali)
        {
        }

     bool hasDocs = true;
     Argument *a;
     if (hasDocs && (a=ali.current()))
        {
        }
#if 0
     if (hasDocs && a=ali.current())
        {
        }
#endif
   }
