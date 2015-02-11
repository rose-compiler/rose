int main(int a, int b) {
	#pragma p1_unreachable begin infeasible
	#pragma p2_unreachable begin infeasible
	#pragma p3_reachable begin infeasible
	if (a + b > 2) {
		if (a < 0) {
			if (b < 0) {
				a = b + 1;
				#pragma p1_unreachable end
			}
			else {
				b = a + 1;
			}
			#pragma p3_reachable end
		}
		else if (a + b < 2) {
			#pragma p2_unreachable end
			a = b + 1;
		}
	}
	return 0;
}
			
