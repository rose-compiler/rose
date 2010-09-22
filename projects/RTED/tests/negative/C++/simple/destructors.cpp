
class Test {
  Test() {
    p = new int;
  }

  ~Test() {
    delete p;
  }

  private:
    int* p;
};


void foo() {
}


int main() {
  Test t;
}
