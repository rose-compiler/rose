int main() {	
	#pragma p1_reachable begin infeasible
	#pragma p2_unreachable begin infeasible
	#pragma p3_reachable begin infeasible
	#pragma p4_unreachable begin infeasible
	int a[5];
	int b = 2;
	int c = 1;
	int t;
	a[b-c] = 2;
	int abmc = a[b-c];
	if (a[b-1] == a[b-c]) {
		if (a[2-1] == a[b-1] && a[2-1] == a[2-c]) {
		#pragma p1_reachable end
		t = 0;
		}
		else {
		#pragma p5 unreachable end
		}
	
	

	a[b-c] = b;
	b = 3;
	}
	if (b == 3) {
	a[b-c] = 4;
	a[b-1] = 7;
	#pragma p3_reachable end
	}
	else {
	#pragma p2_unreachable end
	a[b-c] = abmc;
	}

	int d;
	if (a[b-c] == abmc) {
		#pragma p4_unreachable end
		d = 2;
	}
	d = a[b-c];
	a[b] = d;
	return 0;
}
