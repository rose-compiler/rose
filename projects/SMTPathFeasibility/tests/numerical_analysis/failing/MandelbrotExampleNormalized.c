// from 
// http://en.wikipedia.org/wiki/Mandelbrot_set#Other.2C_non-analytic.2C_mappings
int main() {
int X = 100;
int Y = 100;
double man_i,man_j,itr_x,itr_y,xtemp;
int iteration,max_iteration;
int i, j;
for (i = 0; i < X; i++) {
	for (j = 0; j < Y; j++) {
		man_i = i*(3.5/X)-2.5;
		man_j = j*(2/Y)-1;
		itr_x = 0.0;
		itr_y = 0.0;
		iteration = 0;
		max_iteration = 1000;
		double value = itr_x*itr_x + itr_y*itr_y;
		while ((value < 4) && (iteration < max_iteration)) {
			double t_xtemp,t_itr_y, t_itr_x,t_value;
			int t_iteration;
			xtemp = itr_x*itr_x - itr_y*itr_y + man_i;
			itr_y = 2*itr_x*itr_y + man_j;
			itr_x = xtemp;
			value = itr_x*itr_x + itr_y*itr_y;
			iteration = iteration + 1;
			t_xtemp = xtemp;
			t_itr_y = itr_y;
		 	t_itr_x = itr_x;
			t_value = value;
			t_iteration = iteration;
		}
		iteration = 0;
		//color = palette[iteration];
		// plot(i,j,color)
	}
}
return 0;
}

