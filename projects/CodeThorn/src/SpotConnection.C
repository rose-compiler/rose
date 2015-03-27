#include "sage3basic.h"
#include "SpotConnection.h"

using namespace CodeThorn;

void SpotConnection::init(std::string ltl_formulae_file) {
  //open text file that contains the properties
  ifstream ltl_input(ltl_formulae_file.c_str());
  if (ltl_input.is_open()) {
    //load the containing formulae
    loadFormulae(ltl_input);  //load the formulae into class member "behaviorProperties"
    //initialize the results table with the right size for the properties that will be evaluated
    ltlResults = new PropertyValueTable(behaviorProperties.size());
  }
}

std::list<LtlProperty>* SpotConnection::getUnknownFormulae() {
  std::list<LtlProperty>* result = new std::list<LtlProperty>();
  std::list<int>* unknownPropertyNumbers = ltlResults->getPropertyNumbers(PROPERTY_VALUE_UNKNOWN);
  
  for (std::list<int>::iterator i = unknownPropertyNumbers->begin(); i != unknownPropertyNumbers->end(); ++i) {
    //possible improvement: reduce to one loop only by sorting the behaviorProperty list according to the propertyNumbers
    for (std::list<LtlProperty>::iterator k = behaviorProperties.begin(); k != behaviorProperties.end(); ++k) {
      if ((*i) == k->propertyNumber) {
        result->push_back(*k);
      }
    }
  }
  return result;
}

PropertyValueTable* SpotConnection::getLtlResults() {
  if (ltlResults) {
    return new PropertyValueTable(*ltlResults); 
  } else {
    cout<< "ERROR: LTL results requested even though the SpotConnection has not been initialized yet." << endl;
    assert(0);
  }
}

void SpotConnection::resetLtlResults() { 
  ltlResults->init(ltlResults->size()); 
}

void SpotConnection::checkSingleProperty(int propertyNum, TransitionGraph& stg, 
						std::set<int> inVals, std::set<int> outVals, bool withCounterexample, bool spuriousNoAnswers) {
  if (stg.size() == 0) {
    cout << "STATUS: the transition system used as a model is empty, LTL behavior could not be checked." << endl;
    return;
  }
  if (!stg.isPrecise() && !stg.isComplete()) {
    return;  //neither falsification nor verification works
  } 
  //prepare the analysis
  //determine largest input Value, then merge input and output alphabet
  int maxInputVal = *( std::max_element(inVals.begin(), inVals.end()) );
  std::set<int> ioValues = inVals;
  ioValues.insert(outVals.begin(), outVals.end());
  //initializing the atomic propositions used based on I/O values
  spot::ltl::atomic_prop_set* sap = getAtomicProps(ioValues, maxInputVal);
  //instantiate a new dictionary for atomic propositions 
  // (will be used by the model tgba as well as by the ltl formula tgbas)
  spot::bdd_dict dict;
  //create a tgba from CodeThorn's STG model
  SpotTgba* ct_tgba = new SpotTgba(stg, *sap, dict, inVals, outVals);
  LtlProperty ltlProperty; 
  //TODO: improve on this iteration over list elements
  for (std::list<LtlProperty>::iterator i = behaviorProperties.begin(); i != behaviorProperties.end(); i++) {
    if (i->propertyNumber == propertyNum) {
      ltlProperty = *i;
      break;
    }
  }
  checkAndUpdateResults(ltlProperty, ct_tgba, stg, withCounterexample, spuriousNoAnswers);
  delete ct_tgba;
  ct_tgba = NULL;
}

