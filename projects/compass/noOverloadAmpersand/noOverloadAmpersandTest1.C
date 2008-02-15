
//Your test file code goes here.


class a
{
  const a& operator=(a* incoming);
  const a& operator&(a* incoming);
  
};


int main(int argc, char** argv)
{
  a* input = new a();
  a* b;
  *b = *input;
  return 0;
}
