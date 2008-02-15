// t0313.cc
// convert array to bool (always converts as true...)

// this comes from mozilla stopwatch.i, where it is actually
// a bug (programmer said assert("...") instead of assert(!"..."))

void b(bool);

int f()
{
  b("a");

  // same with function..
  b(f);

  "a"? 1 : 2;

  char buf[20];
  return buf? 1 : 2;
}

