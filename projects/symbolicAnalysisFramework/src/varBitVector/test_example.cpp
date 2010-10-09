typedef struct {
	public:
	int field1;
	double field2;
	char field3;
} bar;

void foo()
{
	int a=0,b=0,c=0,d=0,e=0,f=0,g=0,h=0;
	double i=0.0,j=0.0,k=0.0,l=0.0,m=0.0,n=0.0;
	char q=0,r=0,s=0,t=0,u=0,v=0,w=0;
	bar x,y,z;
	
	x.field1=a*b*c*d*e*f*g*h;
	y.field1=0;
	z.field1=1;
	y.field2=i*j*k*l*m*n;
	x.field2=0.0;
	z.field2=1.0;
	z.field3=q*r*s*t*u*v*w;
	x.field3=0;
	y.field3=1;
	
	a=x.field1 + y.field1 + z.field1;
}
