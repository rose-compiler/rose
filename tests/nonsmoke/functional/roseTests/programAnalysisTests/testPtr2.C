
struct List { int val; List* next; };

void buildList( List*& head, List*& end)
{
  head = end = 0;
}

void appendList(List*& head, List*& end, int val)
{
  if (end == 0)  {
     head = end = new List(); //(List*)malloc(sizeof(List)); 
  }
  else {
     end->next = new List(); //(List*)malloc(sizeof(List));
     end = end->next;
  }
  end->val = val;
  end->next = 0;
}

List* lookupList(List* head, int val)
{
  List* p = head; 
  for ( ; p != 0; p = p->next) {
     if (p->val == val)
       break;
  }
  return p;
}

int main()
{
  List* head, *end;
  buildList(head,end);
  for (int i = 0; i < 10; ++i) {
     appendList(head,end,2 * i); 
  }
  List* p = lookupList(head, 6);
}
