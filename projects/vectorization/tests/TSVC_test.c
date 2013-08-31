#define LEN 32000
#define LEN2 256

#define ntimes 200000



#define TYPE float

#define lll LEN


__attribute__ ((aligned(16))) TYPE X[lll],Y[lll],Z[lll],U[lll],V[lll];


//float* __restrict__ array;
float array[LEN2*LEN2] __attribute__((aligned(16)));

float x[LEN] __attribute__((aligned(16)));
float temp;
int temp_int;


__attribute__((aligned(16))) float a[LEN],b[LEN],c[LEN],d[LEN],e[LEN],
                                   aa[LEN2][LEN2],bb[LEN2][LEN2],cc[LEN2][LEN2],tt[LEN2][LEN2];


int indx[LEN] __attribute__((aligned(16)));


float* __restrict__ xx;
float* yy;

int s000()
{
	for (int nl = 0; nl < 2*ntimes; nl++) {
#pragma SIMD
		for (int i = 0; i < lll; i++) {
//			a[i] = b[i] + c[i];
//			X[i] = (Y[i] * Z[i])+(U[i]*V[i]);
			X[i] = Y[i] + 1;
		}
	}
	return 0;
}

int s1112()
{

//	linear dependence testing
//	loop reversal


	for (int nl = 0; nl < ntimes*3; nl++) {
#pragma SIMD
		for (int i = LEN - 1; i >= 0; i--) {
			a[i] = b[i] + (float) 1.;
		}
	}
	return 0;
}

int s113()
{

//	linear dependence testing
//	a(i)=a(1) but no actual dependence cycle

	for (int nl = 0; nl < 4*ntimes; nl++) {
#pragma SIMD
		for (int i = 1; i < LEN; i++) {
			a[i] = a[0] + b[i];
		}
	}
	return 0;
}

int s212()
{

//	statement reordering
//	dependency needing temporary

	for (int nl = 0; nl < ntimes; nl++) {
#pragma SIMD
		for (int i = 0; i < LEN-1; i++) {
			a[i] *= c[i];
		}
		for (int i = 0; i < LEN-1; i++) {
			b[i] += a[i + 1] * d[i];
		}
	}
	return 0;
}

int s221()
{

//	loop distribution
//	loop that is partially recursive

	for (int nl = 0; nl < ntimes/2; nl++) {
#pragma SIMD
		for (int i = 1; i < LEN; i++) {
			a[i] += c[i] * d[i];
		}
		for (int i = 1; i < LEN; i++) {
			b[i] = b[i - 1] + a[i] + d[i];
		}
	}
	return 0;
}

int s222()
{

//	loop distribution
//	partial loop vectorizatio recurrence in middle

	for (int nl = 0; nl < ntimes/2; nl++) {
#pragma SIMD
		for (int i = 1; i < LEN; i++) {
			a[i] += b[i] * c[i];
		}
		for (int i = 1; i < LEN; i++) {
			e[i] = e[i - 1] * e[i - 1];
		}
#pragma SIMD
		for (int i = 1; i < LEN; i++) {
			a[i] -= b[i] * c[i];
		}
	}
	return 0;
}

// manually loop interchange in 231
int s231()
{
//	loop interchange
//	loop with data dependency

	for (int nl = 0; nl < 100*(ntimes/LEN2); nl++) {
			for (int j = 1; j < LEN2; j++) {
#pragma SIMD
		for (int i = 0; i < LEN2; ++i) {
				aa[j][i] = aa[j - 1][i] + bb[j][i];
			}
		}
	}
	return 0;
}

int s233()
{

//	loop interchange
//	interchanging with one of two inner loops

	for (int nl = 0; nl < 100*(ntimes/LEN2); nl++) {
			for (int j = 1; j < LEN2; j++) {
#pragma SIMD
		for (int i = 1; i < LEN2; i++) {
				aa[j][i] = aa[j-1][i] + cc[j][i];
			}
#pragma SIMD
		for (int i = 1; i < LEN2; i++) {
				bb[j][i] = bb[j][i-1] + cc[j][i];
			}
		}
	}
	return 0;
}

