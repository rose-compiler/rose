//#include <iostream>

using namespace std;

class Thomas {
public:
  int x;
};

int main(int argc, char** argv) {

  Thomas* thomas = new Thomas();
  thomas->x=5;

  Thomas tom;
  tom.x=6;
}
