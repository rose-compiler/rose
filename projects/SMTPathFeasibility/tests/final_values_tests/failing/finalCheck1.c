int main(int j) {
int i = 0;
i = i + 1;
if (j > i) {
	j = j + i;
}
else {
	j = 0;
}
for (int k = 0; k < 10; k++) {
	if (k == 1) {
		j = i*2;
	}
	else if (k == 3) {
		j = j-i;
	}
	else {
		i = i + 1;
	}
}

return 0;
}	
