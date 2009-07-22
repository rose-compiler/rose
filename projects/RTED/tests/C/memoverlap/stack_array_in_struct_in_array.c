typedef struct {
	char before;
	int arr[ 3 ];
	char after;
} type;


int main( int argc, char **argv ) {

	type var[ 2 ];
	type extra_var;
	int y;
	
	var[ 0 ].arr[ 0 ] = 0;
	var[ 0 ].arr[ 1 ] = 1;
	var[ 0 ].arr[ 2 ] = 2;

	var[ 1 ].arr[ 0 ] = 0;
	var[ 1 ].arr[ 1 ] = 1;
	var[ 1 ].arr[ 2 ] = 2;

	extra_var.arr[ 0 ] = 0;
	extra_var.arr[ 1 ] = 1;
	extra_var.arr[ 2 ] = 2;


	// we should detect that var[ 2 ] is out of bounds, even if it's the start
	// of a `type' on the stack (e.g. &extra_var).
	y = var[ 2 ].arr[ 2 ];

	// we should detect that var[ 0 ].arr[ 3 ] is out of bounds, even if it's
	// legal stack memory
	y = var[ 0 ].arr[ 3 ];

	return 0;
}

