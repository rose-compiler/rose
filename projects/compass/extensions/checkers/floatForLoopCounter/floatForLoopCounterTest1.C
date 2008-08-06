int main()
{
  //This should be detected as not conforming
  for (float count = 0.1f; count <= 1; count += 0.1f) 
  {
  }

  for( double d = 0.0; d <= 1; d += 0.1 );

  //This is ok
  for( int i = 0; i<10; i++ );
  return 0;
}
