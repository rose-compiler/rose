#include <stdio.h>

int main() {
	int dividend = 4, divisor = 2;
	double result;
	
	result = ((double)dividend)/divisor;

	printf("result: %d\n", result);
	
	result = static_cast<double> (dividend)/divisor;

	printf("result: %d\n", result);


	return 0;
}