int s2233()
{

//	loop interchange
//	interchanging with one of two inner loops

	for (int nl = 0; nl < 100*(ntimes/LEN2); nl++) {
			for (int j = 1; j < LEN2; j++) {
#pragma SIMD
		for (int i = 1; i < LEN2; i++) {
				aa[j][i] = aa[j-1][i] + cc[j][i];
			}
		}
		for (int i = 1; i < LEN2; i++) {
#pragma SIMD
			for (int j = 1; j < LEN2; j++) {
				bb[i][j] = bb[i-1][j] + cc[i][j];
			}
		}
	}
	return 0;
}

int s235()
{

//	loop interchanging
//	imperfectly nested loops

	for (int nl = 0; nl < 200*(ntimes/LEN2); nl++) {
			for (int j = 1; j < LEN2; j++) {
#pragma SIMD
		for (int i = 0; i < LEN2; i++) {
			a[i] += b[i] * c[i];
				aa[j][i] = aa[j-1][i] + bb[j][i] * a[i];
			}
		}
	}
	return 0;
}


int s241()
{

//	node splitting
//	preloading necessary to allow vectorization

	for (int nl = 0; nl < 2*ntimes; nl++) {
#pragma SIMD
		for (int i = 0; i < LEN-1; i++) {
			a[i] = b[i] * c[i  ] * d[i];
		}
		for (int i = 0; i < LEN-1; i++) {
			b[i] = a[i] * a[i+1] * d[i];
		}
	}
	return 0;
}


int s243()
{

//	node splitting
//	false dependence cycle breaking

	for (int nl = 0; nl < ntimes; nl++) {
#pragma SIMD
		for (int i = 0; i < LEN-1; i++) {
			a[i] = b[i] + c[i  ] * d[i];
			b[i] = a[i] + d[i  ] * e[i];
		}
		for (int i = 0; i < LEN-1; i++) {
			a[i] = b[i] + a[i+1] * d[i];
		}
	}
	return 0;
}

int s244()
{

//	node splitting
//	false dependence cycle breaking


	for (int nl = 0; nl < ntimes; nl++) {
#pragma SIMD
		for (int i = 0; i < LEN-1; ++i) {
			a[i] = b[i] + c[i] * d[i];
			b[i] = c[i] + b[i];
		}
		for (int i = 0; i < LEN-1; ++i) {
			a[i+1] = b[i] + a[i+1] * d[i];
		}
	}
	return 0;
}

int s1244()
{

//	node splitting
//	cycle with ture and anti dependency

	for (int nl = 0; nl < ntimes; nl++) {
#pragma SIMD
		for (int i = 0; i < LEN-1; i++) {
			a[i] = b[i] + c[i] * c[i] + b[i]*b[i] + c[i];
		}
		for (int i = 0; i < LEN-1; i++) {
			d[i] = a[i] + a[i+1];
		}
	}
	return 0;
}

int s2244()
{

//	node splitting
//	cycle with ture and anti dependency

	for (int nl = 0; nl < ntimes; nl++) {
		for (int i = 0; i < LEN-1; i++) {
			a[i+1] = b[i] + e[i];
		}
#pragma SIMD
		for (int i = 0; i < LEN-1; i++) {
			a[i] = b[i] + c[i];
		}
	}
	return 0;
}

int s251()
{

//	scalar and array expansion
//	scalar expansion

	float s;
	for (int nl = 0; nl < 4*ntimes; nl++) {
#pragma SIMD
		for (int i = 0; i < LEN; i++) {
			s = b[i] + c[i] * d[i];
			a[i] = s * s;
		}
	}
	return 0;
}


int s1251()
{

//	scalar and array expansion
//	scalar expansion

	float s;
	for (int nl = 0; nl < 4*ntimes; nl++) {
#pragma SIMD
		for (int i = 0; i < LEN; i++) {
			s = b[i]+c[i];
			b[i] = a[i]+d[i];
			a[i] = s*e[i];
		}
	}
	return 0;
}

int s2251()
{

//	scalar and array expansion
//	scalar expansion


	for (int nl = 0; nl < ntimes; nl++) {
		float s = (float)0.0;
#pragma SIMD
		for (int i = 0; i < LEN; i++) {
			a[i] = s*e[i];
			s = b[i]+c[i];
			b[i] = a[i]+d[i];
		}
	}
	return 0;
}

