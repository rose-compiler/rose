class Class1 {};

template <typename T_Type1>
int operator<<(
  Class1 parm1,
  int    parm2);

template <typename T_Type2>
class Class2 {
  friend 
  int operator<<<T_Type2>(
    Class1 parm1,
    int    parm2);
};
