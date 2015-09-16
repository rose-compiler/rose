int main() {
	#pragma p1_reachable begin infeasible
	#pragma p2_reachable begin infeasible
	#pragma p3_reachable begin infeasible
	#pragma p4_unreachable begin infeasible
	int a[11];
	int b = 2;
	int c = 10;
	a[b] = c;
	if (a[b] == c) {
		#pragma p1_reachable end
		a[c] = b;
	}
	if (a[c] == b) {
		#pragma p2_reachable end
		a[1] = b;
		a[2] = c;
	}
	if (a[1] == b) {
		#pragma p3_reachable end
		a[2] = b;
	}
	if (a[2] == c) {
		#pragma p4_unreachable end
		a[2] = 1;
	}
	return 0;
}
