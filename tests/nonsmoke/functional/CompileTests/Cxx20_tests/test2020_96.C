template<class...> struct Tuple { };
template<          class... Types> void g(Tuple<Types ...>);        // #1
template<class T1, class... Types> void g(Tuple<T1, Types ...>);    // #2
template<class T1, class... Types> void g(Tuple<T1, Types& ...>);   // #3
 
g(Tuple<>());                     // calls #1
g(Tuple<int, float>());           // calls #2
g(Tuple<int, float&>());          // calls #3
g(Tuple<int>());                  // calls #3

