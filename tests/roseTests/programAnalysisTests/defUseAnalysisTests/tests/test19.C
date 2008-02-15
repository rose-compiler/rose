int global;
int global2=3;
void setMe()
{
  global=1;
  global2=3;
}

main()
{
  int a;
  setMe();
  a=global;
  global2=0;
  
}
