int main()
{
    int a,b,c;
    int *p,*q;
    a=10;
    p=&a;
    #pragma rose a: Aliases:{ }{}\
    b: Aliases:{ }{}\
    c: Aliases:{ }{}\
    p: Aliases:{ a }{}\
    q: Aliases:{ }{}\
    x: Aliases:{ }{}

    if(1)
    {
      p = &b;
      #pragma rose a: Aliases:{ }{}\
        b: Aliases:{ }{}\
        c: Aliases:{ }{}\
        p: Aliases:{ b }{}\
        q: Aliases:{ }{}\
        x: Aliases:{ }{}
    }
    else
      { 
        int *x;
        x = &c;
        #pragma rose a: Aliases:{ }{}\
        b: Aliases:{ }{}\
        c: Aliases:{ }{}\
        p: Aliases:{ a }{}\
        q: Aliases:{ }{}\
        x: Aliases:{ c }{}
      }

    q = p;
    #pragma rose a: Aliases:{ }{}\
    b: Aliases:{ }{}\
    c: Aliases:{ }{}\
    p: Aliases:{ a b }{}\
    q: Aliases:{ a b }{}\
    x: Aliases:{ c }{}
    
    return 0;
}
