
int main()
{
  return 0;
}


class myClass 
{
  public: short x;
  int y;
  long z;
}

;
typedef struct __rose_generated_structure_tag_name_0 {
int a;
int b;
class ::myClass theClass;}myStruct;

int testExprParsing_0(int arg)
{
  return arg;
}


int testExprParsing_1()
{
  int i = 1;
  int j = 2;
  myStruct q;
  q.b = 1;
  q.a = ((q.b) + (((char )5)));
  i = (j + 1);
  q.theClass.::myClass::y = i;
  j = ((q.theClass.::myClass::z));
  q.theClass.::myClass::z = ((-98));
  q.a = ((q.b) - 5);
  i = (((j) - ((long )12)));
  q.b += 2;
  q.theClass.::myClass::x -= ((unsigned int )16);
  q.b += (-32);
  q.theClass.::myClass::x -= (((short )(-33)));
  if ((i - 5) <= (j + 1)) {
  }
  else {
  }
  if ((q.a) <= (j + 1)) {
  }
  else {
  }
  if (i <= ((q.a) - 1)) {
  }
  else {
  }
  if ((q.theClass.::myClass::z) <= (1)) {
  }
  else {
  }
  if (i <= (-1)) {
  }
  else {
  }
  if (1 <= (q.a)) {
  }
  else {
  }
  if ((-1) <= j) {
  }
  else {
  }
  if ((-761) <= (j + 220)) {
  }
  else {
  }
  if (((i + 761)) <= (q.theClass.::myClass::z)) {
  }
  else {
  }
  if ((i - 5) >= (j + 1)) {
  }
  else {
  }
  if ((q.a) >= (j + 1)) {
  }
  else {
  }
  if (i >= ((q.a) - 1)) {
  }
  else {
  }
  if ((q.theClass.::myClass::z) >= (1)) {
  }
  else {
  }
  if (i >= (-1)) {
  }
  else {
  }
  if (1 >= (q.a)) {
  }
  else {
  }
  if ((-1) >= j) {
  }
  else {
  }
  if ((-761) >= (j + 220)) {
  }
  else {
  }
  if (((i - 761)) >= (q.theClass.::myClass::z)) {
  }
  else {
  }
  if ((i - 5) < (j + 1)) {
  }
  else {
  }
  if ((q.a) < (j + 1)) {
  }
  else {
  }
  if (i < ((q.a) - 1)) {
  }
  else {
  }
  if ((q.theClass.::myClass::z) < (1)) {
  }
  else {
  }
  if (i < (-1)) {
  }
  else {
  }
  if (1 < (q.a)) {
  }
  else {
  }
  if ((-1) < j) {
  }
  else {
  }
  if ((-761) < (j + 220)) {
  }
  else {
  }
  if (((i - 761)) < (q.theClass.::myClass::z)) {
  }
  else {
  }
  if ((i - 5) > (j + 1)) {
  }
  else {
  }
  if ((q.a) > (j + 1)) {
  }
  else {
  }
  if (i > ((q.a) - 1)) {
  }
  else {
  }
  if ((q.theClass.::myClass::z) > (1)) {
  }
  else {
  }
  if (i > (-1)) {
  }
  else {
  }
  if (1 > (q.a)) {
  }
  else {
  }
  if ((-1) > j) {
  }
  else {
  }
  if ((-761) > (j + 220)) {
  }
  else {
  }
  if (((i - 761)) > (q.theClass.::myClass::z)) {
  }
  else {
  }
  i *= 5;
  i = (j / 5);
  2 + testExprParsing_0((i * 9));
  if ((i * 1) < j) {
  }
  else {
  }
  if (i == j) {
  }
  else {
  }
  if ((q.a = ((i <= j)))) {
  }
  else {
  }
  return i;
}


int foo()
{
  return 1;
}

int glob;

int &bar()
{
  return glob;
}


void insert_before(int i)
{
}


void insert_after(int i)
{
}


int testCFGTransform_0()
{
/*(a=b)*/
  int a = 0;
  int b = 9;
  int c = 1;
  int rose_temp__1 = (insert_before(0) , (a = (b + c)));
  (insert_before(1) , a = (insert_after(0) , rose_temp__1));
  insert_after(1);
  (insert_before(2) , b = c);
  insert_after(2);
  int rose_temp__2 = (insert_before(3) , (a = b));
  int &rose_temp__3 = (insert_before(4) , (bar() = (insert_after(3) , rose_temp__2)));
  if ((insert_after(4) , rose_temp__3)) {
  }
  else {
  }
  bool rose_temp__4;
  for ((insert_before(5) , a = 0), insert_after(5); ((rose_temp__4 = (insert_before(6) , a < 10) , insert_after(6)) , rose_temp__4); ((insert_before(11) , a++) , insert_after(11))) {
    bool rose_temp__5 = (insert_before(8) , (foo() < c));
    (insert_before(10) , b = ((int )(insert_after(8) , rose_temp__5)));
    insert_after(10);
  }
  (insert_before(7) , a = foo());
  insert_after(7);
  int rose_temp__6 = (insert_before(9) , b = a);
  return (insert_after(9) , rose_temp__6);
}

extern void testCallGraphTraversal_0();
extern void testCallGraphTraversal_1();
extern void testCallGraphTraversal_2();
extern void testCallGraphTraversal_3();
extern void testCallGraphTraversal_4();
extern void testCallGraphTraversal_5();
extern void testCallGraphTraversal_6();
extern void testCallGraphTraversal_7();
extern void testCallGraphTraversal_8();
extern void testCallGraphTraversal_9();
extern void testCallGraphTraversal_10();

void testCallGraphTraversal_0()
{
  testCallGraphTraversal_1();
  testCallGraphTraversal_2();
  testCallGraphTraversal_3();
}


void testCallGraphTraversal_1()
{
  testCallGraphTraversal_2();
}


void testCallGraphTraversal_2()
{
  testCallGraphTraversal_4();
  testCallGraphTraversal_5();
}


void testCallGraphTraversal_3()
{
  testCallGraphTraversal_6();
}


void testCallGraphTraversal_4()
{
  testCallGraphTraversal_7();
}


void testCallGraphTraversal_5()
{
  testCallGraphTraversal_3();
}


void testCallGraphTraversal_6()
{
  testCallGraphTraversal_4();
}


void testCallGraphTraversal_7()
{
  testCallGraphTraversal_9();
  testCallGraphTraversal_10();
}


void testCallGraphTraversal_10()
{
}

