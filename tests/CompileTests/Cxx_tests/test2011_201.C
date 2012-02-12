
template<typename _Tp>
class X
   {
     public:
          X(int & __a) { }
   };

template<typename _Tp>
class allocator: public X<_Tp>
   {
     public:
       // Demonstrate case of direct base class in preinitialization list.
          allocator(const int & __a) : X<_Tp>(__a) { }        
   };

