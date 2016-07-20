namespace boost {

template <class T> class rv;

}  

namespace boost {
namespace container {

template <typename Alloc> struct allocator_traits {};

}  
}  

namespace boost { 
namespace container {

namespace container_detail {

template < typename OuterAlloc , bool Dummy , class Q0 , class Q1 , class Q2 , class Q3 , class Q4 , class Q5 , class Q6 , class Q7 , class Q8 , class Q9 > class scoped_allocator_adaptor_base;

template <typename OuterAlloc , class Q0 , class Q1 , class Q2 , class Q3 , class Q4 , class Q5 , class Q6 , class Q7 , class Q8 , class Q9 > class scoped_allocator_adaptor_base<OuterAlloc, true , Q0 , Q1 , Q2 , Q3 , Q4 , Q5 , Q6 , Q7 , Q8 , Q9  > : public OuterAlloc 
   { 
     typedef allocator_traits<OuterAlloc> outer_traits_type; 
     public: 
          scoped_allocator_adaptor_base& operator=(scoped_allocator_adaptor_base &t);
   };

}  

template <typename OuterAlloc , class Q0 , class Q1 , class Q2 , class Q3 , class Q4 , class Q5 , class Q6 , class Q7 , class Q8 , class Q9 >
class scoped_allocator_adaptor
{
   public:
   typedef container_detail::scoped_allocator_adaptor_base <OuterAlloc , true , Q0 , Q1 , Q2 , Q3 , Q4 , Q5 , Q6 , Q7 , Q8 , Q9 > base_type;
   typedef OuterAlloc                                       outer_allocator_type;   
   typedef allocator_traits<OuterAlloc>                     outer_traits_type;
   
   template <class U>
   struct rebind
   {
      typedef scoped_allocator_adaptor < typename outer_traits_type::template portable_rebind_alloc<U>::type , Q0 , Q1 , Q2 , Q3 , Q4 , Q5 , Q6 , Q7 , Q8 , Q9 > other;
   };

  scoped_allocator_adaptor(::boost::rv< scoped_allocator_adaptor > & other) {}

  scoped_allocator_adaptor &operator=(const ::boost::rv< scoped_allocator_adaptor > & other)
     {  return static_cast<scoped_allocator_adaptor&>(base_type::operator=(static_cast<const base_type &>(other))); }

};


}} 

