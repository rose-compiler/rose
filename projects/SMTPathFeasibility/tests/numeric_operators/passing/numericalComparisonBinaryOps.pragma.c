int main(int i1, short s1, long l1, double d1, float f1) {
	#pragma p1_reachable begin infeasible
	#pragma p2_reachable begin infeasible
        #pragma p3_reachable begin infeasible
        #pragma p4_reachable begin infeasible
        #pragma p5_reachable begin infeasible
        #pragma p6_reachable begin infeasible
        #pragma p7_reachable begin infeasible
        #pragma p8_unreachable begin infeasible
        #pragma p9_reachable begin infeasible
        #pragma p10_reachable begin infeasible
        #pragma p11_reachable begin infeasible
        #pragma p12_reachable begin infeasible

	int i2 = 3;
	short s2 = 200;
	long l2 = 10000l;
	double d2 = 1.005;
	float f2 = 1.0005f;
	if (i1 > i2) { //path 1 has i1 = 0, i2 = 3
		#pragma p1_reachable end
		if (s1 >= s2) { // path 2 has i1 = 4, s1 = 0
			#pragma p2_reachable end
			if (l1 < l2) { // path 3 has s1 = 200, l1 = 10000
				#pragma p3_reachable end
				if (d1 <= d2) { // path 4 has l1 = 0, d1 = 401.0/200 = 2.005
					if (f1 != f2) { // path 5 has d1 = 0.0, f1 = 2001.0 / 2000 = 1.0005
						if (f1 == f1) { // this must always be true, thus there is only one unsat where the condition is assumed false
							int a = 0;
							#pragma p10_reachable end
						}
						else {
							#pragma p8_unreachable end
						}
						#pragma p11_reachable end	
					}
					else {
						#pragma p7_reachable end
					}
				}
				else {
					#pragma p6_reachable end
				}
			}
			else {
				#pragma p5_reachable end
			}
		}
		else {
			#pragma p4_reachable end
		}
	}		
	else {
		#pragma p12_reachable end
		int a = 2;
	}
	return 0;
}

