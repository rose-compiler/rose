
int main()
{
  int k;
  k = 0;
// k is used in the same scope in which it is declared
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
{
// deeper scope, should be trimmed
        j = 0;
      }
    }
{
      int j;
{
{
          j = 0;
// initially built into scope tree, later trimmed. 
        }
      }
      j = 2;
//this should trigger trimming the previous path 
    }
  }
  return 0;
}
