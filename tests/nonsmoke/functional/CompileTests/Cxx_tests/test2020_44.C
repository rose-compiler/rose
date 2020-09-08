// RC-71:

    #include <string>
    #include <fstream>

    void bar(const std::string &);

    void foo() {
        std::ifstream ifile;

     // Unparsed as: std::__cxx11::string res(
     // std::istreambuf_iterator< std::basic_streambuf< char ,class std::char_traits< char  > > ::char_type ,std::basic_streambuf< char ,class std::char_traits< char  > > ::traits_type > (ifile),
     // std::istreambuf_iterator< std::basic_streambuf< char ,class std::char_traits< char  > > ::char_type ,std::basic_streambuf< char ,class std::char_traits< char  > > ::traits_type > ());
     // Original code:
        std::string res(
         (std::istreambuf_iterator<char>(ifile)), // <<< these parenthesis are needed. ROSE does not add them.
       // std::istreambuf_iterator<char>(ifile), // <<< these parenthesis are needed. ROSE does not add them.
          std::istreambuf_iterator<char>()
        );

        bar(res);
    }

