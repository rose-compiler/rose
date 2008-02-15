int f( bool b )
{
  return 0;
}

bool g( char c )
{
  return true;
}

int h1( int )
{
  return 50;
}

int h2( bool )
{
  return 2;
}

int (*fp())(bool b)
{
  return &f;
}

int main()
{
  int i =  f( g('a') );
  f(true);

//i = f(g(h1(1) + h2(true)) || true) + h1( 2 );
  i = f(g(h1(1) + h2(true)) || true) + h1( 2 );

//return i = f ( g( h1( 1 ) ) ) + h1( h1( 2 ) ) + h1( 1 );
  return i = f ( g( h1( 1 ) ) ) + h1( h1( 2 ) ) + h1( 1 );

//for (i = f(g('a'));; );
  for (i = f(g('a'));; );

//for (i = f(g('a')); i < h1(h2(false)); h1(h1(42)+5));
  for (i = f(g('a')); i < h1(h2(false)); h1(h1(42)+5));

  if ( 1 )
    if (f(g('d')))
      i = 3;

  for ( f(g('v')), i = h1(h1(3));  i < 5; i++ )
  {
    if ( g( h1(h1(50)) + h2(f(true) > 10)) == false )
      {
	int (*ptrf)(bool);
	return i = fp()(true);
      }
  }
   
}
