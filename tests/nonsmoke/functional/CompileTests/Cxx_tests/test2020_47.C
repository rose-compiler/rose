// RC-71 (shorter version)

struct stream {};

struct iterator {
  // constexpr iterator();
  iterator();
  iterator(stream &);
};

struct string {
  template<typename _InputIterator>
  string(_InputIterator __beg, _InputIterator __end);
};

void bar(string&);void foo() 
   {
     stream astream;
  // *Without* the parenthesis, the type of res is:
  //     "string (*)(iterator, iterator (*)())"
  // string res( (iterator(astream)), iterator() );  bar(res);
     string res( (iterator(astream)), (iterator()) );

     bar(res);
   }

 
