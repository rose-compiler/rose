
foo1()
   {
     for (auto&& [first,second] : mymap) 
          { // use first and second
          }
   }

// (since C++17)
// Explanation
// The above syntax produces code equivalent to the following (__range, __begin and __end are for exposition only):


// This code examples must be put into functions to make sense.

{
    auto && __range = range_expression ;
    for (auto __begin = begin_expr, __end = end_expr; __begin != __end; ++__begin) {

        range_declaration = *__begin;
        loop_statement

    }

}
	(until C++17)

{

    auto && __range = range_expression ;
    auto __begin = begin_expr ;
    auto __end = end_expr ;
    for ( ; __begin != __end; ++__begin) {

        range_declaration = *__begin;
        loop_statement

    }

}
	(since C++17)
(until C++20)

{

    init-statement
    auto && __range = range_expression ;
    auto __begin = begin_expr ;
    auto __end = end_expr ;
    for ( ; __begin != __end; ++__begin) {

        range_declaration = *__begin;
        loop_statement

    }

} 
