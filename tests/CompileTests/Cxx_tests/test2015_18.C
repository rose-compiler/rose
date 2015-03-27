class MyClass {
public:
  static void myFunc();
private:
  int myMember;
  static int MyClass::* myPointer;
};

int MyClass::* MyClass::myPointer;

void
MyClass::myFunc() {
  myPointer = &MyClass::myMember;
}

int main(){ return 0;}
