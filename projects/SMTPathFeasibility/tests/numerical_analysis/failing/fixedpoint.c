// from http://www.sci.brooklyn.cuny.edu/~mate/nml/numanal.pdf
// pages 54-55

double funct(double);
double fixedpoint(double (*fnct)(double), double startingval, double xtol, int maxits, int *itcount, int *outcome);

int main() {

const double tol = 5e-10;
double root;
int its, success;
root = fixedpoint(&funct, 1.0, tol, 50, &its, &success);
if (success == 2) {
	printf("The root is %.12f\n",root);
	printf("%u iterations were used to find the root\n",its);
}
else if (success == 0) {
	printf("The maximum number of iterations has been reached\n");
}
}


