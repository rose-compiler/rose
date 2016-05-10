template <class Tag>
struct keyword
   {
  // This is a static template variable.
     static keyword<Tag> instance;
   };

template <class Tag> keyword<Tag> keyword<Tag>::instance = {};

#if 1
struct weight_map {};

// keyword<weight_map> _weight_map = keyword<weight_map> ::instance;
#endif

#if 1
struct weight_map2 {};

// keyword<weight_map2> _weight_map2 = keyword<weight_map2> ::instance;
#endif

#if 1
struct distance_map {};

// keyword<distance_map> _distance_map = keyword<distance_map> ::instance;
#endif

void foobar()
   {
     keyword<weight_map> ::instance;
     keyword<weight_map2> ::instance;
     keyword<distance_map> ::instance;
   }
