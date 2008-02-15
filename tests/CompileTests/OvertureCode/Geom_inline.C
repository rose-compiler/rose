inline 
//realArray
int
get_circle_center(realArray const &p1, realArray const &p2, realArray const &p3, realArray &center) {

  // get the center of a circle given three points, coding adapted
  // from a Mathematica equation to C dump.

//   cout<<"geom_inline"<<endl;
//   p1.display();
//   p2.display();
//   p3.display();
//   cout<<"geom_inline end"<<endl;

  //realArray c(Range(0,0),Range(0, p1.getLength(1)-1));
  /*
  realArray  p1pow = pow(p1,2);
  realArray  p2pow = pow(p2,2);
  realArray  p3pow = pow(p3,2);

  p1pow.reshape(Range(p1.getBase(1), p1.getBound(1)));
  p2pow.reshape(Range(p2.getBase(1), p2.getBound(1)));
  p3pow.reshape(Range(p3.getBase(1), p3.getBound(1)));
  */
  real x1 = p1(p1.getBase(0),0); 
  real y1 = p1(p1.getBase(0),1); 
  real x2 = p2(p2.getBase(0),0); 
  real y2 = p2(p2.getBase(0),1); 
  real x3 = p3(p3.getBase(0),0); 
  real y3 = p3(p3.getBase(0),1); 

  real denom = 2.0*(x3*y1-x2*y1+x1*y2-x3*y2-x1*y3+x2*y3);

  //cout <<"denom is "<<denom<<" "<<" "<<2.0*(x3*y1-x2*y1+x1*y2-x3*y2-x1*y3+x2*y3)<<" "<<2.*((x3-x2)*(y3-y1) - (x3-x1)*(y3-y2))<<endl;
  real x;
  real y;
  //if (fabs(denom)>0.0) {
  if (fabs(denom)>10.*REAL_EPSILON) {
    x = (pow(x3,2)*(y1-y2)+pow(x2,2)*(y3-y1)+(y2-y3)*(pow(x1,2)+pow(y1,2)-y1*y2-y1*y3+y2*y3))/denom;

    y =-((x1-x3)*(pow(x1,2)-pow(x2,2)+pow(y1,2)-pow(y2,2))-(x1-x2)*(pow(x1,2)-pow(x3,2)+pow(y1,2)-pow(y3,2)))/denom;
  } else {
    //p1.display("p1");
    //p2.display("p2"); 
    //p3.display("p3"); 
    return -1;
  }

  center(center.getBase(0),0) = x;
  center(center.getBase(0),1) = y;
  
  //cout <<" x = "<<x<<"  y = "<<y<<" denom =  "<<denom<<endl;
  //center.display("circle center");
  //return c;
  return 0;
}

inline 
double
triangleArea2D(const realArray &p1, const realArray &p2, const realArray &p3)
{
  
  return 0.5*((p2(0)-p1(0))*(p3(1) - p1(1)) - (p3(0)-p1(0))*(p2(1)-p1(1)));

}

inline
bool
isBetween2D(const realArray &p1, const realArray p2, const realArray &p3)
{
  // is p3 between p1 and p2 ?
  if (p1(0)!=p3(0)) 
    return ((p1(0)<=p3(0) && p3(0)<=p2(0)) ||
	    (p2(0)<=p3(0) && p3(0)<=p1(0)));
  else
    return ((p1(1)<=p3(1) && p3(1)<=p2(1)) ||
	    (p2(1)<=p3(1) && p3(1)<=p1(1)));
}

inline
bool
isBetweenOpenInterval2D(const realArray &p1, const realArray p2, const realArray &p3)
{
  // is p3 between p1 and p2 ?
  if ( fabs(p1(0)-p3(0))>10.*REAL_EPSILON ) 
    return ((p1(0)<p3(0) && p3(0)<p2(0)) ||
	    (p2(0)<p3(0) && p3(0)<p1(0)));
  else
    return ((p1(1)<p3(1) && p3(1)<p2(1)) ||
	    (p2(1)<p3(1) && p3(1)<p1(1)));
}

inline
bool 
angleLessThan(const realArray &a, const realArray &b, real angle)
{

  return angle>acos(sum(a*b)/(sqrt(sum(a*a)*sum(b*b))));
    
}

inline
bool 
angleGreaterThan(const realArray &a, const realArray &b, real angle)
{

  return (angle<acos(sum(a*b)/(sqrt(sum(a*a)*sum(b*b)))));
    
}
