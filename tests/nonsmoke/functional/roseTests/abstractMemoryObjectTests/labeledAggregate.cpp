/*
 *  TODO &h1
 *   nested structures
 *   dot and arrow expressions
*/

struct house
{
  int bedrooms;
  int bathrooms;
  float area;
};

struct community
{
  int population;
  house onehouse; 
};

void foo() {
  struct community cc;
  struct house h1, h2;
  float total;
  total = h1.area + h2.area + cc.onehouse.area;
}