void SpotConnection::checkAndUpdateResults(LtlProperty property, SpotTgba* ct_tgba, TransitionGraph& stg, 
						bool withCounterexample, bool spuriousNoAnswers) {
  std::string* pCounterExample;
  if (checkFormula(ct_tgba, property.ltlString , ct_tgba->get_dict(), &pCounterExample)) {  //SPOT returns that the formula could be verified
    if (stg.isComplete()) {
      ltlResults->strictUpdatePropertyValue(property.propertyNumber, PROPERTY_VALUE_YES);
    } else {
      //not all possible execution paths are covered in this stg model, ignore SPOT's result
    }
  } else {  //SPOT returns that there exists a counterexample that falsifies the formula
    if (stg.isPrecise()) {
      ltlResults->strictUpdatePropertyValue(property.propertyNumber, PROPERTY_VALUE_NO);
      if (withCounterexample) {
        ltlResults->strictUpdateCounterexample(property.propertyNumber, *pCounterExample);
      }
      delete pCounterExample;
    } else {
      // old: the stg is over-approximated, falsification cannot work. Ignore SPOT's answer.
      if (spuriousNoAnswers) {
        // new: register counterexample and check it later
        ltlResults->strictUpdatePropertyValue(property.propertyNumber, PROPERTY_VALUE_NO);
        if (withCounterexample) {
          ltlResults->strictUpdateCounterexample(property.propertyNumber, *pCounterExample);
        }
        delete pCounterExample;
      }
    }
  }
  pCounterExample = NULL;
}

void SpotConnection::checkLtlProperties(TransitionGraph& stg,
						std::set<int> inVals, std::set<int> outVals, bool withCounterexample, bool spuriousNoAnswers) {
  if (stg.size() == 0) {
    cout << "STATUS: the transition system used as a model is empty, LTL behavior could not be checked." << endl;
    return;
  }
  if (!stg.isPrecise() && !stg.isComplete()) {
    return;  //neither falsification nor verification works
  } else {  //prepare the analysis
    //determine largest input Value, then merge input and output alphabet
    int maxInputVal = *( std::max_element(inVals.begin(), inVals.end()) );
    std::set<int> ioValues = inVals;
    ioValues.insert(outVals.begin(), outVals.end());
    //initializing the atomic propositions used based on I/O values
    spot::ltl::atomic_prop_set* sap = getAtomicProps(ioValues, maxInputVal);
    //instantiate a new dictionary for atomic propositions 
    // (will be used by the model tgba as well as by the ltl formula tgbas)
    spot::bdd_dict dict;
    //create a tgba from CodeThorn's STG model
    SpotTgba* ct_tgba = new SpotTgba(stg, *sap, dict, inVals, outVals);
    std::string* pCounterExample; 
    std::list<LtlProperty>* yetToEvaluate = getUnknownFormulae();
    for (std::list<LtlProperty>::iterator i=yetToEvaluate->begin(); i != yetToEvaluate->end(); ++i) {
    }
    for (std::list<LtlProperty>::iterator i = yetToEvaluate->begin(); i != yetToEvaluate->end(); ++i) {
      if (checkFormula(ct_tgba, i->ltlString , ct_tgba->get_dict(), &pCounterExample)) {  //SPOT returns that the formula could be verified
        if (stg.isComplete()) {
          ltlResults->strictUpdatePropertyValue(i->propertyNumber, PROPERTY_VALUE_YES);
        } else {
          //not all possible execution paths are covered in this stg model, ignore SPOT's result
        }
      } else {  //SPOT returns that there exists a counterexample that falsifies the formula
        if (stg.isPrecise()) {
          ltlResults->strictUpdatePropertyValue(i->propertyNumber, PROPERTY_VALUE_NO);
          if (withCounterexample) {
            ltlResults->strictUpdateCounterexample(i->propertyNumber, *pCounterExample);
          }
          delete pCounterExample;
        } else {
          // old: the stg is over-approximated, falsification cannot work. Ignore SPOT's answer.
          if (spuriousNoAnswers) {
            // new: register counterexample and check it later
            ltlResults->strictUpdatePropertyValue(i->propertyNumber, PROPERTY_VALUE_NO);
            if (withCounterexample) {
              ltlResults->strictUpdateCounterexample(i->propertyNumber, *pCounterExample);
            }
            delete pCounterExample;
          }
        }
      }
    } //end of "for each unknown property" loop
    pCounterExample = NULL;
    delete yetToEvaluate;
    yetToEvaluate = NULL;
    delete ct_tgba;
    ct_tgba = NULL;
  } //end of implicit condition (stg.isPrecise() || stg.isComplete())
}

