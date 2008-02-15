
#include "steensgaard.h"

struct lessVariable
{
   //typedef long Variable;
   bool operator () (const Variable v1, const Variable v2)
   {
      return v1 < v2;
   }
};

int main(int argc, char* argv[]) {
  ECRmap table;

  if (argc <= 1) {
     std::cerr << "Usage: " << argv[0] << " <int>\n";
     exit(1);
  }
  if (*argv[1] > '0') {
    std::cout << "a = &x;\n";
    table.x_eq_addr_y("a", "x");
  }
  if (*argv[1] > '1') {
     std::cout << "*x2 = x;\n";
     table.deref_x_eq_y("x2", "x");
     std::cout << "*a = a;\n";
     table.deref_x_eq_y("a", "a");
  }
  if (*argv[1] > '2') {
     std::cout << "a2 = a;\n";
     table.x_eq_y("a2", "a");
  } 
  if (*argv[1] > '3') {
     std::cout << "x3 = *x2;\n";
     table.x_eq_deref_y("x3", "x2");
  }
  if (*argv[1] > '4') {
     std::cout << "allocate(x)\n";
     table.allocate("x");
  }
  if (*argv[1] > '5') {
     std::cout << "y = op(y1,y2);\n";
     std::list<std::string> opd;
     opd.push_back("y1");
     opd.push_back("y2");
     table.x_eq_op_y("y",opd);
  }
  if (*argv[1] > '6') {
     std::cout << "func_def(p,(p1,p2),(p3,p4))\n";
     std::list<std::string> par,res;
     par.push_back("p1");
     par.push_back("p2");
     res.push_back("p3");
     res.push_back("p4");
     table.function_def_x("p",par,res);
  }
  if (*argv[1] > '7') {
     std::cout << "func_call(p,(y1,y2),(y3,y4))\n";
     std::list<std::string> par,res;
     par.push_back("y1");
     par.push_back("y2");
     res.push_back("y3");
     res.push_back("y4");
     table.function_call_p("p",res,par);
  }
  std::cout << "====================\n";
  table.output(std::cout);
  
  return 0;
}

