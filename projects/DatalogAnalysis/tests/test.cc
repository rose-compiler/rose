
//should be const
//no globals
double pi= 3.1415;

void checkGoto()
{
    int n= 10;
    for (int i= 0; i < n; i++) {
        for (int j= 0; j < n; j++) {
            for (int k=0; k < n; k++) {
                goto BAD_GOTO1;
                goto GOOD_GOTO1;
BAD_GOTO1:
            }
        }
    }
GOOD_GOTO1:

    if (0) {
        goto BAD_GOTO2;
    }
BAD_GOTO2:
    return;
}

void assignInConditional()
{
    int x;
    x=1;

    //assign in conditional!!
    if ( x = 1 ) {
        return;
    }

    if ( x==1 ) {
        return;
    }
}

void switchChecks()
{
    int i;

    switch (i) {
        case 0:
            //no break!!!
        case 1:
            if (0) {
                break;
            } else {
                //nobreak
            }
        case 2:
            break;
        case 3:
            i=0;
            //no break
        //nodefault
    }

    switch (i) {
        case 0:
            break;
            //as dan's point
            do {
        default:
            i=2;
            } while (0);
    }

    switch (i) {
        case 0:
            switch (i) {
                default:
                    i= 1;
            }
            //no break
        //no default
    }
}

struct FieldTest {
    int x;
};

void constCheck()
{
    int constable[10];
    int unconstable[10];
    int unc1;
    int unc2;
    int unc3;
    int unc4;
    int unc5;
    unc1= unc2;
    unc2++;
    unc3--;
    unc4= (int)'c';
    --unc5;


    struct FieldTest unc_a, unc_b, con_c;
    unc_a.x= unc_b.x;
    unc_b= con_c;

    unconstable[3] = constable[2];
}

class BadVirtDest1 {
    public:
        ~BadVirtDest1();

        virtual void f();
};

class OkVirtDest1 {
    public:
        virtual ~OkVirtDest1();
        void f();
        virtual void v();
};

class OkDestructor {
    public:
        ~OkDestructor();
};

class OkDestructor2 {
    public:
        void f();
        void g();
};

class OkDestructor3 {
    public:
        ~OkDestructor3();
        void f();
        void g();
};

class OkDestructor4 : public OkVirtDest1 {
};

class BadVirt2 {
public:
    ~BadVirt2();
};

class Ok : public BadVirt2 {
};

#define NULL ((void*)0)
void *myMalloc(int);

void nullDeref()
{
    int *badDeref1;
    int *badDeref2;
    int *goodDeref;
    int x;
    bool cond;


    goodDeref= (int*) myMalloc(sizeof(int));
    badDeref1= (int*) myMalloc(sizeof(int));
    badDeref2= badDeref1;

   
    x= *badDeref1;

    if ( cond ) {
        if ( badDeref2 == NULL) {
            return;
        }
    }

    if (goodDeref==NULL) {
        return;
    }
 
    x= *goodDeref;


    x= badDeref1[3];


    x= *badDeref2;
}

int mystrlen( /*should be const:*/ char *s )
{
    int i; //uninitialized!

    while (*s++)
        i++;

    return i;

    // dead code
    i++;
    return i;
}

int literalcheck()
{
    return mystrlen("hello");
}

int iEatPi()
{
    if (0) {
        return pi;
    } else {
        return pi;
    }

    //dead code:
    return pi;
}

void shadowingVars() {
    int i;
    i= 0;
    {
        //shadowed variable
        int i;

        for (i= 0; i < 100; i++) {
            //shadowed variable
            double pi= 0;
            pi++;
        }
    }
}

void x_isbadRef( int &x, int &y )
{
    x= 0;
    x= y+ 3;
}

int *ptrToLocal()
{
    int x;
    return &x;
}


int samate_srd(int argc, char *argv[])
{
    int init_value= 0;
    int loop_counter;
    char buf[10];

    /* buffer overflows */
    buf[loop_counter] = 'a';

    if ( loop_counter <= 17 ) {
        buf[loop_counter] = 'a';
    }

    for(loop_counter = init_value; loop_counter <= 17; loop_counter++)
    {
        buf[loop_counter] = 'A';
    }

    /* all not buffer overflows */
    for(loop_counter = init_value; loop_counter <= 5; loop_counter++)
    {
        buf[loop_counter] = 'A';
    }

    for(loop_counter = init_value; loop_counter <= 17; loop_counter++)
    {
        if ( loop_counter <= 5 ) {
            buf[loop_counter] = 'A';
        }
    }


    for(loop_counter = init_value; loop_counter <= 17; loop_counter++)
    {
        /* impressive, no? */
        if ( loop_counter > 5 ) break;

        buf[loop_counter] = 'A';
    }


    return 0;
}
