 
void main(int argc,char** argv)
{
    float x1,x2,x3;
    float fx1,fx2,fx3;
    int count = 0;
    int iter;
    fx3 = -1.0;
    x1 = atof(argv[1]);
    x2 = atof(argv[2]);
    iter = atoi(argv[3]);
    float diff;
    if (x1 > x2) {
	diff = x1-x2;
    }
    else {
	diff = x2-x1;
    }
//  int fx1 = (x1^3) + (x1^2) - (3x1) - 3;
//  int fx2 = (x2^3) + (x2^2) - (3x2) - 3;
    #pragma smt init [x1=1.0,x2=3.0,x3=0.0,iter=1000]
    #pragma smt [count=1000]
    while ( diff > 0.000001f || fx3 == 0.0f)
    {
        if(count == iter)
        {
            break;
        }
 	fx1 = x1*x1*x1 + x1*x1 + 3.0f*x1 - 3.0f;
	fx2 = x2*x2*x2 + x2*x2 + 3.0f*x2 - 3.0f;
	 	
        x3 = (x1 + x2)/2.0f;
 	fx3 = x3*x3*x3 + x3*x3 + 3.0f*x3 - 3.0f;
//        cout <<"x1=" << x1 <<" | x2="<< x2 <<" | x3=" << x3 <<" | " << "  f(x1)=" << fx1 << " |  f(x2)=" << fx2 << " |  f(x3)=" << fx3 << endl << endl;
 
        //float temp1 = f(x1);
        //float temp2 = f(x3);
        if( fx1 * fx3 < 0.0f )
        {
            x2 = x3;
        }
        else
        {
            x1 = x3;
        }
	if (x1 > x2) {
		diff = x1-x2;
	}
	else {
		diff = x2-x1;
	}
        count++;
    }
 
}
