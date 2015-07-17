int dostuff(int a, int b) {
	int c = a + b;
	c = c + 1;
	return c;
}

float dootherstuff(float a, float b, float c, float d) {
	float e = a * b + c / d;
	if (e > 5.0) {
		e = e * 2;
	}
	return e;
}

int main() {
	dostuff(1,2);
	dootherstuff(1.0,2.0,3.0,4.0);
	return 0;
}
