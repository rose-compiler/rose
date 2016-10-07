// t0129.cc
// exercise the C comment pattern

// strategy: intersperse the comments with numbers to add,
// and then test at the end to see if we saw them all

enum { x =

/**/        1+
/***/       2+
/****/      3+
/*****/     4+
/******/    5+
            6+
/*
*/
            7+
/**
*/
            8+
/*
**/
            9+
/**
**/
            10+

//ERROR(1): 100+        // would make the wrong size

0 };


// the constant-evaluator knows how to deal with ?:, and the array
// type checker knows that sizes must be non-negative, so if we miss
// one then the size will be -1 and we'll get an error
int arr[x==55? 1 : -1];
