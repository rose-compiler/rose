
class list
   {
  // List class defined similarly to STL List class (but without templates)

     public:
       // fixed element type for list class (to avoid templates)
          typedef int elementType;

     protected:
          struct list_node {
               list_node* next;
               list_node* prev;
               elementType data;
             };

          typedef elementType* pointer;
          typedef elementType& reference;

          typedef list_node* link_type;

       // size_t not defined (since we don't include system header files)
       // so EDG could not compile this line.
       // typedef size_t size_type;
          typedef unsigned size_type;

     protected:
          link_type first;
          link_type last;

          size_type length;

     public:
          class iterator
             {
               friend class list;
               protected:
                    link_type node;
                    iterator(link_type x);

               public:
                    iterator();
                    bool operator==(const iterator& x) const;
                    bool operator!=(const iterator& x) const;
                    reference operator*() const;
                    iterator& operator++();
                    iterator operator++(int);
             };

          list();

          iterator begin();
          iterator end();

          unsigned int size();
       // Error in EDG frontend
       // void push_back(const reference x );
          void push_back( reference x );
   };

// Example function to be called in for loop
#if 0
class targetClass
   {
     void foo (int x);
   };
#else
void foo (int x);
#endif

int main ()
   {
     list l;
     list::iterator p;
     for (p = l.begin(); p != l.end(); p++)
        {
          foo(*p);
        }
   }












