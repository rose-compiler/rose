// input.cpp

#include <stdio.h>

class Point
{ public:
  Point(int x, int y)
  : x(x), y(y)
  { }
  int x, y;
};

void PrintPoint(Point p)
{
   printf("(%d,%d)\n", p.x, p.y);
}

void Rectangle(Point p1, Point p2)
{
   printf("(%d,%d)x(%d,%d)\n",
	p1.x, p1.y, p2.x, p2.y);
}

int main(void)
{
  Point p(15,27);
  PrintPoint(p);			// works
  PrintPoint(Point(42,0));		// fails
  Rectangle(Point(1,2),Point(3,4));	// fails
  return 0;
}
