// test for template functions 3, page 336 stroustrup, 13.3.2 [4] :
// Function Template Overloading: if a function and a specialization are equally good matches, the function is preferred!!!

// #include <iostream>



template <class T>
T min(T a, T b)
{
    return (a < b) ? a : b;
}

void foo(int r) {}




int main() {

	int i = 15;
        int j = 16;

     // a function does not hide a template
        int x  = min(i,j);

	{
   // Here an explicit function will hide the template function (and I think any explicit instantiation as well)
                int min(int, int);
		int wwwwwwwwwwwwwww;
		int yyy;
		yyy =  min(wwwwwwwwwwwwwww, yyy); // prefers "handwritten" min

	}

        foo(x);

  	return 0;

}



int min(int a, int b) {
	// std::cout << " hi" << std::endl;
	return a;

}
