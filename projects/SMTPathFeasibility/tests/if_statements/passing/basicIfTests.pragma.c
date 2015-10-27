int main(int a, double b) {
	double d = -12.0;
	#pragma p1_reachable begin infeasible
	#pragma p2_unreachable begin infeasible
	#pragma p3_unreachable begin infeasible
	if (a > 4 && b > 3.0) {
		if (b < 2.0) {
			#pragma p2_unreachable end
			double d;
			d = b + 1.0;
			b = d;
		}
		else {
			double d;
			d = b - 1.0;
			b = d;
		}
	}
	else {
		if (b >= 2.0) {
			#pragma p1_reachable end
			double d;
			d = 2.05;
			b = d;
		}
	}
	if (d != -12.0) {
		#pragma p3_unreachable end
		d = 2.0;
	}
	return 0;
}
