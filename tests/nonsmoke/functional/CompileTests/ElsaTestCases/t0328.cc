// t0328.cc
// expression with lots of ambiguities

typedef int x;

void foo()
{
  int y;

  0 +
    // this outermost expression has 7 ambiguous alternatives, due to
    // the interaction of cast-vs-funcall and multiplication-vs-deref;
    // adding more "*(x)(y)" would add linearly to the # of
    // ambiguities at the outer level
    (x)(y)*(x)(y)*(x)(y)*(x)(y)*(x)(y)*(x)(y)*(x)(y);
}
