
#if 0
template <class T, class B >
struct less_than_comparable1 : B
   {
  // DQ: There should be a reference to this function in any template class instantiation of this template.
     friend bool operator<=(const T& x, const T& y) { return false; }
   };
#endif



namespace boost 
   {
     namespace detail { template <typename T> class empty_base { }; } 

     template <class T, class B >
     struct less_than_comparable1 : B
        {
       // DQ: There should be a reference to this function in any template class instantiation of this template.
          friend bool operator<=(const T& x, const T& y) { return false; }
        };

     namespace detail 
        {
          struct true_t {};
          struct false_t {};
        }

     template<class T> struct is_chained_base {  typedef ::boost::detail::false_t value; };

     template <class T ,class U = T ,class B = ::boost::detail::empty_base<T> ,class OOO = typename is_chained_base<U> ::value > struct less_than_comparable {};
     template<class T, class U, class B> struct less_than_comparable<T, U, B, ::boost::detail::true_t> : less_than_comparable1<T, U> {}; 
     template <class T, class B> struct less_than_comparable<T, T, B, ::boost::detail::false_t> {};
     template<class T, class U, class B, class O> struct is_chained_base< ::boost::less_than_comparable<T, U, B, O> > { typedef ::boost::detail::true_t value; };  

     template <class T ,class U = T ,class B = ::boost::detail::empty_base<T> ,class OOOOOO = typename is_chained_base<U> ::value > struct equality_comparable {};
     template<class T, class U, class B> struct equality_comparable<T, U, B, ::boost::detail::true_t> {};
     template <class T, class B> struct equality_comparable<T, T, B, ::boost::detail::false_t> {};
     template<class T, class U, class B, class O> struct is_chained_base< ::boost::equality_comparable<T, U, B, O> > { typedef ::boost::detail::true_t value; };  

     namespace date_time 
        {
       // DQ: Note that the derivation from boost::less_than_comparable is required to reproduce the bug.
          template <class T, class time_system>
          class base_time : private boost::less_than_comparable<T, boost::equality_comparable<T> >
             {
               public:
                    typedef T time_type;
                    bool operator<(const time_type& rhs) const;
             };
        } 

     namespace posix_time 
        {
          class ptime : public date_time::base_time<ptime, long>
             {
               public:
                    ptime();      
             };
        }

     typedef boost::posix_time::ptime system_time;

     namespace detail
        {
          void get_milliseconds_until()
             {
               system_time const now;
               system_time const then;

            // DQ: This calls the operator <= ().
               then <= now;
             }
        }
   }


