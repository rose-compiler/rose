template <typename T>
class X
   {
     public: 
          class iterator 
             {
               public: 
                    bool operator!= (iterator rhs);
                 // iterator operator++(int x);
                    iterator operator++();
             };

          iterator foo();
   };

void foobar()
   {
     X<int*> *my_list;
     X<int*>::iterator it;

  // The problem is that the increment and test are marked as compiler generated.
     for ( it = my_list->foo(); 
           it != my_list->foo(); ++it ) 
        {
        }
   }
