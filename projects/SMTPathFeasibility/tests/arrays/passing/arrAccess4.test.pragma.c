int main() {
	#pragma p1_reachable begin infeasible
	#pragma p2_unreachable begin infeasible
	int a[5];
	int b = 3;
	int c = -1;
	a[b+c] = b;
	if (a[2] == 3) {
		#pragma p1_reachable end
		c = c + 1;
	}
	else {
		#pragma p2_unreachable end
		c = c - 1;
	}
	return 0;
}
