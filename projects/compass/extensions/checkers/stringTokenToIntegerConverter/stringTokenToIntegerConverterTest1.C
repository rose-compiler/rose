
//Your test file code goes here.

int atoi(const char *str);
long int atol(const char* str);
long long int atoll(const char* str);
int sscanf(const char *restrict s, const char *restrict format, ... );

int main(int argc, char* argv[])
{
	int si;
	long int sli;
	long long int slli;

	if (argc > 3) {
		si = atoi(argv[1]);
		sscanf(argv[1], "%d", &si);
		sli = atol(argv[2]);
		slli = atoll(argv[3]);
	}
}
