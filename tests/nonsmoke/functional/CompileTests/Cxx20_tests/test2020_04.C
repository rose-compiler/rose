unsigned int i = 1;
auto r = -1 < i; // existing pitfall: returns ‘false’
auto r2 = -1 <=> i; // Error: narrowing conversion required

