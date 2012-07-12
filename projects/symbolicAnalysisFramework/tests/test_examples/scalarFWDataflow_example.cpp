#define NOOP a=x;

/*void func(int &var,int var2)
{
//var += var2;
  var++;
//var=1;
}

extern void bar(int &var);*/

int main(int argc,char **argv)
{
  /*int x = 1;
  int y;
  y=1;
  int z;
  int a;
  int b = 1;*/
  int c = 0;
  int z = 0;
  while(z < 100) {
        z += 5;
        c++;
  }
//bar(c);
//c=16;
        return z+c;
/*  func(b,c);
  while(x == 1){
    a = 2;
  }
  if (x <= 0) {
    a = x;
  }
  else {
    a = x;
  }
  if (x <= 5) {
    y = 1;
  }
  else {
    a = y;
  }
  y = (z*5+a);
  y++;*/
/*if(x<=10)
        {
                y=1;
        }
        else
        {
                z=2;    
        }*/
}

/*int main(int argc, char** argv)
{
        int x, y, z;
        z=5;
        x=z + 6;
        y = -4* x;
        while(z<10)
        {
                z = z+5;
        }
        if(x<10)
        {
                z=z+10;
        }
        else
        {
                y = y*2;
        }
}
*/
