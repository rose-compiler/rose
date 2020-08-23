// RC-71:

    #include <string>
    #include <fstream>

    void bar(const std::string &);

    void foo() {
        std::ifstream ifile;
        std::string res(
          (std::istreambuf_iterator<char>(ifile)), // <<< these parenthesis are needed. ROSE does not add them.
          std::istreambuf_iterator<char>()
        );
        bar(res);
    }

