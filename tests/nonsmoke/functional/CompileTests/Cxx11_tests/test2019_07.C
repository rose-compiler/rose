class Element;

template <class T> class Array;

// NOTE: call to bar() in Mesh::foo() is resolved to this function.
template <class T> void bar(Array<T> &);

template <class T>
class Array 
   {
     friend void bar<T>(Array<T> &); // << NEEDED

  // public: template <class T> void bar(Array<T> &);
   };

class Mesh
   {
     protected:
          Array<Element *> elements;
          void bar();

  // public: template <class T> void bar(Array<T> &);

     public:
          void foo();
   };

void Mesh::foo()
   {
  // Should be unparsed as: ::bar(elements);  but is unparsed as: bar < Element * > ((this) -> elements);
  // Missing global qualifications required to generate: ::bar < Element * > ((this) -> elements);
     ::bar(elements);
   }

