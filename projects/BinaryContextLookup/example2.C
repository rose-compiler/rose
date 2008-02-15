#include <math.h>
#include <stdio.h>

double vector_norm(double x,double y,double z)
{
	return sqrt(x*x+z*z+y*y);
}

int main(int argc,char ** argv)
{
	double x=2,y=1,z=2;
	scanf("%lf %lf %lf",&x,&y,&z);
	printf("Norm of (%f,%f,%f) is %f\n",x,y,z,vector_norm(x,y,z));
	return 1;
	
}
