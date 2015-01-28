int main() {
	#pragma p1_reachable begin infeasible
	#pragma p2_unreachable begin infeasible
	int a[5];
	a[1] = 2;
	if (a[1] == 2) {
		#pragma p1_reachable end
		
		a[2] = 3;
	}
	else {
		#pragma p2_unreachable end
		a[2] = 4;
	}
	return 0;
}
