class Exception{};

int main()
{
  try{
    throw Exception();
  }
  catch( Exception e ){}

  return 0;
}
