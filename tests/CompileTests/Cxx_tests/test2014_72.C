
namespace boost
   {
     template<class T> class shared_ptr;

     namespace detail
        {
          template< class T > struct sp_member_access
             {
               typedef T * type;
             };
        }

     template<class T> class shared_ptr
        {
          private:
               typedef shared_ptr<T> this_type;

          public:

               shared_ptr();

               typename boost::detail::sp_member_access< T >::type operator-> () const;
        };
   }


typedef int LocalId;

class Patch
   {
     public:
          const LocalId & getLocalId() const;
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

void Refine_postProcessFluxLimiter( double phi_floor )
   {
     PatchLevel::iterator pi;

     int mbid = pi->getLocalId();
   }


