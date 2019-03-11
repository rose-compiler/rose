// Generalizing the Range-Based For Loop

int* range_expression;
int* begin_expr;
int* end_expr;

int range_declaration;

void foobar()
   {
     auto && __range = range_expression ; 
     auto __begin = begin_expr ;
     auto __end = end_expr ;
     for ( ; __begin != __end; ++__begin)
        {
          range_declaration = *__begin; 
       // loop_statement 
        } 
   }
