template <class Tag>
struct keyword
   {
  // This is a static template variable.
     static keyword<Tag> const instance;
   };

// template <class Tag> keyword<Tag> const keyword<Tag>::instance = {};

#if 1
struct weight_map {};

keyword<weight_map> const& _weight_map = keyword<weight_map> ::instance;
#endif

