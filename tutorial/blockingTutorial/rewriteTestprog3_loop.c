
int main(int argc, char *argv[]) {
	double a[10][10];
	double b[10][10];

	for(int i=0;i<99;i++)
		for(int j=0;j<99;j++) {
			a[i][j] = b[i][j];
		}
	
	return 0;
}


