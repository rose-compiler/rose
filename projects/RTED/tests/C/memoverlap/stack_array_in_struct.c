
int main( int argc, char **argv ) {

    struct {
        char before;
        int arr[ 3 ];
        char after;
    } var;

	int y;
	
	var.arr[ 0 ] = 0;
	var.arr[ 1 ] = 1;
	var.arr[ 2 ] = 2;

	// we should detect that var.arr[ 4 ] is out of bounds, even if it's legal
	// stack memory (e.g. &y)
	y = var.arr[ 4 ];

	return 0;
}

