// cc.in35
// problem with recognizing that different instantiations
// of a template class are different types

template <class num>
class TPoint {
public:
  num x, y;
};



class stringBuilder;
typedef TPoint<int> point;
typedef TPoint<double> fpoint;



stringBuilder& operator<< (stringBuilder &sb, point const &pt)
{   
  // would require adding operators for stringBuilder, but
  // that is not what this file is intended to test
  //return sb << "(" << pt.x << ", " << pt.y << ")";
  return sb;
}

stringBuilder& operator<< (stringBuilder &sb, fpoint const &pt)
{
  //return sb << "(" << pt.x << ", " << pt.y << ")";
  return sb;
}


 
 
 
