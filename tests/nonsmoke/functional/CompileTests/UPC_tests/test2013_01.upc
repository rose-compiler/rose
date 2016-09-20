//--------------- input ---------------------
 // [liao6@tux322:/export/tmp.liao6/workspace/edg-40-test/sourcetree/projects/UpcTranslation/tests]cat barrier.upc
int a=100;
int main()
{
  upc_barrier ;
  upc_barrier 10;
  upc_barrier a+10;
//upc_barrier 0.9; //illegal case
  return 0;
}
