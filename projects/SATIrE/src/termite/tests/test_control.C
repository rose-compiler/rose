int argl(int);
int argl(int i) {
	return i;
}
extern int somemore();
extern int xyza;
static int axyz;
int main() {
	int i = 1 + 1 + 1 + 1;
	enum xyz {XX,YY,ZZ};
	int (*funcpt)(int);
	funcpt = &argl;
	int* fff = &i;
	i = 1+1;
	bool j = true;
	float f = 3.0;
	f = f;
	j = i < 2;
	switch (i) {
		case 1: i++;
		case 2: i++;
		case 3: i++;
		case 4: break;
		case 5: break;
		default: i = 4;
	}
	if(i < 3) {
		j = j;
	} else if (i == 3) {
		i = i;
	}
	int arr[] = {1,2,3};
	for(int foo = 0; foo < 3; foo++) {
		i = foo;
		foo++;
	}
	while(i > 0) {
		i--;
	}
	do {
		i++;
	}while(i < 5);
	j = j;
	i = 0;
	int sizeint = (int) sizeof(j);
	return argl(i);
}
