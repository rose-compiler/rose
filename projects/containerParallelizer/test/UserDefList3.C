void foo ( int a );

// Used to identify the ends of the list 
// (dummy list elements representing the front and back of the list)
#define TERMINAL_VALUE 1000

class list
   {
     public:
          typedef int elementType;

     protected:
          typedef elementType* pointer;
          typedef elementType& reference;
     struct list_node;
          typedef list_node* link_type;
          typedef unsigned int size_type;

     public:
          class iterator
             {
               friend class list;
               protected:
                    link_type node;
                    iterator(link_type x) : node(x) {}

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
     void push_back(const reference x);
     elementType& operator[](int index);
   };

void foo ( int a ) {
  //printf ("a = %d \n",a);
  ;
}

int main() {
  list l;
  for (list::iterator i = l.begin(); i != l.end(); i++) {
    foo(*i);
  }
  return 0;
}









