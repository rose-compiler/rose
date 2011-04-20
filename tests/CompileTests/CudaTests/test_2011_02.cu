
__global__ void square_array() {
	__device__ float * res;
}

int main(int argc, char ** argv) {
	square_array <<< 1, 1 >>> ();
}
