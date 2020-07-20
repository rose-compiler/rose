new int(*[10])(); // error: parsed as (new int) (*[10]) ()
new (int (*[10])()); // okay: allocates an array of 10 pointers to functions

