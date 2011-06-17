
#include "rose-max-cover/TDBUtraversal.hpp"

int main(int argc, char * argv[]) {
        SgProject* project = frontend(argc,argv);
        
        PolyhedralModelisation poly_model;
        
        poly_model.traverse(project);
        
        return 0;
}
