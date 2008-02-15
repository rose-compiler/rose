#include "AttributeType.h"
using namespace std;
int main(){
ostringstream* test = new ostringstream();
AttributeType* hka = new AttributeType(test);
AttributeType* ha  = hka;
int k = 10;
typedef AttributeType::PlacementPositionEnum PlacementPositionEnum;
PlacementPositionEnum pos = AttributeType::AfterCurrentPosition;

(*ha) << "void humunculus(){\n";
ha->set(AttributeType::TopOfCurrentScope);
ha->set(pos);
ha->set(AttributeType::BottomOfCurrentScope);
//(*ha)
//ha->setBottomOfScope();
//(*ha) << "\n} // end of humunculus() \n"; 
(*ha) ("\nchar x;\n ",pos);// << ("a string" << k << "40" << "bullock" << endl ;
(*ha) ("\ntop of scope\n",AttributeType::TopOfCurrentScope);
(*ha) ("\nbottom of scope\n",AttributeType::BottomOfCurrentScope);
(*ha) << "Trying to write in txt\n" << ("\n after current  \n", pos);
(*ha) << "\n}\n";
(*ha) << "eks" << "pico" << "delo" << "presh";

cout << "This is the test string: \n" << ha->get()->str() << "\n String finnished \n";

return 0;
}
