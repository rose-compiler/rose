// 
// // Error: rose_xxx_lib.cpp:36:47
// C is not a base of B
//
class A {
  public: 
    void foo_1 (bool b);
    void bafoo_2 (bool b);
    void bafoo_3 (bool b);
    void bafoo_4 (bool b);
    void bafoo_5 (bool b);
};

class B
{
  protected:
      A x; 
};

class C: public B {};

class F: public C
{
  public: 
    virtual void foobar();
};

void F::foobar()
{
  x.foo_1(true);

  // DQ (11/12/2020) This was an issue for the code segregation tool.
  // This is unparsed as (when the global symbol across files is cleared):
  // ((class ::B *)((class ::C *)this__ptr__)) -> C::x .  bafoo_2 (true);
  // ((class ::B *)((class ::C *)this__ptr__)) -> C::x .  bafoo_3 (true);
  // ((class ::B *)((class ::C *)this__ptr__)) -> C::x .  bafoo_4 (true);
  // ((class ::B *)((class ::C *)this__ptr__)) -> C::x .  bafoo_5 (true);
  // And when it is not cleared, it is unparsed as:
  // ((class B *)((class C *)this__ptr__)) -> x .  bafoo_2 (true);
  // ((class B *)((class C *)this__ptr__)) -> x .  bafoo_3 (true);
  // ((class B *)((class C *)this__ptr__)) -> x .  bafoo_4 (true);
  // ((class B *)((class C *)this__ptr__)) -> x .  bafoo_5 (true);

  x.bafoo_2(true);
  x.bafoo_3(true);
  x.bafoo_4(true);
  x.bafoo_5(true);

}
