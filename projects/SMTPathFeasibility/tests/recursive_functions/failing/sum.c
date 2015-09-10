int sum(int n) {
	if (n <= 0) {
		return 0;
	}
	else {
		return (n + sum(n-1));
	}
}

int main() {
	int n = 12;
	sum(n);
}
