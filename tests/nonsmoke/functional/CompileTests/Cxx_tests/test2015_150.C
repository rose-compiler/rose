template <class Tag>
struct keyword
   {
  // This is a static template variable.
     static keyword<Tag> const instance;
   };

template <class Tag> keyword<Tag> const keyword<Tag>::instance = {};

#if 1
struct weight_map {};

keyword<weight_map> const& _weight_map = keyword<weight_map> ::instance;
#endif

#if 1
struct weight_map2 {};

keyword<weight_map2> const& _weight_map2 = keyword<weight_map2> ::instance;
#endif

#if 1
struct distance_map {};

keyword<distance_map> const& _distance_map = keyword<distance_map> ::instance;
#endif

