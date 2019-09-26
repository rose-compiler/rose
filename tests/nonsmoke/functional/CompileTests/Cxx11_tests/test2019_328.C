class test {
public:
    typedef void (test::*callback_func_ptr)();
    callback_func_ptr cb_func;
    void get_pc();
    void set_cb_ptr(callback_func_ptr ptr);
    void call_cb_func();
};

void test::get_pc() {
  // std::cout << "PC" << std::endl;
}

void test::set_cb_ptr(callback_func_ptr ptr) {
    cb_func = ptr;
}

void test::call_cb_func() {
    (this->*cb_func)();
}

int main() {
    test t1;
    t1.set_cb_ptr(&test::get_pc);
    t1.call_cb_func();
}

