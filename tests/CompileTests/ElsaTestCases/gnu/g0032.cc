// g0032.cc
// using '$' in identifiers

struct A {
  int identifier_$_has_dollar;

  // this one conflicts with cqual's syntax...
  int $starts_with_dollar;
  
  int ends_with_dollar$;
  
  int two_$_dollars_$_here;
  
  int adjacent_$$_dollars;
};