void SpotConnection::compareResults(TransitionGraph& stg, std::string ltl_fsPlusRes_file,
					std::set<int> inVals, std::set<int> outVals) {
  //determine largest input Value, then merge input and output alphabet
  int maxInputVal = *( std::max_element(inVals.begin(), inVals.end()) );
  std::set<int> ioValues = inVals;
  ioValues.insert(outVals.begin(), outVals.end());
  //initializing the atomic propositions used based on I/O values
  spot::ltl::atomic_prop_set* sap = getAtomicProps(ioValues, maxInputVal);
  //instantiate a new dictionary for atomic propositions 
  // (will be used by the model tgba as well as by the ltl formula tgbas)
  spot::bdd_dict dict;
  //create a tgba from CodeThorn's STG model
  SpotTgba* ct_tgba = new SpotTgba(stg, *sap, dict, inVals, outVals);
  // open the LTL text file
  ifstream ltl_input(ltl_fsPlusRes_file.c_str());
  if (ltl_input.is_open()) {
    // parse all LTL formulas and their corresponding solutions
    ltlData* inputs = parseSolutions(ltl_input);
    // check each formula indivdually, using the same model tgba at all times
    std::string* pCounterExample;
    for (ltlData::iterator iter = inputs->begin(); iter != inputs->end(); ++iter) {
      parseWeakUntil((*iter)->ltlString);
      //check formula and print out how the result compares to the expected solution  
      bool spotResult = checkFormula(ct_tgba, (*iter)->ltlString, ct_tgba->get_dict(), &pCounterExample);
      cout << comparison(spotResult, (*iter)->expectedRes, (*iter)->ltlString, *pCounterExample) << endl;
    }
    ltl_input.close();
  }
  delete ct_tgba;
  ct_tgba = NULL;
}


///deprecated. uses text format for the tgba. Newer version is implemented that could also make use of on the fly computation
void SpotConnection::compareResults(std::string tgba_file, std::string ltl_fsPlusRes_file) {
  
  //cout<<"STATUS: reading STG from file: "<<tgba_file<<endl;
  spot::bdd_dict* dict = new spot::bdd_dict();
  spot::ltl::environment& env(spot::ltl::default_environment::instance());
  spot::tgba_parse_error_list pel2;
  spot::tgba_explicit_string* model_tgba = spot::tgba_parse(tgba_file.c_str(), pel2, dict, env);
  if (spot::format_tgba_parse_errors(std::cerr, tgba_file.c_str(), pel2)) {
    cerr<<"Error: stg format error."<<endl;
    assert(0);
  }
  //open LTL text file and check for each formula if the provided solution is correct
  ifstream ltl_input(ltl_fsPlusRes_file.c_str());
  if (ltl_input.is_open()) {
    ltlData* inputs = parseSolutions(ltl_input);
    std::string* pCounterExample;
    for (ltlData::iterator iter = inputs->begin(); iter != inputs->end(); ++iter) {
      parseWeakUntil((*iter)->ltlString);
      //check formula and print out how the result compares to the expected solution  
      bool spotResult = checkFormula(dynamic_cast<spot::tgba*>(model_tgba), (*iter)->ltlString, dict, &pCounterExample);
      cout << comparison(spotResult, (*iter)->expectedRes, (*iter)->ltlString, *pCounterExample) << endl;
    }
    delete inputs;
    inputs = NULL;
    ltl_input.close();
  }
  delete dict;
  dict = NULL;
}


