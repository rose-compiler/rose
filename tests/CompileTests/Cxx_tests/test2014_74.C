template<class T> 
class shared_ptr
   {
     public:
          T* operator-> () const;
   };

class Patch
   {
     public:
          int x;
   };

class Iterator
   {
     public:
          Iterator();
          const shared_ptr<Patch> & operator->() const;           
   };

void foo()
   {
     Iterator pi;

     int mbid = pi->x;
   }