/* Fail at liveness analysis */
//int s3251()
//{
//
////	scalar and array expansion
////	scalar expansion
//
//
//	for (int nl = 0; nl < ntimes; nl++) {
//		for (int i = 0; i < LEN-1; i++){
//			a[i+1] = b[i]+c[i];
//		}
//#pragma SIMD
//		for (int i = 0; i < LEN-1; i++){
//			b[i]   = c[i]*e[i];
//			d[i]   = a[i]*e[i];
//		}
//	}
//	return 0;
//}

int s252()
{

//	scalar and array expansion
//	loop with ambiguous scalar temporary


	float t, s;
	for (int nl = 0; nl < ntimes; nl++) {
		t = (float) 0.;
#pragma SIMD
		for (int i = 0; i < LEN; i++) {
			s = b[i] * c[i];
			a[i] = s + t;
			t = s;
		}
	}
	return 0;
}

int s253()
{

//	scalar and array expansion
//	scalar expansio assigned under if


	float s;
	for (int nl = 0; nl < ntimes; nl++) {
#pragma SIMD
		for (int i = 0; i < LEN; i++) {
			if (a[i] > b[i]) {
				s = a[i] - b[i] * d[i];
				c[i] += s;
				a[i] = s;
			}
		}
	}
	return 0;
}


int s254()
{

//	scalar and array expansion
//	carry around variable

	float x;
	for (int nl = 0; nl < 4*ntimes; nl++) {
		x = b[LEN-1];
#pragma SIMD
		for (int i = 0; i < LEN; i++) {
			a[i] = (b[i] + x) * (float).5;
			x = b[i];
		}
	}
	return 0;
}

// %2.5

int s255()
{

//	scalar and array expansion
//	carry around variables, 2 levels


	float x, y;
	for (int nl = 0; nl < ntimes; nl++) {
		x = b[LEN-1];
		y = b[LEN-2];
#pragma SIMD
		for (int i = 0; i < LEN; i++) {
			a[i] = (b[i] + x + y) * (float).333;
			y = x;
			x = b[i];
		}
	}
	return 0;
}

// %2.5

int s256()
{

//	scalar and array expansion
//	array expansion


	for (int nl = 0; nl < 10*(ntimes/LEN2); nl++) {
		for (int i = 0; i < LEN2; i++) {
#pragma SIMD
			for (int j = 1; j < LEN2; j++) {
				a[j] = (float)1.0 - a[j - 1];
				cc[j][i] = a[j] + bb[j][i]*d[j];
			}
		}
	}
	return 0;
}

int s258()
{

//	scalar and array expansion
//	wrap-around scalar under an if


	float s;
	for (int nl = 0; nl < ntimes/10; nl++) {
		s = 0.;
#pragma SIMD
		for (int i = 0; i < LEN; ++i) {
			if (a[i] > 0.) {
				s = d[i] * d[i];
			}
			b[i] = s * c[i] + d[i];
			e[i] = (s + (float)1.) * aa[0][i];
		}
	}
	return 0;
}

int s261()
{

//	scalar and array expansion
//	wrap-around scalar under an if


	float t;
	for (int nl = 0; nl < ntimes; nl++) {
		for (int i = 1; i < LEN; ++i) {
			t = a[i] + b[i];
			a[i] = t + c[i-1];
		}
#pragma SIMD
		for (int i = 1; i < LEN; ++i) {
			t = c[i] * d[i];
			c[i] = t;
		}
	}
	return 0;
}

int s271()
{

//	control flow
//	loop with singularity handling


	for (int nl = 0; nl < 4*ntimes; nl++) {
#pragma SIMD
		for (int i = 0; i < LEN; i++) {
			if (b[i] > (float)0.) {
				a[i] += b[i] * c[i];
			}
		}
	}
	return 0;
}

int s272(float t)
{

//	control flow
//	loop with independent conditional

	for (int nl = 0; nl < ntimes; nl++) {
#pragma SIMD
		for (int i = 0; i < LEN; i++) {
			if (e[i] >= t) {
				a[i] += c[i] * d[i];
				b[i] += c[i] * c[i];
			}
		}
	}
	return 0;
}

