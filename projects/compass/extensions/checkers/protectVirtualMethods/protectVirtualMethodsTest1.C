class Class 
{
  int n;

  public:
    Class() { n = publicVirtualFunction(); } //constructor
    ~Class() {} //Destructor

    virtual int publicVirtualFunction() { return 1; }
}; //class Class 

int main()
{
  Class c;
  return 0;
} //main()
