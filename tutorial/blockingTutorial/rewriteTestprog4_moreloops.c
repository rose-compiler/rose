
int main(int argc, char *argv[]) {
	double a[99][99];
	double b[99][99];

	for(int j=1;j<2;j++) {
		a[0][j] = b[0][j];
	}

	for(int i=0;i<99;i++)
		for(int j=0;j<99;j++) {
			a[i][j] = b[i][j];
		}

	for(int i=1;i<99;i++)
		for(int j=2;j<88;j++) 
			for(int k=3;k<77;k++) 
				for(int l=4;l<66;l++) {
					a[i][k] = b[j][l];
				}

	for(int i=0;i<99;i++)
		for(int j=0;j>99;j++) {
			a[i][j] = b[i][j];
		}

	for(int i=0;i!=99;i++)
		for(int j=0;j!=99;j++) {
			a[i][j] = b[i][j];
		}

	return 0;
}


