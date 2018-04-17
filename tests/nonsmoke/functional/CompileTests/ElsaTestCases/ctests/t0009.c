// t0009.c
// oink/test2-cc/voidconditional.c

// sm: I do not believe this is valid C99, but gcc accepts it ...

void driv(void)
{
  1 ? 1 : driv();
}


void main(void)
{
  driv();
}

