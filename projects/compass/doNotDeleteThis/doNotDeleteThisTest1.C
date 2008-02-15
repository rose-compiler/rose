class SomeClass {
    public:
        SomeClass(){};
        ~SomeClass(){};
        void doSomething(){};
        void destroy();
};

void SomeClass::destroy() {
    delete this;  // Dangerous!!
}

int main()
{
  SomeClass *sc = new SomeClass;
  sc->destroy();
  sc->doSomething();  // Undefined behavior

  SomeClass *sc2 = new SomeClass;

  delete sc2;

  return 0;
}
