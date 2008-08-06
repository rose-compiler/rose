class Class 
{
  int n;

  public:
    Class(){ n = 1; }//Classy(); } //constructor
    ~Class() {} //Destructor

    virtual int Classy() { return 1; } 
}; //class Class 

int main()
{
  Class c;
  return 0;
} //main()
