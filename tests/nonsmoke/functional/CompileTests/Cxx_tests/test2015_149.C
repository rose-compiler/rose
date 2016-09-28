
namespace boost { namespace parameter {

template <class Tag>
struct keyword
   {
     static keyword<Tag> const instance;
   };

template <class Tag> keyword<Tag> const keyword<Tag>::instance = {};

}} 



namespace boost {
  namespace graph {
    namespace keywords {

#if 1
      namespace tag { 
         struct weight_map 
            { 
              static char const* keyword_name() { return "weight_map"; } 
           // typedef boost::parameter::value_type< boost::mpl::_2, weight_map, boost::parameter::void_ > _; 
           // typedef boost::parameter::value_type< boost::mpl::_2, weight_map, boost::parameter::void_ > _1; 
            }; 
      } 

      namespace { ::boost::parameter::keyword<tag::weight_map> const& _weight_map = ::boost::parameter::keyword<tag::weight_map> ::instance; } 
#endif

#if 1
      namespace tag { 
           struct weight_map2 
              { 
                static char const* keyword_name() { return "weight_map2"; } 
             // typedef boost::parameter::value_type< boost::mpl::_2, weight_map2, boost::parameter::void_ > _; 
             // typedef boost::parameter::value_type< boost::mpl::_2, weight_map2, boost::parameter::void_ > _1; 
              }; 
      } 

      namespace { ::boost::parameter::keyword<tag::weight_map2> const& _weight_map2 = ::boost::parameter::keyword<tag::weight_map2> ::instance; } 
#endif

#if 1
      namespace tag { 
           struct distance_map 
              { 
                static char const* keyword_name() { return "distance_map"; } 
             // typedef boost::parameter::value_type< boost::mpl::_2, distance_map, boost::parameter::void_ > _; 
             // typedef boost::parameter::value_type< boost::mpl::_2, distance_map, boost::parameter::void_ > _1; 
              }; 
      } 

      namespace { ::boost::parameter::keyword<tag::distance_map> const& _distance_map = ::boost::parameter::keyword<tag::distance_map> ::instance; } 
#endif

    }
  }

} 

