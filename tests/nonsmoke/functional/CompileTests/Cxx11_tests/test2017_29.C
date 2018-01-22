// ----------input ---------------------

#if 0
class baseWork{
  protected:
    double * m_nvol;
};
#endif

// with inheritance: unparser will generate: ()->m_nvol[0] ...
// without inheritance: unparser will generate: ->m_nvol[0] ...
class PdVWorkRaja // : public baseWork 
{

 public:
    double * m_nvol;

  void foo_input(double*, int);
};

void PdVWorkRaja::foo_input(double *a, int n)
{
  auto func =(0,n, [=] (int i)
      {
   // Error: this is unparsed as: a[i] = -> m_nvol[0];
      a[i] = m_nvol[0];
      });

  // func(0);
}
