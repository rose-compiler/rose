class A
{
  public:
    A();     
  private:
    enum Ba_enum
    {
      AB_ONE
    };
};

A::A()
{
#pragma rose_outline  
  AB_ONE;
}

