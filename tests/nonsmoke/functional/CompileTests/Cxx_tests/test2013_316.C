
template<typename _Iterator, typename _Container>
class __normal_iterator
   {
     public:
          __normal_iterator & operator++()
             {
	            return *this;
             }

           __normal_iterator operator++(int)
             {
               return *this; 
             }
   };


void foo()
   {
     __normal_iterator<int,short> x;
     x++;
     ++x;
   }



   
