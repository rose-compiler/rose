#include<stdio.h>
struct astruct {
	int b;
	int a;
};

struct anested {
	struct astruct an;
	int b;
	int a;
};
int main() {
	int i;
	struct astruct as;
	as.b = 0;
	as.a = 0;
	struct anested anest;
	anest.an = as;
	unsigned int ui;
	signed int si;
	short s;
	signed short ss;
	unsigned short us;
	long l;
	unsigned long ul;
	signed long sl;
	long long ll;
	signed long long sll;
	unsigned long long ull;
	int aia[10];
	int* aip =(int*) malloc((sizeof(int))*10);
	//int* magicarr[][10]; 
	int (*magicarr)[10] = malloc(20*sizeof *magicarr);
	magicarr[2][2] = 0;	
	i = 0;
	ui = 0;
	si = 0;
	
	s = 0;
	ss = 0;
	us = 0;
	
	l = 0;
	ul = 0;
	sl = 0;
	ll = 0;
	sll = 0;
	ull = 0;
	
	aia[0] = 0;
	aip[0] = 1;
	char c;
	unsigned char uc;
	signed char sc;
	c = 0;
	uc = 0;	
	double d;
	long double ld;
	float f;
	int* p = i;
	int** pp = p;
	int*** ppp = pp;	
	d = 0.0;
	ld = 0.0;
	f = 0.0;
	int aiaia[10][10];
	aiaia[0][0] = 1;

		
			
	return 0;
}