bool SpotConnection::checkFormula(spot::tgba* ct_tgba, std::string ltl_string, spot::bdd_dict* dict, std::string** ce_ptr) {
 
  bool result;
  //cout<<"STATUS: parsing LTL: "<<ltl_string<<endl;
  negateFormula(ltl_string);
  spot::ltl::parse_error_list pel;
  const spot::ltl::formula* f = spot::ltl::parse(ltl_string, pel);
  if (spot::ltl::format_parse_errors(std::cerr, ltl_string, pel)) {
    f->destroy();						
    cerr<<"Error: ltl format error."<<endl;
    assert(0);
  }

  bool fm_exprop_opt = false;
  bool fm_symb_merge_opt = true;
  bool post_branching = false;
  bool fair_loop_approx = false;

  spot::tgba* formula_tgba = spot::ltl_to_tgba_fm(f, dict, fm_exprop_opt,
                         fm_symb_merge_opt, post_branching, fair_loop_approx);

  //cout<<"STATUS: computing product automaton."<<endl;
  spot::tgba_product product(formula_tgba,ct_tgba);

  //cout<<"STATUS: emptiness check."<<endl;
  spot::emptiness_check *ec= new spot::couvreur99_check(&product);
  spot::emptiness_check_result* ce = ec->check();
  if(ce) {   //a counterexample exists, original formula does not hold on the model
    result = false;
    spot::tgba_run* run = ce->accepting_run();
    if (run) {
      ostringstream runResult;
      spot::print_tgba_run(runResult, &product, run);
      //assign a string representation of the counter example if the corresponding out parameter is set
      if (ce_ptr) {
        std::string r = runResult.str();
        r = filterCounterexample(r, boolOptions["counterexamples-with-output"]);
        *ce_ptr = new string(r);//formatRun(r);	
      }
      delete run;
    }
    delete ce;
  } else {    //the product automaton defines the empty language, the formula holds on the given model
    result = true;
  }
  //cout<<"STATUS: deallocating ressources for formula tgba."<<endl;
  delete ec;
  dict->unregister_all_my_variables(formula_tgba);
  delete formula_tgba;
  f->destroy();

  return result;
}

spot::ltl::atomic_prop_set* SpotConnection::getAtomicProps(std::set<int> ioVals, int maxInputVal) {
  std::string ltl_props = "";
  bool firstEntry = true;
  for (std::set<int>::iterator i = ioVals.begin(); i != ioVals.end(); ++i) {
    if (!firstEntry)
      ltl_props += " & ";
    ltl_props += int2PropName(*i, maxInputVal);
    firstEntry = false;
  }
  spot::ltl::parse_error_list pel;
  const spot::ltl::formula* atomic_props = spot::ltl::parse(ltl_props, pel);
  if (spot::ltl::format_parse_errors(std::cerr, ltl_props, pel)) {
    atomic_props->destroy();						
    cerr<<"Error: ltl format error."<<endl;
    assert(0);
  }
  spot::ltl::atomic_prop_set* sap = spot::ltl::atomic_prop_collect(atomic_props);
  return sap;
}

std::string SpotConnection::comparison(bool spotRes, bool expectedRes, std::string& ltlFormula, std::string& ce) {
  std::string result;
  if (spotRes) {
    if (expectedRes) {
      result = "correct solution: true expected and model satisfies formula. Formula was " + ltlFormula;
    } else {
      result = "ERROR in solution: false expected but model satisfies formula. Formula was " + ltlFormula;
    }
  } else {
    if (!expectedRes) {
      result = "correct solution: false expected, model does not satisfy formula. Formula was " + ltlFormula; 
    } else {
      result = "ERROR in solution: true expected but counterexample exists. Formula was " + ltlFormula +"\n";
      result += "counterexample: \n";
      result += ce;
    }
  }
  return result;
}

std::list<std::string>* SpotConnection::loadFormulae(istream& input) {
  std::list<std::string>* result = new std::list<std::string>(); //DEBUG: FIXE ME (REMOVE)
  std::string line;
  int defaultPropertyNumber=0; //for RERS 2012 where no numbers were assigned in the properties.txt files
  bool explicitPropertyNumber = false;  //indicates whether or not an ID for the property could be extraced from the file
  LtlProperty* nextFormula = new LtlProperty();
  while (std::getline(input, line)){
  //basic assumption: there is a formula after each number "#X:" (X being an integer) before the next number occurs
    if (line.size() > 0 && line.at(0) == '#') {   // e.g. "#1:", the number by which the porperty is being refered to comes after '#'
      int endIndex = line.find_first_of (':', 0); //the ':' marks the end of the property number
      nextFormula->propertyNumber = boost::lexical_cast<int>(line.substr(1, (endIndex-1)));
      explicitPropertyNumber = true;
    } else if (line.size() > 0 && line.at(0) == '(') {   // '(' at column 0 indicates the beginning of a formula
      if (!explicitPropertyNumber) {  //enumerate those properties without any ID from 0 (therefore either all or non should have a number)
        nextFormula->propertyNumber = defaultPropertyNumber; 
        defaultPropertyNumber++;
      }
      nextFormula->ltlString = parseWeakUntil(line);
      behaviorProperties.push_back(*nextFormula);
      explicitPropertyNumber = false;
      nextFormula = new LtlProperty();
    }
    //ignore any other lines that neither contain a property number nor a formula
  }
  delete nextFormula;
  nextFormula = NULL;
  return result;   //DEBUG: FIXE ME (REMOVE)
}

