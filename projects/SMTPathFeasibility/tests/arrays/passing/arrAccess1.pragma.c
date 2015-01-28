int main() {
	#pragma p1_reachable begin infeasible
	int a[5];
	a[1] = 2;
	#pragma p1_reachable end
	return 0;
}
