class Class
{
  int privateData;
  friend int foo( Class & c );

  public:
    Class(){ privateData=0; }
}; //class Class

int foo( Class & c )
{
  return c.privateData + 1;
} //foo( Class & c )
