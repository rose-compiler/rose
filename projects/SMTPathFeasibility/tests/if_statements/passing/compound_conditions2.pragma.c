int main() {
	#pragma p1_reachable begin infeasible
	int a = 0;
	int b = 1;
	int c = 2;
	if (a == 1 || b == 1 && c == 2) {
		#pragma p1_reachable end
	}
	return 0;
}
