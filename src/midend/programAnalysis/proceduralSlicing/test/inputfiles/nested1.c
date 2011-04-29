/*
This file is for testing nested control loops.
*/
int printf(const char *, ...);

int main(){
  int a,b,c,i,j;
  float x,y,z;
  bool istrue;
  
  int d,e,f;
  a=4;
  b=5;
  i=1;
  e=5; f=5;  // in case we don't reach the if-branch
  if(a<=b){
    c=6;
    d=a;
    istrue=false;
    for(j=1; j<=c; j++){
      a=5;
      while(i==a){
        b=c;
        c = j+4;
        if(j==c && istrue){
          f=c;
          e--;  
          break;
        }
        else{
          i++;
          b--;
          a++;
        }
      }
    }
    b-=a; 
  }
   else{
    b--;
    d=b;
     }
  
  return 0;
}
