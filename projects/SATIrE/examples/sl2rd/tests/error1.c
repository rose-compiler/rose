#include <stdlib.h>

#define STRLEN 14

int cma(char c)
{
	if (c == 'a') {
		return 1;
	} else {
		return 0;
	}
}

int cmb(char c)
{
	if (c == 'b') {
		return 1;
	} else {
		return 0;
	}
}


int cmc(char c)
{
	if (c == 'c') {
		return 1;
	} else {
		return 0;
	}
}

int cmd(char c)
{
	if (c == 'd') {
		return 1;
	} else {
		return 0;
	}
}

int cme(char c)
{
	if (c == 'e') {
		return 1;
	} else {
		return 0;
	}
}


int cmf(char c)
{
	if (c == 'f') {
		return 1;
	} else {
		return 0;
	}
}


int cmg(char c)
{
	if (c == 'g') {
		return 1;
	} else {
		return 0;
	}
}

int cmh(char c)
{
	if (c == 'h') {
		return 1;
	} else {
		return 0;
	}
}

int cmi(char c)
{
	if (c == 'i') {
		return 1;
	} else {
		return 0;
	}
}

int cmj(char c)
{
	if (c == 'j') {
		return 1;
	} else {
		return 0;
	}
}

int cmk(char c)
{
	if (c == 'k') {
		return 1;
	} else {
		return 0;
	}
}

int cml(char c)
{
	if (c == 'l') {
		return 1;
	} else {
		return 0;
	}
}

int cmm(char c)
{
	if (c == 'm') {
		return 1;
	} else {
		return 0;
	}
}

int cmn(char c)
{
	if (c == 'n') {
		return 1;
	} else {
		return 0;
	}
}

int cmo(char c)
{
    if (c == 'o') {
        return 1;
    } else {
        return 0;
    }
}

typedef int(*I_PF_C)(char);

typedef struct st {
	const I_PF_C f;
	int nu;
} ST;


#define MAX_EL 15
const ST arr[MAX_EL] = {
	{ cma, 0 },
	{ cmb, 1 },
	{ cmc, 2 },
	{ cmd, 3 },
	{ cme, 4 },
	{ cmf, 5 },
	{ cmg, 6 },
	{ cmh, 7 },
	{ cmi, 8 },
	{ cmj, 9 },
	{ cmk, 10 },
	{ cml, 11 },
	{ cmm, 12 },
	{ cmn, 13 },
	{ cmo, 14 },
};

int arr_cnt[MAX_EL]= { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } ;

const char str[STRLEN]="abcabcabcdefg";

int main()
{
    volatile int su=0;
    int i=0;
	int j=0;

	for (j=0; j<STRLEN; j++) { 
		for (i=0; i<MAX_EL; i++) {
			if ( arr[i].f != NULL ) { 
				arr_cnt[i] += (arr[i].f)(str[j]);
			}
		}
	}

	for (i=0; i<MAX_EL; i++) {
		su=su+arr_cnt[i];
	}

    return su;

}
