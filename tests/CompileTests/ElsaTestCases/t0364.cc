// t0364.cc
// tricky problem with arg-dep lookup

// isolated from nsCSSFrameConstructor.cpp:7312:5: error: no viable candidate for function call


class nsIStyleContext {
public:
  virtual void myGetStyleData(int aSID);
};


template <class T>
inline void
myGetStyleData(nsIStyleContext* aStyleContext, const T** aStyleStruct)
{}

template <class T>
inline void
myGetStyleData(void * aFrame, const T** aStyleStruct)
{}

void foo()
{
  const int *vis;
  nsIStyleContext *ctx;

  // call site where no receiver is present
  myGetStyleData(ctx, &vis);
}


struct S {
  void bar();
};

void S::bar()
{
  const int *vis;
  nsIStyleContext *ctx;

  // call site with an irrelevant receiver
  myGetStyleData(ctx, &vis);
}




// EOF
