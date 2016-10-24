
// This test code tests a number of different ways that templates can be used

template < class T >
class TemplatedClass
   {
     public:
       // template < class S > S publicTemplateMemberFunction (T t) {}
          template < class S > S publicTemplateMemberFunction (T t) {}
   };




