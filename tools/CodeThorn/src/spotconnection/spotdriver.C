
#include "spotdriver.h"

int main(int argc, char* argv[]) {
  std::string stg_filename;
  stg_filename = argv[1];

  std::string ltl_filename;
  ltl_filename = argv[2];

  cout<<"STATUS: reading TGBA from file: "<<stg_filename<<endl;
  spot::bdd_dict* dict = new spot::bdd_dict();
  spot::ltl::environment& env(spot::ltl::default_environment::instance());
  spot::tgba_parse_error_list pel;
  spot::tgba_explicit_string* model_tgba = spot::tgba_parse(stg_filename.c_str(), pel, dict, env);
  if (spot::format_tgba_parse_errors(std::cerr, stg_filename.c_str(), pel)) {
    delete model_tgba;
    cerr<<"Error: tgba format error."<<endl;
    return 1;
  }

  ifstream ltl_input(ltl_filename.c_str());
  if (ltl_input.is_open()) {
  cout<<"STATUS: reading LTL formulas and their expected results from file: "<<ltl_filename<<endl;
    std::string result;
    ltlData* inputs = parse(ltl_input);
    for (ltlData::iterator iter = inputs->begin(); iter != inputs->end(); ++iter) {
      parseWeakUntil((*iter)->ltlString);
      if (checkFormula(model_tgba, (*iter)->ltlString, (*iter) -> expectedRes, dict)) {
        result = "correct solution: " + ( (*iter)->ltlString);
      } else {
        //error will be printed on std out by checkFormula(...)
      }
      cout << result << endl;
    }
  } else {
    cerr<<"Error: error while trying to read ltl file."<<endl;
    return 1;
  }
  //cout<<"STATUS: deallocating ressources for model tgba and bdd_dict."<<endl;
  dict->unregister_all_my_variables(model_tgba);
  delete model_tgba;
  delete dict;
  return 0;
}


bool checkFormula(spot::tgba_explicit_string* model_tgba, std::string ltl_string, bool expectedRes, spot::bdd_dict* dict) {
 
  bool result;
  //cout<<"STATUS: parsing LTL: "<<ltl_string<<endl;
  negateFormula(ltl_string);
  spot::ltl::parse_error_list pel;
  const spot::ltl::formula* f = spot::ltl::parse(ltl_string, pel);
  if (spot::ltl::format_parse_errors(std::cerr, ltl_string, pel)) {
    f->destroy();						
    cerr<<"Error: ltl format error."<<endl;
    //exit(1);
  }

  bool ce_expected = !expectedRes;
  bool fm_exprop_opt = false;
  bool fm_symb_merge_opt = true;
  bool post_branching = false;
  bool fair_loop_approx = false;

  spot::tgba* formula_tgba = spot::ltl_to_tgba_fm(f, dict, fm_exprop_opt,
                         fm_symb_merge_opt, post_branching, fair_loop_approx);

  //cout<<"STATUS: computing product."<<endl;
  spot::tgba_product product(formula_tgba,model_tgba);

  //cout<<"STATUS: emptiness check."<<endl;
  spot::emptiness_check *ec= new spot::couvreur99_check(&product);
  spot::emptiness_check_result* ce = ec->check();
  if(ce && !expectedRes) {   //formula succesfully falsified
    result = true;
    delete ce;
  } else if (ce && expectedRes) {    // error: counter-example exists even though formula is true on given automaton
    cout << "ERROR. Formula: " << ltl_string <<endl;
    cout << "Found following counter-example even though formula is true on all paths: " <<endl;
    spot::tgba_run* run = ce->accepting_run();
    if (run) {
      ostringstream oss;
      spot::print_tgba_run(oss, &product, run);	
      std::string spotRun = oss.str();
      displayRun(spotRun);	
      delete run;
    }
    result = false;
    delete ce; 
  } else if (!ce && !expectedRes) {    //error: no counter-example was not found, even though existing
    cout << "ERROR. No counter-example found for true formula " << ltl_string;
    result = false;
  } else {    //formula is true and no counter-example was found, correct answer
    result = true;
  }
  //cout<<"STATUS: deallocating ressources for formula tgba."<<endl;
  delete ec;
  dict->unregister_all_my_variables(formula_tgba);
  delete formula_tgba;
  f->destroy();

  return result;
}

