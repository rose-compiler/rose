// test aliasing analysis
//Aliasing through direct assignment
void foo ()
{
  double * p1, *p2, *p3; 
  p1 =p2;
  p3; // reference p3 so the test translator can visit it 
}

