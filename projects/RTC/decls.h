struct User{
	float* user_ptr1;
};

class Base{
	unsigned int *ptr1;
	unsigned int *ptr2;
	unsigned int var1;
	char var2;
	float var3;
	float* ptr3;
	unsigned int *ptr4;
	struct User *str_ptr1;
	public:
	virtual void print() {
		int* print_ptr1;
		printf("This is base class\n");
	}

	virtual void set_var1(unsigned int val) {
		var1 = val;
	}

	unsigned int get_var1() {
		return var1;
	}
};

class Derived1 : public Base {
	unsigned int* der1_ptr1;
	float* der1_ptr2;
	public:
	void print() {
		int* print_ptr1;
		printf("This is Derived1 class\n");
	}
};

class Derived2 : public Derived1 {
	unsigned int* der2_ptr1;
	float* der2_ptr2;
	class Base* der2_base_ptr1;
	class Derived1 der2_der1_obj;
	public:
	void print() {
		int* print_ptr1;
		printf("This is Derived2 class\n");
	}

	void set_base_ptr(unsigned int size) {

		//class Base* temp = new class Base[size];
		class Base* temp = (class Base*)malloc(size*sizeof(class Base));
		
		der2_base_ptr1 = &temp[size-1]; 
	}

	void set_two_pointers(unsigned int size) {

		der2_ptr1 = (unsigned int*)malloc(size*sizeof(unsigned int));

		der2_ptr2 = (float*)der2_ptr1++;

		set_base_ptr(size*2);

	}


};

struct node {
	class Base* base_ptr;
	unsigned int node_ctr;
	struct node* next;
	struct node* prev;
};

