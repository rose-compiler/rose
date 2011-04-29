
__global__ void square_array() {
}

int main(int argc, char ** argv) {
	square_array <<< 1,1 >>> ();
}