ltlData* SpotConnection::parseSolutions(istream& input) {
  ltlData* result = new ltlData();
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
      } else {cout << "parse ERROR" << endl; assert(0);  }
      result->push_back(current);
    }
  }
  return result;
}

string& SpotConnection::parseWeakUntil(std::string& ltl_string) {
  int positionCharW;
  while ( (positionCharW = ltl_string.find("WU")) != -1) {
    ltl_string.erase((positionCharW +1), 1);  //delete the following "U" of "WU"
  }
  return ltl_string;
}

void SpotConnection::negateFormula(std::string& ltl_string) {
  ltl_string = "!( " + ltl_string + " )";
}

#if 0
// a more verbose version of the ouput would look like this:
std::string* SpotConnection::formatRun(string& run) {
  std::string* result = new std::string();
  //result->append("******************************\n");
  //result->append(run); //prints the entire run with all of SPOT's annotations
  //result->append("******************************\n");
  result->append("I/O run:\n");
  std::string* temp =  filter_run_IO_only(run);
  result->append( *temp ); result->append("\n");	
  delete temp;
  result->append("******************************\n");
  result->append("input run only:\n");
  temp =  filter_run_IO_only(run, true);
  result->append( *temp ); result->append("\n");
  delete temp;
  result->append("******************************\n");
  return result;
}
#endif

std::string* SpotConnection::formatRun(string& run) {
  std::string* result = new std::string();
  //result->append("******************************\n");
  //result->append(run); //prints the entire run with all of SPOT's annotations
  //result->append("******************************\n");
  //result->append("I/O run:\n");
  //std::string* temp =  filter_run_IO_only(run);
  //result->append( *temp ); result->append("\n");
  //delete temp;
  //result->append("******************************\n");
  //result->append("input run only:\n");
  std::string* temp =  filter_run_IO_only(run, true);
  result->append( *temp ); // result->append("\n");
  delete temp;
  //result->append("******************************\n");
  return result;
}

