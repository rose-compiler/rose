
class List 
{
  public: class List *next;
  int val;
}

;

int main()
{
  class List *x;
  class List *y;
  class List *s;
  x = (::new List );
  s = x;
  int i = 0;
  redo:
  y = (::new List );
  x -> next = y;
  switch(i){
    case 0:
{
      x -> val = 100;
      break; 
    }
// unreachable
    x -> val += 5;
// sjdhgfjsgdf
    break; 
    default:
{
      x -> val = i;
    }
  }
  x = y;
  if (i < 5) {
    goto redo;
  }
  else {
  }
  return 0;
}

