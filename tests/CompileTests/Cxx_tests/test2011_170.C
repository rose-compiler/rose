// Example of template member data.

template < class T >
class ClassTypeWithStaticData
   {
     public:
          static T templateParameterVariable;
   };

template < class T >
T ClassTypeWithStaticData<T>::templateParameterVariable = 0;
