
int main()
{
  int k;
// k is used in the same scope in which it is declared
  k = 0;
// simplest use-def chain
{
{
{
        int i;
        i = 0;
      }
    }
  }
{
// use with multiple defs
{
      int j;
      j = 0;
// deeper scope, should be trimmed
{
        j = 0;
      }
    }
{
      int j;
{
{
// initially built into scope tree, later trimmed. 
          j = 0;
        }
      }
//this should trigger trimming the previous path 
      j = 2;
    }
  }
  return 0;
}
