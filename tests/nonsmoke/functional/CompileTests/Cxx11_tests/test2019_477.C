// This is one of three ROSE bugs specific to the SL application.
// This and the test2019_476.C appear to be related issues.


typedef unsigned long size_t;

template < class Key, class T>
class Map
   {
     public: 
          Map(size_t maxSize);
          void insert (const Key& key, const T& data);
   };

class A
   {
     public:
          enum Type
            {
              VALUE
            };
   };

class B : public A
   {
     public:
          B();
          A* getValue(A::Type x);

     private:
          enum C
             {
               VALUE
             };
          Map<A::Type,C> myMap;
   };

B::B()
   : myMap(VALUE)
   {
  // Comment this out to simplify the reproducer.
     myMap.insert(A::VALUE,VALUE);

     getValue(A::VALUE);
   }
