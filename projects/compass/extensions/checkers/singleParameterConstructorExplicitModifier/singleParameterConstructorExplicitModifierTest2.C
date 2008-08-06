class Class
{
  private:
    int num;

  public:
    explicit Class( int n ){ num = n; }
    int getNum() const { return num; }
}; //class Class

class ClassX
{
  private:
    int num;

  public:
    ClassX( int n ){ num = n; }
    int getNum() const { return num; }
}; //class Class2

int main()
{
  Class c1(1);
  ClassX c2(2);

  return 0;
} //main()
