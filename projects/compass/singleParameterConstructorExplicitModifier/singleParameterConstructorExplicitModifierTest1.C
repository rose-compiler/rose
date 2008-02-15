class Class
{
  int num;
  public:
    Class( int n ){ num = n; }
    int getNum() const { return num; }
}; //class Class

int main()
{
  Class c1(1);

  return 0;
} //main()
