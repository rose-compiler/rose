int main()
{
  enum FOO
  {
    ROSE  = 0,
    IS   = 1,
    COOL = 2
  };
  FOO x;
  x=ROSE;
  if(x!=ROSE) {
    x=IS;
  } else {
    x=COOL;
  }
  return 0;
}
