int main() {
	struct my_Struct {
		int a;
		double b;
		long c;
		float d;
		short e;
	} m1;
	struct my_Struct m2;
	m2.a = 1;
	struct my_Struct* p_m2 = &m2;
	int a_m = p_m2->a;
	int a_m2 = m2.a;
}	
	
