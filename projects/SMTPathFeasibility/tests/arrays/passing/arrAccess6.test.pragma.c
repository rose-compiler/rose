int main() {
	#pragma p1_unreachable begin infeasible
#pragma p2_reachable begin infeasible
#pragma p3_reachable begin infeasible
#pragma p4_unreachable begin infeasible
#pragma p5_reachable begin infeasible
#pragma p6_unreachable begin infeasible
#pragma p7_unreachable begin infeasible
#pragma p8_unreachable begin infeasible
	int a[5];
	int b = 2;
	a[1] = b;
	if (a[1] != b) {
		b = 2;
		#pragma p1_unreachable end
	}
	else {
		#pragma p2_reachable end
		b = 3;
	}
	if (a[1] != b) {
		#pragma p3_reachable end
		b = a[1];
	}
	else {
		#pragma p4_unreachable end
		b = 5;
	}
	int d;
	d = a[1];
	a[2] = d;
	if (a[1] == d && a[1] == b && d == b && a[2] == b) {
		if (a[1] == d) {
			if (a[1] == b) {
				if (d == b) {
					if (a[2] == b) {
						#pragma p5_reachable end
						d = 2;
					}
					else if (a[2] == b) {
						#pragma p6_unreachable end
					}
					else {
						#pragma p7_unreachable end
					}
				}
			}
			
		}
		else {
			#pragma p8_unreachable end
		}
		d = 3;
	
	}		
	return 0;
}
