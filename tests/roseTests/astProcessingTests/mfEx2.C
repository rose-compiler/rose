int bob(int z) {
    if (z == 0) {
    int k = 0;
    //return k;
    }
    else {
    //return 3;
     }
     return 0;
}

int fred(int y) {
    int x;
    x  = bob(y);
    return x;
}

int main() {
    int a = 2;
    int b = 3;
    //if (a == 2) {
    //   return a;
   // }
    
    //int x;
    //int y;
    //int x = bob(fred(a));
    //int y = 0;
    int xp = bob(a);
    int yp = fred(b);
    if (xp + yp == 2) {
       xp = a;
    }
   // int x = bob(fred(a));
   // int y = fred(bob(b));
    return xp + yp;
}

