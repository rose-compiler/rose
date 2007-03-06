class List {
public:
  List* next;
};
List *a,*b,*ap,*bp, *y;

int main() {           

  int i,j,n;     
  n=5;
  a=new List();
  b=new List();
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