int s273()
{

//	control flow
//	simple loop with dependent conditional


	for (int nl = 0; nl < ntimes; nl++) {
#pragma SIMD
		for (int i = 0; i < LEN; i++) {
			a[i] += d[i] * e[i];
			if (a[i] < (float)0.)
				b[i] += d[i] * e[i];
		}
#pragma SIMD
		for (int i = 0; i < LEN; i++) {
			c[i] += a[i] * d[i];
		}
	}
	return 0;
}

int s274()
{

//	control flow
//	complex loop with dependent conditional


	for (int nl = 0; nl < ntimes; nl++) {
#pragma SIMD
		for (int i = 0; i < LEN; i++) {
			a[i] = c[i] + e[i] * d[i];
			if (a[i] > (float)0.) {
				b[i] = a[i] + b[i];
			} else {
				a[i] = d[i] * e[i];
			}
		}
	}
	return 0;
}


int s275()
{

//	control flow
//	if around inner loop, interchanging needed

	for (int nl = 0; nl < 10*(ntimes/LEN2); nl++) {
				for (int j = 1; j < LEN2; j++) {
#pragma SIMD
		for (int i = 0; i < LEN2; i++) {
			if (aa[0][i] > (float)0.) {
					aa[j][i] = aa[j-1][i] + bb[j][i] * cc[j][i];
				}
			}
		}
	}
	return 0;
}

int s2275()
{

//	loop distribution is needed to be able to interchange

	for (int nl = 0; nl < 100*(ntimes/LEN2); nl++) {
			for (int j = 0; j < LEN2; j++) {
#pragma SIMD
		for (int i = 0; i < LEN2; i++) {
				aa[j][i] = aa[j][i] + bb[j][i] * cc[j][i];
			a[i] = b[i] + c[i] * d[i];
			}
		}
	}
	return 0;
}

//int s276()
//{
//
////	control flow
////	if test using loop index
//
//	int mid = (LEN/2);
//	for (int nl = 0; nl < 4*ntimes; nl++) {
//#pragma SIMD
//		for (int i = 0; i < LEN; i++) {
//			if (i+1 < mid) {
//				a[i] += b[i] * c[i];
//			} else {
//				a[i] += b[i] * d[i];
//			}
//		}
//	}
//	return 0;
//}

int s2711()
{

//	control flow
//	semantic if removal

	for (int nl = 0; nl < 4*ntimes; nl++) {
#pragma SIMD
		for (int i = 0; i < LEN; i++) {
			if (b[i] != (float)0.0) {
				a[i] += b[i] * c[i];
			}
		}
	}
	return 0;
}

int s2712()
{

//	control flow
//	if to elemental min

	for (int nl = 0; nl < 4*ntimes; nl++) {
#pragma SIMD
		for (int i = 0; i < LEN; i++) {
			if (a[i] > b[i]) {
				a[i] += b[i] * c[i];
			}
		}
	}
	return 0;
}

int s1281()
{

//	crossing thresholds
//	index set splitting
//	reverse data access

	float x;
	for (int nl = 0; nl < 4*ntimes; nl++) {
#pragma SIMD
		for (int i = 0; i < LEN; i++) {
			x = b[i]*c[i]+a[i]*d[i]+e[i];
			a[i] = x-(float)1.0;
			b[i] = x;
		}
	}
	return 0;
}

//int s293()
//{
//
////	loop peeling
////	a(i)=a(0) with actual dependence cycle, loop is vectorizable
//
//	for (int nl = 0; nl < 4*ntimes; nl++) {
//#pragma SIMD
//		for (int i = 0; i < LEN; i++) {
//			a[i] = a[0];
//		}
//	}
//	return 0;
//}

int s2101()
{

//	diagonals
//	main diagonal calculation
//	jump in data access

	for (int nl = 0; nl < 10*ntimes; nl++) {
#pragma SIMD
		for (int i = 0; i < LEN2; i++) {
			aa[i][i] += bb[i][i] * cc[i][i];
		}
	}
	return 0;
}

