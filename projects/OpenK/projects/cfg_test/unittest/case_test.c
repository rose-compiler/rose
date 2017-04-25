
int foo(int class)
{
	int g = 0;
	switch(class) {
		case 1:
			g += 2;
			break;
		case 2:
			g++;
		case 3:
			g++;
		default:
			g = 0;
	}

	return g;
}
