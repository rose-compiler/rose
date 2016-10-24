// This test code tests a number of different ways that templates can be used

#if 1
template < class T > void templateFunction () {};
#endif

#if 1
class ClassType
   {
     public:
       // EDG BUG: EDG IR does not record use of export keyword on template member functions
       // EDG BUG: member function template marked as template function instead of templeate member function in EDG
     public   : template < class T > void publicTemplateMemberFunction    () {}
     protected: template < class T > void protectedTemplateMemberFunction () {}
     private  : template < class T > void privateTemplateMemberFunction   () {}

  // Note that friend functions are always public (not certain if this is correct behavior for C++)
     protected: template < class T > friend void protectedTemplateFriendFunction () {}
     public   : template < class T > friend void publicTemplateFriendFunction    () {}
     private  : template < class T > friend void privateTemplateFriendFunction   () {}
   };
#endif

#if 1
template < class T >
class TemplatedClass
   {
     public:
          T templateParameterVariable;

          TemplatedClass() {}
         ~TemplatedClass() {}
          T returnTemplateParameter();
          T returnTemplateParameterWithDefinition() {}

          template < class S > S publicTemplateMemberFunction (T t) {}

          static T t1;
   };
#endif

#if 1
template < class T >
class ClassTypeWithStaticData
   {
     public:
          static T templateParameterVariable;
   };

template < class T >
T ClassTypeWithStaticData<T>::templateParameterVariable = 0;
#endif

#if 1
template < class T >
class TemplateClassTypeWithNestedClass
   {
     public:
          int integerVariable;
          T templateParameterVariable;

          template < class S >
          class NestedClass
             {
               public:
                    S templateParameterVariable;
             };
   };
#endif

#if 1
class ClassTypeWithNestedClass
   {
     public:
          int integerVariable;

          template < class S >
          class NestedClass
             {
               public:
                    static S templateParameterVariable;
             };
   };

template < class S >
S ClassTypeWithNestedClass::NestedClass<S>::templateParameterVariable = 0;
#endif