int s2102()
{

//	diagonals
//	identity matrix, best results vectorize both inner and outer loops


	for (int nl = 0; nl < 100*(ntimes/LEN2); nl++) {
			for (int j = 0; j < LEN2; j++) {
#pragma SIMD
		for (int i = 0; i < LEN2; i++) {
				aa[j][i] = (float)0.;
			}
		}
#pragma SIMD
		for (int i = 0; i < LEN2; i++) {
			aa[i][i] = (float)1.;
		}
	}
	return 0;
}

int s351()
{

//	loop rerolling
//	unrolled saxpy

	float alpha = c[0];
	for (int nl = 0; nl < 8*ntimes; nl++) {
#pragma SIMD
		for (int i = 0; i < LEN; i++ ) {
			a[i] += alpha * b[i];
		}
	}
	return 0;
}

int s352()
{

//	loop rerolling
//	unrolled dot product

	float dot;
	for (int nl = 0; nl < 8*ntimes; nl++) {
		dot = (float)0.;
#pragma SIMD
		for (int i = 0; i < LEN; i += 5) {
			dot = dot + a[i] * b[i] + a[i + 1] * b[i + 1] + a[i + 2]
				* b[i + 2] + a[i + 3] * b[i + 3] + a[i + 4] * b[i + 4];
		}
	}
	return 0;
}

//int s431()
//{
//
////	parameters
////	parameter statement
//
//	int k1=1;
//	int k2=2;
//	int k=2*k1-k2;
//
//	for (int nl = 0; nl < ntimes*10; nl++) {
//#pragma SIMD
//		for (int i = 0; i < LEN; i++) {
//			a[i] = a[i+k] + b[i];
//		}
//	}
//	return 0;
//}

int s441()
{

//	non-logical if's
//	arithmetic if

	for (int nl = 0; nl < ntimes; nl++) {
#pragma SIMD
		for (int i = 0; i < LEN; i++) {
			if (d[i] < (float)0.) {
				a[i] += b[i] * c[i];
			} else {
				a[i] += c[i] * c[i];
			}
		}
#pragma SIMD
		for (int i = 0; i < LEN; i++) {
			if (d[i] == (float)0.) {
				a[i] += b[i] * b[i];
			} else {
				a[i] += c[i] * c[i];
			}
		}
	}
	return 0;
}

int s451()
{

//	intrinsic functions
//	intrinsics

	for (int nl = 0; nl < ntimes/5; nl++) {
#pragma SIMD
		for (int i = 0; i < LEN; i++) {
			a[i] = sinf(b[i]) + cosf(c[i]);
		}
	}
	return 0;
}

//int s452()
//{
//
////	intrinsic functions
////	seq function
//
//	for (int nl = 0; nl < 4*ntimes; nl++) {
//#pragma SIMD
//		for (int i = 0; i < LEN; i++) {
//			a[i] = b[i] + c[i] * (float) (i+1);
//		}
//	}
//	return 0;
//}
//
int s453()
{

//	induction varibale recognition

	float s;

	for (int nl = 0; nl < ntimes*2; nl++) {
		s = 0.;
#pragma SIMD
		for (int i = 0; i < LEN; i++) {
			s += (float)2.;
			a[i] = s * b[i];
		}
	}
	return 0;
}

//
//int s491(int* __restrict__ ip)
//{
//
////	vector semantics
////	indirect addressing on lhs, store in sequence
////	scatter is required
//
//	for (int nl = 0; nl < ntimes; nl++) {
//#pragma SIMD
//		for (int i = 0; i < LEN; i++) {
//			a[ip[i]] = b[i] + c[i] * d[i];
//		}
//	}
//	return 0;
//}
//
//int s4112(int* __restrict__ ip, float s)
//{
//
////	indirect addressing
////	sparse saxpy
////	gather is required
//
//	for (int nl = 0; nl < ntimes; nl++) {
//#pragma SIMD
//		for (int i = 0; i < LEN; i++) {
//			a[i] += b[ip[i]] * s;
//		}
//	}
//	return 0;
//}
//
//int s4113(int* __restrict__ ip)
//{
//
////	indirect addressing
////	indirect addressing on rhs and lhs
////	gather and scatter is required
//
//
//	for (int nl = 0; nl < ntimes; nl++) {
//#pragma SIMD
//		for (int i = 0; i < LEN; i++) {
//			a[ip[i]] = b[ip[i]] + c[i];
//		}
//	}
//	return 0;
//}
//
int va()
{

//	control loops
//	vector assignment

	for (int nl = 0; nl < ntimes*10; nl++) {
#pragma SIMD
		for (int i = 0; i < LEN; i++) {
			a[i] = b[i];
		}
	}
	return 0;
}

