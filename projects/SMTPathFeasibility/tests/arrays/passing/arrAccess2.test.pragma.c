int main() {
	#pragma p1_reachable begin infeasible
	#pragma p2_unreachable begin infeasible
	#pragma p3_reachable begin infeasible
	#pragma p4_unreachable begin infeasible
	int a[5];
	int b = 2;
	a[b] = 2;
	if (a[b] == 2) {
		#pragma p1_reachable end
		a[b] = 1;
	}
	else {
		#pragma p2_unreachable end
		a[b] = 1;
	}
	int c;
	if (a[b] == 1) {
		#pragma p3_reachable end
		c = 0;
	}
	else {
		#pragma p4_unreachable end
		c = 1;
	}
	return 0;
}
