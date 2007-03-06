class List {
public:
  List* next;
};
List *a,*b,*ap,*bp, *y, *cp, *ct1, *ct2;

int main() {           

  int i,j,n;     
  n=5;
  a=new List();
  b=new List();

  // create cycle
  cp=new List();
  ct1=new List();
  ct2=new List();
  cp->next=ct1;
  ct1->next=ct2;
  ct2->next=ct1;
  ct1=0;
  ct2=0;

  ap=a;
  bp=b;
  i=n;             
  j=n;
  while (i>0) {    
    List* t1=new List();
    ap->next=t1;
    t1=0;
    t1=ap->next;
    ap=t1;
    t1=0;
    i=i-1;         
    j=i;           
    while (j>0) {  
      List* t2=new List();
      bp->next=t2;
      t2=0;
      t2=bp->next;
      bp=t2;
      t2=0;
      j=j-1;
    }              
  }           
  ap->next=b; // conc lists
  y=a; 
  //write(length(y)-2)         
  return 0;
}                  