//int vag( int* __restrict__ ip)
//{
//
////	control loops
////	vector assignment, gather
////	gather is required
//
//	clock_t start_t, end_t, clock_dif; double clock_dif_sec;
//
//
//	init( "vag  ");
//	start_t = clock();
//
//	for (int nl = 0; nl < 2*ntimes; nl++) {
//		for (int i = 0; i < LEN; i++) {
//			a[i] = b[ip[i]];
//		}
//		dummy(a, b, c, d, e, aa, bb, cc, 0.);
//	}
//	end_t = clock(); clock_dif = end_t - start_t;
//	clock_dif_sec = (double) (clock_dif/1000000.0);
//	printf("vag\t %.2f \t\t", clock_dif_sec);;
//	check(1);
//	return 0;
//}
//
//// %5.1
//
//int vas( int* __restrict__ ip)
//{
//
////	control loops
////	vector assignment, scatter
////	scatter is required
//
//	clock_t start_t, end_t, clock_dif; double clock_dif_sec;
//
//
//	init( "vas  ");
//	start_t = clock();
//
//	for (int nl = 0; nl < 2*ntimes; nl++) {
//		for (int i = 0; i < LEN; i++) {
//			a[ip[i]] = b[i];
//		}
//		dummy(a, b, c, d, e, aa, bb, cc, 0.);
//	}
//	end_t = clock(); clock_dif = end_t - start_t;
//	clock_dif_sec = (double) (clock_dif/1000000.0);
//	printf("vas\t %.2f \t\t", clock_dif_sec);;
//	check(1);
//	return 0;
//}
//
int vif()
{

//	control loops
//	vector if

	for (int nl = 0; nl < ntimes; nl++) {
#pragma SIMD
		for (int i = 0; i < LEN; i++) {
			if (b[i] > (float)0.) {
				a[i] = b[i];
			}
		}
	}
	return 0;
}

int vpv()
{

//	control loops
//	vector plus vector


	for (int nl = 0; nl < ntimes*10; nl++) {
#pragma SIMD
		for (int i = 0; i < LEN; i++) {
			a[i] += b[i];
		}
	}
	return 0;
}

int vtv()
{

//	control loops
//	vector times vector

	// Function Body
	for (int nl = 0; nl < ntimes*10; nl++) {
#pragma SIMD
		for (int i = 0; i < LEN; i++) {
			a[i] *= b[i];
		}
	}
	return 0;
}

// %5.1

int vpvtv()
{

//	control loops
//	vector plus vector times vector


	for (int nl = 0; nl < 4*ntimes; nl++) {
#pragma SIMD
		for (int i = 0; i < LEN; i++) {
			a[i] += b[i] * c[i];
		}
	}
	return 0;
}

// %5.1

//int vpvts( float s)
//{
//
////	control loops
////	vector plus vector times scalar
//
//	for (int nl = 0; nl < ntimes; nl++) {
//#pragma SIMD
//		for (int i = 0; i < LEN; i++) {
//			a[i] += b[i] * s;
//		}
//	}
//	return 0;
//}

int vpvpv()
{

//	control loops
//	vector plus vector plus vector


	for (int nl = 0; nl < 4*ntimes; nl++) {
#pragma SIMD
		for (int i = 0; i < LEN; i++) {
			a[i] += b[i] + c[i];
		}
	}
	return 0;
}

int vtvtv()
{

//	control loops
//	vector times vector times vector

	for (int nl = 0; nl < 4*ntimes; nl++) {
		for (int i = 0; i < LEN; i++) {
			a[i] = a[i] * b[i] * c[i];
		}
	}
	return 0;
}

