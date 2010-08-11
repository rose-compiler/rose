template <class num>
class TPoint {
public:
  num x, y;
};

typedef TPoint<int> point;
typedef TPoint<double> fpoint;

void foo ( point const & pt) {}
void foo ( fpoint const & pt) {}