std::string SpotConnection::filterCounterexample(std::string spotRun, bool includeOutputStates) {
  // 1.) read in list of prefix props and cycle props
  std::list<std::string> prefix, cycle;
  std::istringstream run(spotRun);
  std::string result = "";	
  std::string line;
  enum PartOfRun {RUN_PREFIX, RUN_CYCLE, RUN_UNKNOWN};
  PartOfRun currentPart = RUN_UNKNOWN;
  while (std::getline(run, line)){
    //cout << "DEBUG: current line being parsed: " << line << endl;
    boost::trim(line);
    if (line.at(0) == 'P') {  //identify where the prefix of the run begins
      currentPart = RUN_PREFIX;
      //cout << "DEBUG: prefix detected. " << endl;
    } else if (line.at(0) == 'C') {  //identify where the cycle part of the run begins
      currentPart = RUN_CYCLE;
      //cout << "DEBUG: cycle detected. " << endl;
    } else if (line.at(0) == '|') {  //indicates a transition
      line = line.substr(1, (line.size()-1));	//cut off the '|' prefix
      boost::trim(line);
      vector<std::string> transitionAndAccSet; 
      boost::split(transitionAndAccSet, line, boost::is_any_of("{")); 
      line = *transitionAndAccSet.begin();       //cut off "{<acceptance-sets>}" suffix
      // iterate over all propositions in the transition and only add the non-negated one
      vector<std::string> props; 		
      boost::split_regex(props, line, boost::regex("( & )|(\\{)"));
      //cout << "DEBUG: props.size(): " << props.size()  << endl;
      vector<string>::iterator i;
      for(vector<string>::iterator i = props.begin(); i < props.end(); i++) {
        boost::trim(*i);
        if (i->at(0) != '!' && i->at(0) != 'A' && i->at(0) != ' ') {  //'A' indicates "{Acc[<num>]}" (acceptance set)
          if (currentPart == RUN_PREFIX) {
            prefix.push_back(*i);
          } else if (currentPart == RUN_CYCLE) {
            cycle.push_back(*i);
          } else {
            cout << "ERROR: could not reformat SPOT counterexample run. " << endl;
            assert(0);
          }
        }
      }			
    }
  }
  // 2.) remove output states from the returned counterexample if parameter "includeOutputStates" is false   
  std::list<std::string> returnedPrefix;
  std::list<std::string> returnedCycle;
    for (std::list<std::string>::iterator i = prefix.begin(); i != prefix.end() ; ++i) {
      if (i->at(0) == 'i' || (includeOutputStates && i->at(0) == 'o' )) { 
        returnedPrefix.push_back(*i);
        //cout << "DEBUG: added " << (*i) << " to returnedPrefix. " << endl;
      }
    }
    for (std::list<std::string>::iterator i = cycle.begin(); i != cycle.end() ; ++i) {
      if (i->at(0) == 'i' || (includeOutputStates && i->at(0) == 'o' )) { 
        returnedCycle.push_back(*i);
        //cout << "DEBUG: added " << (*i) << " to returnedCycle. " << endl;
      }
    }
  
  // 3.) concatenate and return both prefix and cycle as a formatted string
  result += "[";
  for (std::list<std::string>::iterator i = returnedPrefix.begin(); i != returnedPrefix.end() ; ++i) {
    if (i != returnedPrefix.begin()) {
      result += ";";
    }
    result += (*i);
  }
  result += "]";
  assert(returnedCycle.size() > 0); // empty cycle part in counterexample currently not supported
  result += "([";
  for (std::list<std::string>::iterator i = returnedCycle.begin(); i != returnedCycle.end() ; ++i) {
    if (i != returnedCycle.begin()) {
      result += ";";
    }
    result += (*i);
  }
  result += "])*";
  return result;
  //cout << "DEBUG: result in function: " << result  << endl;
}

// return a string representation of the I/O characters from the given spot run. 
// return only the input characters if parameter "inputOnly" is true.
std::string* SpotConnection::filter_run_IO_only(string& spotRun, bool inputOnly) {
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
      boost::split_regex(props, line, boost::regex("( & )|(\\{)"));
      vector<string>::iterator iter;
      for(iter = props.begin(); iter < props.end(); iter++) {
        boost::trim(*iter);
        if (iter->at(0) != '!' && iter->at(0) != 'A') {  //'A' stands for Acc (acceptance set)
          if (inputOnly) {
            //add only if the variable starts with 'i' (input)
            if (iter->at(0) == 'i') {
              *result += formatIOChar(*iter, firstEntry, cycleStart);
              firstEntry = false;
              cycleStart = false;
            }
          } else {  //input and output characters
            *result += formatIOChar(*iter, firstEntry, cycleStart);
            firstEntry = false;
            cycleStart = false;
          }
        }
      }			
    }			
  }
  *result += ")*";	
  return result;
};

std::string SpotConnection::formatIOChar(std::string prop, bool firstEntry, bool cycleStart) {
  std::string result;
  if (!firstEntry && !cycleStart){ 
    result = ", ";
  } else if (!firstEntry && cycleStart) {  
    result = "]("; 
  } else if (firstEntry && cycleStart) {  
    result = "(";
  } else {
    result = "[";
  }
  result += prop;
  return result;
}

std::string SpotConnection::int2PropName(int ioVal, int maxInputVal)  {
  std::string result;
  if (ioVal >maxInputVal && ioVal <= 26) {
    result = "o";  //an output variable follows (RERS mapping)
  } else if (ioVal >= 1 && ioVal <= maxInputVal) {
    result = "i";  //an input variable follows (RERS mapping)
  } else {
    cerr << "ERROR: input/output variable not recognized (not rers format)" << endl;
    assert(0);
  }
  char atomicProp = (char) (ioVal + ((int) 'A') - 1);
  result += boost::lexical_cast<string>(atomicProp);
  return result;
}
