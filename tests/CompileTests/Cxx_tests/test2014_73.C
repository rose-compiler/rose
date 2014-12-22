namespace boost
   {
     template<class T> class shared_ptr
        {
          public:
               T* operator-> () const;
        };
   }

class Patch
   {
     public:
          int x;
   };

class PatchLevel
   {
     public:
          class Iterator
             {
               public:
                    Iterator();
                    const boost::shared_ptr<Patch> & operator->() const;           
             };

          typedef Iterator iterator;
   };

void Refine_postProcessFluxLimiter()
   {
     PatchLevel::iterator pi;

     int mbid = pi->x;
   }


