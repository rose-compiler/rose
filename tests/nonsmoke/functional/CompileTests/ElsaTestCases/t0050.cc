// cc.in50
// this caused a segfault when the GLR core failed to dup()
// terminal svals yielded more than once directly from
// the LexerInterface

void f(double d);

int main()
{
  f(2.0);
}
