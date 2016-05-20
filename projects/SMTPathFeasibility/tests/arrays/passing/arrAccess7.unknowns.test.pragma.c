int main(int b, int c) {
	#pragma p1_reachable begin infeasible
        #pragma p2_reachable begin infeasible
        #pragma p3_unreachable begin infeasible
        #pragma p4_reachable begin infeasible
        #pragma p5_reachable begin infeasible
	int d = 3;
	int a[5];
	if (b > c) {
	#pragma p1_reachable end
	a[b-c] = d;
	d = a[b-c];
	}
	else {
	#pragma p5_reachable end
	d = a[c-b];
	a[c-b] = d;
	}
	if (b <= c) {
		if (d == a[c-b]) {
			d = 2;
			#pragma p2_reachable end
		}
		else if (a[c-b] == d) {
			d = 3;
			#pragma p3_unreachable end
		}
		else {
			d = 4;
			#pragma p4_unreachable end
		}
	}
	int q;
	q = d;
	d = q+d;
	return 0;
}
