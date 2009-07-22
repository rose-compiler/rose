
int main( int argc, char **argv ) {

	int x[ 3 ] = { 0, 1, 2 };
	int y;
	
	// we should detect that x[ 4 ] is out of bounds, even if it's legal stack
	// memory (e.g. &y)
	y = x[ 4 ];

	return 0;
}

