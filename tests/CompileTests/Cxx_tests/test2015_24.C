template <class T>
class TGAHelmOp
{
   public:
     TGAHelmOp()
       :m_isTimeDependent(false)
     {
     }
     explicit TGAHelmOp(bool a_isTimeDependent)
       :m_isTimeDependent(a_isTimeDependent)
     {
     }
   private:
     bool m_isTimeDependent;
};

template <class T>
class LevelTGAHelmOp : public TGAHelmOp< T >
{
   public:
     LevelTGAHelmOp()
    // This is not correct C++ code:
    // : TGAHelmOp<T>::TGAHelmOp(false)
    // The correct C++ code should be
       : TGAHelmOp<T>(false)
     {
     }
};

class AMRPoissonOp : public LevelTGAHelmOp<int>
{
   public:
     AMRPoissonOp()
     {
     }
}; 

