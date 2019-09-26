// This is a test code from Tristan demonstrating a name qualification bug in MFEM
// which is a dependency of Ardra (I think).

class Element;

template <class T>
class Array;

template <class T>
void bar(Array<T> &);

template <class T>
class Array {
  friend void bar<T>(Array<T> &); // << NEEDED
};

class Mesh {
  protected:
    Array<Element *> elements;
    void bar();

  public:
    void foo();
};

void Mesh::foo() {
  ::bar(elements); // << get unparsed without '::' which refers to ::Mesh::bar
}

