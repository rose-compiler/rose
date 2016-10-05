
namespace boost { 
     namespace parameter {

          template <class Tag>
          struct keyword
             {
               public:
                    static keyword<Tag> const instance;

                    static keyword<Tag>& get()
                       {
                         return const_cast<keyword<Tag>&>(instance);
                       }
             };

          template <class Tag>
          keyword<Tag> const keyword<Tag>::instance = {};

        }
   }

// Build an instantiation.
boost::parameter::keyword<int> key;

void foobar()
   {
     key.instance;
   }
