void print(int);

void f()
{
  int i; //i should be declared at the for loop.
  int sum = 0; //sum is OK.

  //i is only used in the for scope.
  for (i = 0; i < 10; ++i) {
    //sum is used right after the block of declaration it belongs.
    sum += i;
  }

  //sum is used in the scope of definition.
  print(sum);
}
