struct B0 {
  int v;
};

struct A0 : public B0 {};
struct A1 : public B0 {};

struct B1 : virtual public A0, virtual public A1 {};

struct A2 : public B1 {};
struct A3 : public B1 {};

struct X :  public A2, public A3 {};

void foo () {
  X x;
  int v_A2_B1_A0_B0 = x.A2::B1::A0::v;
  int v_A3_B1_A1_B0 = x.A3::B1::A1::v;
}

#if 0
bash-4.2$ g++ -c namequal-data-member.C
namequal-data-member.C: In function ‘void foo()’:
namequal-data-member.C:18:37: error: ‘A0’ is an ambiguous base of ‘X’
   int v_A2_B1_A0_B0 = x.A2::B1::A0::v;
                                     ^
namequal-data-member.C:19:37: error: ‘A1’ is an ambiguous base of ‘X’
   int v_A3_B1_A1_B0 = x.A3::B1::A1::v;
                                     ^
#endif

