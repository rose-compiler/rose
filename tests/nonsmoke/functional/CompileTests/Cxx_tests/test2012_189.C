#if 0
template <typename T>
class _Rb_tree_iterator
   {
     public:
          typedef T  value_type;
   };
#endif

template <typename T>
class map
   {
     public:
#if 0
         typedef T  value_type;
         typedef _Rb_tree_iterator<value_type>       iterator;
#else
      // This case will work...but map is translated to the typedef base type.
      // typedef _Rb_tree_iterator<int>       iterator;
         typedef int       iterator;
#endif
   };

class map_no_template
   {
     public:
#if 0
         typedef T  value_type;
         typedef _Rb_tree_iterator<value_type>       iterator;
#else
      // This case will work...but map is translated to the typedef base type.
      // typedef _Rb_tree_iterator<int>       iterator;
         typedef int       iterator;
#endif
   };

class foo
   {
     public:
          void foobar()
             {
            // This will fail (seems it has to be a part of a template.
               map<int>::iterator it;

            // This will preserve the typedef type in the class map_no_template
               map_no_template::iterator it2;
             }
   };




