int go(int x) {
    int z = x;
    if (x > 2) {
       x = x + 1;//     return 0;
    }
    else {
       z = z + 1;
    }
    return z;
}

int far(int x) {
   int y = x;
   int z = go(x);
   int ret;
   if (z == x) {
   ret = x;
   }
   else {
   ret = z;
   }
   return ret;
}

int fargo(int x, int y) {
    int z = far(x);
    int k = go(y);
    return k + z;
}
int main() {
    //int x,y,q;
    int x = 2;
    int y = 3;
    int q1 = go(x);
    int q2 = far(x);
    int q = fargo(q2, q1);//,go(y));//,go(y));
    return q;
}
