
class X {};

class Y : public X {};

Y* dynamic_yPtr;
// X* dynamic_xPtr;

// This is a base class cast (will not be considered to be a dynamic cast.
X* xPtr3    = dynamic_cast<X*>(dynamic_yPtr);

X* xPtr4    = (X*)dynamic_yPtr;


