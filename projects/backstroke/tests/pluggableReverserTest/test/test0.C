
struct model
{
    int i;
    int j;
    int k;
    //static int k;
};

struct A
{
    int i;
};

#if 0
namespace my
{
    int x;
}

int g;
#endif


#if 0

void event0(model* m)
{
    //int a = 0;
    //int t = m->i;
    //m->i = m->j;
    //m->j = t;
    //
#if 1
    if (m->i)
        m->j += m->i;
    else
        m->j -= m->i;
#endif
#if 1
    //++m->i;
    if (m->i = 1)
        m->i = 0;
        //m->j += m->i;
        //m->i = 0;

    ++m->i;
#endif
#if 0
    {
    }
#endif
#if 0
    m->i = 1;
    m->i = 1;
    m->i = 1;
    m->i = 1;
    m->i = 1;
    m->i = 1;
#endif
#if 0
    m->i += 1;
    m->i += 1;
    m->i += 1;
#endif
    //m->i = -m->i + t;
    //int q,w,e,r,t,y;
    //g = 0;
    //model::k = 0;
    //my::x = 0;
    //++a;
    //m->i += a;
    //m->i = 0;
}

void event1(model* m)
{
    if (m->i)
        m->i = 10;
    else
        m->i = 0;
}

void event2(model* m)
{
    int i = 10;
    m->i += i;
    m-> j += i;
}

#endif

#if 0
void event3(model* m)
{
    int t = m->i;
    m->i = m->j;
    m->j = t;
#if 0
    int i = 0;
    int j = i;
    int k = j;
#endif
}


void event4(model* m)
{
    //int q,w,e,r,t,y,u,i,o,p,a,s,d,f,g;
    m->i = 10;
    if (m->j)
        m->i += 10;
    if (m->k)
        m->i = 10;
    m->i = 10;
}
#endif


void event5(model* m)
{
#if 0
    A p, q;
    q.i = 0;
    q = p;
    
    int a;// = m->i;
    //m->j += a;
    //a = 10;
    m->i = 1;
#endif
    //m->i = 1;
    while(1)
    {
        m->i = 1;
#if 0
        if (1)
        {
            if (1)
                a = 0;

        }
        else
        {
            if (1) 
                ;//a = 0;
        }

        //++m->i;
#endif
    }

    
}

#define NEW_BLOCK 0

void event6()
{
    
     int x = 1;
     int z = 1;
     switch(x)
        {
          case 1:
#if NEW_BLOCK
             { 
#endif
               int y = 47;
               y++; 
               ++x;
#if NEW_BLOCK
             }
#endif
        }
}

void event7(model* m)
{
   A a, b;
   if (1)
   {
     a.i = 0;
     a = b;
   }
   //A b = a;
   //a = b;
   //a.i = b.i;
}