ltlData* parse(istream& input) {
  ltlData* result = new std::list<FormulaPlusResult*>();
  std::string line;
  FormulaPlusResult* current;	
  while (std::getline(input, line)){
    if (line.size() > 7 && line.at(7) == ':') {   //means that a formula follows ("Formula: (....)")	
      line = line.substr(9, (line.size()-9));	//cut off non-formula line prefix
      current = new FormulaPlusResult();
      current->ltlString = line;
      //look two lines underneath for the result
      std::string resultLine;
      std::getline(input, resultLine);
      std::getline(input, resultLine);
      if (resultLine.size() > 11 && resultLine.at(11) == 'n'){  // "Formula is not satisfied! ..."
        current->expectedRes = false;
      } else if (resultLine.size() > 11 &&  resultLine.at(11) == 's') { // "Formula is satisfied."
        current->expectedRes = true;
      } else {cout << "parse ERROR" << endl; exceptionRaised: assert(0);  }
      result->push_back(current);
    }
  }
  return result;
}

void parseWeakUntil(std::string& ltl_string) {
  int positionCharW;
  while ( (positionCharW = ltl_string.find("WU")) != std::string::npos) {
    ltl_string.erase((positionCharW +1), 1);  //delete the following "U" of "WU"
  }
}

void negateFormula(std::string& ltl_string) {
  ltl_string = "!( " + ltl_string + ")";
}

void displayRun(string& run) {
  //cout << "******************************" << endl;
  //cout << run;
  cout << "******************************" << endl;
  cout << "I/O run:" << endl;
  cout << ( *filter_run_IO_only(run) ) << endl;	
  cout << "******************************" << endl;
  cout << "input run only:" << endl;
  cout << ( *filter_run_IO_only(run, true) ) << endl;	
  cout << "******************************" << endl;

}

// return a string representation of the I/O characters from the given spot run. 
// return only the input characters if parameter "inputOnly" is true.
std::string* filter_run_IO_only(string spotRun, bool inputOnly) {
  bool cycleStart = false;
  bool firstEntry = true;
  std::istringstream run(spotRun);
  std::string* result = new string();	
  std::string line;	
  while (std::getline(run, line)){
    boost::trim(line);
    if (line.at(0) == 'C') {  //identify where the cycle part of the run begins
      cycleStart = true;
    } else if (line.at(0) == '|') {  //indicates a transition
      line = line.substr(1, (line.size()-1));	//cut off the '|' prefix
      boost::trim(line);
      // iterate over all propositions in the transition and only add the non-negated one
      vector<std::string> props; 		
      boost::split_regex(props, line, boost::regex(" & "));
      vector<string>::iterator iter;
      for(iter = props.begin(); iter < props.end(); iter++) {
        if (iter->at(0) != '!') {
          if (inputOnly) {
            //add only if the variable starts with 'i' (input)
            if (iter->at(0) == 'i') {
              *result += formatIOChar(*iter, firstEntry, cycleStart);
              firstEntry = false;
            }
          } else {  //add both input and output characters
            *result += formatIOChar(*iter, firstEntry, cycleStart);
            firstEntry = false;
          }
        }
      }
      cycleStart = false;			
    }			
  }
  *result += ")*";	
  return result;
};

std::string formatIOChar(std::string prop, bool firstEntry, bool cycleStart) {
  std::string result;
  if (!firstEntry){  //manage comma separation
    result = ", ";
  }
  if (cycleStart) {  //manage cycle indication
    result += "("; 
  }
  result += prop;
  return result;
}
