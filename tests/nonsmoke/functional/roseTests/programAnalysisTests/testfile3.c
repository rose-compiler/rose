int main(){

 float a;
 a=5;
 float b = 6;
 float c = 5;
 int i,j;
 i=1;
 j=0;

 b=3;
 float x=0;

 for(j=i; j<a; j++){
   b--;     c--;
   int k;
   for(k=2; k<j; k++){
#pragma start
     x+=x*i;
#pragma end
   }
 }

 return 0;
} 
