class X
{
private:
  int i; 
 public:
  X (int in){ i =in; }
 
  friend X operator+(X lhs,        // passing lhs by value helps optimize chained a+b+c
                     const X& rhs) // otherwise, both parameters may be const references
  {
    lhs.i += rhs.i; // reuse compound assignment
    return lhs; // return the result by value (uses move constructor)
  }
};

void foo()
{
  X x1(1), x2(2);
  x1 = x1 + x2;
}
