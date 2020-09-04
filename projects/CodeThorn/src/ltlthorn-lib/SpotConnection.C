#include "sage3basic.h"
#include "SpotConnection.h"
#include "CodeThornCommandLineOptions.h"

using namespace CodeThorn;
using namespace std;

#include "rose_config.h"
#ifdef HAVE_SPOT

SpotConnection::SpotConnection() {};

//constructors with automatic initialization
SpotConnection::SpotConnection(std::string ltl_formulae_file) { init(ltl_formulae_file); }
SpotConnection::SpotConnection(std::list<std::string> ltl_formulae) { init(ltl_formulae); }

void SpotConnection::init(std::string ltl_formulae_file) {
  //open text file that contains the properties
  ifstream ltl_input(ltl_formulae_file.c_str());
  if (ltl_input.is_open()) {
    ltlResults = new PropertyValueTable();
    //load the containing formulae
    loadFormulae(ltl_input);  //load the formulae into class member "ltlResults"
  } else {
    cerr<<"Error: could not open file "<<ltl_formulae_file<<endl;
    exit(1);
  }
}

void SpotConnection::init(std::list<std::string> ltl_formulae) {
  ltlResults = new PropertyValueTable();
  int propertyId = 0;
  for (list<string>::iterator i=ltl_formulae.begin(); i!=ltl_formulae.end(); i++) {
    ltlResults->addProperty(*i, propertyId);
    propertyId++;
  }
}

PropertyValueTable* SpotConnection::getLtlResults() {
  if (ltlResults) {
    return ltlResults; 
  } else {
    cerr<< "ERROR: LTL results requested even though the SpotConnection has not been initialized yet." << endl;
    assert(0);
  }
}

void SpotConnection::resetLtlResults() { 
  ROSE_ASSERT(ltlResults);
  ltlResults->init(ltlResults->size()); 
}

void SpotConnection::resetLtlResults(int property) { 
  ROSE_ASSERT(ltlResults);
  ltlResults->setPropertyValue(property, PROPERTY_VALUE_UNKNOWN);
  ltlResults->setCounterexample(property, "");
}

void SpotConnection::setModeLTLDriven(bool ltlDriven) {
  modeLTLDriven=ltlDriven;
}
void SpotConnection::checkSingleProperty(int propertyNum, TransitionGraph& stg, 
						LtlRersMapping ltlRersMapping, bool withCounterexample, bool spuriousNoAnswers) {
  if (stg.size() == 0 && !modeLTLDriven) {
    cout << "STATUS: the transition system used as a model is empty, LTL behavior could not be checked." << endl;
    return;
  }
  if (!stg.isPrecise() && !stg.isComplete()) {
    return;  //neither falsification nor verification works
  } 

  //initializing the atomic propositions used based on I/O values
  spot::ltl::atomic_prop_set* sap = getAtomicProps(ltlRersMapping);

  //instantiate a new dictionary for atomic propositions 
  // (will be used by the model tgba as well as by the ltl formula tgbas)
  spot::bdd_dict dict;
  //create a tgba from CodeThorn's STG model
  SpotTgba* ct_tgba = new SpotTgba(stg, *sap, dict, ltlRersMapping.getInputValueSet(), ltlRersMapping.getOutputValueSet());
  LtlProperty ltlProperty; 
  ltlProperty.ltlString = ltlResults->getFormula(propertyNum);
  ltlProperty.propertyNumber = propertyNum;
  checkAndUpdateResults(ltlProperty, ct_tgba, stg, withCounterexample, spuriousNoAnswers);
  delete ct_tgba;
  ct_tgba = NULL;
}

PropertyValue SpotConnection::checkPropertyParPro(string ltlProperty, ParProTransitionGraph& stg, set<string> annotationsOfModeledTransitions) {
  if (!stg.isPrecise() && !stg.isComplete()) {
    return PROPERTY_VALUE_UNKNOWN;  //neither falsification nor verification works
  } 
  PropertyValue result;
  spot::ltl::atomic_prop_set* sap = getAtomicProps(ltlProperty);
  // for an over-approximation, all atomic propositions need to be modeled in the stg in order to analyze if the property holds
  if (stg.isComplete() && !stg.isPrecise()) {
    for (spot::ltl::atomic_prop_set::iterator i=sap->begin(); i!=sap->end(); ++i) {
      if (annotationsOfModeledTransitions.find((*i)->name()) == annotationsOfModeledTransitions.end()) {
	return PROPERTY_VALUE_UNKNOWN;
      }
    }
  }
  //instantiate a new dictionary for atomic propositions 
  // (will be used by the model tgba as well as by the ltl formula tgbas)
  spot::bdd_dict dict;
  //create a tgba from CodeThorn's STG model
  ParProSpotTgba* ct_tgba = new ParProSpotTgba(stg, *sap, dict);
  bool formulaHolds = checkFormula(ct_tgba, ltlProperty, ct_tgba->get_dict());
  delete ct_tgba;
  ct_tgba = NULL;
  if (formulaHolds) {
    if (stg.isComplete()) {
      result = PROPERTY_VALUE_YES;
    } else {
      result = PROPERTY_VALUE_UNKNOWN;
    }
  } else {
    if (stg.isPrecise()) {
      result = PROPERTY_VALUE_NO;
    } else {
      result = PROPERTY_VALUE_UNKNOWN;
    }
  }
  return result;
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
                                        LtlRersMapping ltlRersMapping, bool withCounterexample, bool spuriousNoAnswers) {
  if (stg.size() == 0 && !modeLTLDriven) {
    cout << "STATUS: the transition system used as a model is empty, LTL behavior cannot be checked." << endl;
    return;
  }
  if (!stg.isPrecise() && !stg.isComplete()) {
    cout << "STATUS: neither falsification nor verification possible (STG is not precise and not complete)." << endl;
    return;  //neither falsification nor verification works
  } else {  //prepare the analysis

    //initializing the atomic propositions used based on I/O values
    spot::ltl::atomic_prop_set* sap = getAtomicProps(ltlRersMapping);
    //instantiate a new dictionary for atomic propositions 
    // (will be used by the model tgba as well as by the ltl formula tgbas)
    spot::bdd_dict dict;
    //create a tgba from CodeThorn's STG model
    SpotTgba* ct_tgba = new SpotTgba(stg, *sap, dict, ltlRersMapping.getInputValueSet(), ltlRersMapping.getOutputValueSet());
    std::string* pCounterExample; 
    ROSE_ASSERT(ltlResults);
    std::list<int>* yetToEvaluate = ltlResults->getPropertyNumbers(PROPERTY_VALUE_UNKNOWN);
    for (std::list<int>::iterator i = yetToEvaluate->begin(); i != yetToEvaluate->end(); ++i) {
      if (modeLTLDriven && args.getBool("reset-analyzer")) {
	stg.getAnalyzer()->resetAnalysis();
	cout << "STATUS: Analyzer reset successful, now checking LTL property " << *i << "." << endl;
      }
      if (checkFormula(ct_tgba, ltlResults->getFormula(*i), ct_tgba->get_dict(), &pCounterExample)) {  //SPOT returns that the formula could be verified
        if (stg.isComplete()) {
          ltlResults->strictUpdatePropertyValue(*i, PROPERTY_VALUE_YES);
        } else {
          //not all possible execution paths are covered in this stg model, ignore SPOT's result
        }
      } else {  //SPOT returns that there exists a counterexample that falsifies the formula
        if (stg.isPrecise()) {
          ltlResults->strictUpdatePropertyValue(*i, PROPERTY_VALUE_NO);
          if (withCounterexample) {
            ltlResults->strictUpdateCounterexample(*i, *pCounterExample);
          }
          delete pCounterExample;
        } else {
          // old: the stg is over-approximated, falsification cannot work. Ignore SPOT's answer.
          if (spuriousNoAnswers) {
            // new: register counterexample and check it later
            ltlResults->strictUpdatePropertyValue(*i, PROPERTY_VALUE_NO);
            if (withCounterexample) {
              ltlResults->strictUpdateCounterexample(*i, *pCounterExample);
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

ParProSpotTgba* SpotConnection::toTgba(ParProTransitionGraph& stg) {
  // retrieve all atomic propositions found in the given LTL propeties 
  spot::ltl::atomic_prop_set* sap = getAtomicProps();
  //instantiate a new dictionary for atomic propositions 
  spot::bdd_dict* dict = new spot::bdd_dict();
  //create a tgba from CodeThorn's STG model
  ParProSpotTgba* ct_tgba = new ParProSpotTgba(stg, *sap, *dict);
  return ct_tgba;
}

void SpotConnection::checkLtlPropertiesParPro(ParProTransitionGraph& stg, bool withCounterexample, bool spuriousNoAnswers, set<string> annotationsOfModeledTransitions) {
  if (stg.size() == 0 && !modeLTLDriven) {
    cout << "STATUS: the transition system used as a model is empty, LTL behavior could not be checked." << endl;
    return;
  }
  if (!stg.isPrecise() && !stg.isComplete()) {
    return;  //neither falsification nor verification works
  } else { 
    // retrieve all atomic propositions found in the given LTL propeties 
    spot::ltl::atomic_prop_set* sap = getAtomicProps();
    //instantiate a new dictionary for atomic propositions 
    // (will be used by the model tgba as well as by the ltl formula tgbas)
    spot::bdd_dict dict;
    //create a tgba from CodeThorn's STG model
    ParProSpotTgba* ct_tgba = new ParProSpotTgba(stg, *sap, dict);
    std::string* pCounterExample; 
    std::list<int>* yetToEvaluate = ltlResults->getPropertyNumbers(PROPERTY_VALUE_UNKNOWN);
    for (std::list<int>::iterator i = yetToEvaluate->begin(); i != yetToEvaluate->end(); ++i) {
      if (checkFormula(ct_tgba, ltlResults->getFormula(*i), ct_tgba->get_dict(), &pCounterExample)) {  //SPOT returns that the formula could be verified
        if (stg.isComplete()) {
	  bool resultCanBeTrusted = true;
	  if (!stg.isPrecise()) {
	    // for an over-approximation, all atomic propositions need to be modeled in the stg in order to analyze if the property holds
	    spot::ltl::atomic_prop_set* sapFormula = getAtomicProps(ltlResults->getFormula(*i));
	    for (spot::ltl::atomic_prop_set::iterator k=sapFormula->begin(); k!=sapFormula->end(); ++k) {
	      if (annotationsOfModeledTransitions.find((*k)->name()) == annotationsOfModeledTransitions.end()) {
		resultCanBeTrusted = false;
		break;
	      }
	    }
	  }
	  if (resultCanBeTrusted) {
	    ltlResults->strictUpdatePropertyValue(*i, PROPERTY_VALUE_YES);
	  } else {
	    ltlResults->strictUpdatePropertyValue(*i, PROPERTY_VALUE_UNKNOWN);
	  }
        } else {
          //not all possible execution paths are covered in this stg model, ignore SPOT's result
        }
      } else {  //SPOT returns that there exists a counterexample that falsifies the formula
        if (stg.isPrecise()) {
          ltlResults->strictUpdatePropertyValue(*i, PROPERTY_VALUE_NO);
          if (withCounterexample) {
            ltlResults->strictUpdateCounterexample(*i, *pCounterExample);
          }
          delete pCounterExample;
        } else {
          // old: the stg is over-approximated, falsification cannot work. Ignore SPOT's answer.
          if (spuriousNoAnswers) {
            // new: register counterexample and check it later
            ltlResults->strictUpdatePropertyValue(*i, PROPERTY_VALUE_NO);
            if (withCounterexample) {
              ltlResults->strictUpdateCounterexample(*i, *pCounterExample);
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
    //assign a string representation of the counterexample if the corresponding out parameter is set
    if (ce_ptr) {
      spot::tgba_run* run = ce->accepting_run();
      if (run) {
	ostringstream runResult;
	spot::print_tgba_run(runResult, &product, run);
	//assign a string representation of the counterexample if the corresponding out parameter is set
	std::string r = runResult.str();
	r = filterCounterexample(r, args.getBool("counterexamples-with-output"));
	*ce_ptr = new string(r);//formatRun(r);	
	delete run;
      } else {
	cerr << "ERROR: SPOT says a counterexample exist but no accepting run could be returned." << endl;
	ROSE_ASSERT(0);
      }
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

spot::ltl::atomic_prop_set* SpotConnection::getAtomicProps() {
  spot::ltl::atomic_prop_set* result = new spot::ltl::atomic_prop_set();
  std::list<int>* propertyNumbers = ltlResults->getPropertyNumbers();
  for (std::list<int>::iterator i=propertyNumbers->begin(); i!=propertyNumbers->end(); ++i) {
    std::string formulaString = ltlResults->getFormula(*i);
    spot::ltl::atomic_prop_set* sap = getAtomicProps(formulaString);
    result->insert(sap->begin(), sap->end());
    delete sap;
    sap = NULL;
  } 
  delete propertyNumbers;
  propertyNumbers = NULL;
  return result;
}

spot::ltl::atomic_prop_set* SpotConnection::getAtomicProps(string ltlFormula) {
  spot::ltl::atomic_prop_set* result = new spot::ltl::atomic_prop_set();
  spot::ltl::parse_error_list pel;
  const spot::ltl::formula* formula = spot::ltl::parse(ltlFormula, pel);
  if (spot::ltl::format_parse_errors(std::cerr, ltlFormula, pel)) {
    formula->destroy();						
    cerr<<"Error: ltl format error."<<endl;
    ROSE_ASSERT(0);
  }
  spot::ltl::atomic_prop_set* sap = spot::ltl::atomic_prop_collect(formula);
  result->insert(sap->begin(), sap->end());
  delete sap;
  sap = NULL;
  return result;
}

// TODO 3
spot::ltl::atomic_prop_set* SpotConnection::getAtomicProps(LtlRersMapping ltlRersMapping) {
  std::set<int> ioVals=ltlRersMapping.getInputOutputValueSet();
  std::string ltl_props = "";
  bool firstEntry = true;
  for (std::set<int>::iterator i = ioVals.begin(); i != ioVals.end(); ++i) {
    if (!firstEntry)
      ltl_props += " & ";
    ltl_props += ltlRersMapping.getIOString(*i); // e.g. iA
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

std::list<std::string>* SpotConnection::loadFormulae(istream& input) {
  std::list<std::string>* result = new std::list<std::string>(); //DEBUG: FIXE ME (REMOVE)
  std::string line;
  int defaultPropertyNumber=0; //for RERS 2012 because no numbers were assigned in the properties.txt files
  bool explicitPropertyNumber = false;  //indicates whether or not an ID for the property could be extraced from the file
  LtlProperty* nextFormula = new LtlProperty();
  while (std::getline(input, line)){
    //basic assumption: there is a formula after each number "#X:" (X being an integer) before the next number occurs
    // e.g. "#1:", the number by which the porperty is being refered to comes after '#'
    if (line.size() > 0 && line.at(0) == '#' && line.find_first_of(':') != string::npos) {  
      int endIndex = line.find_first_of (':', 0); //the ':' marks the end of the property number
      nextFormula->propertyNumber = boost::lexical_cast<int>(line.substr(1, (endIndex-1)));
      explicitPropertyNumber = true;
    } else if (line.size() > 0 && line.at(0) == '(') {   // '(' at column 0 indicates the beginning of a formula
      if (!explicitPropertyNumber) {  //enumerate those properties without any ID from 0 (therefore either all or non should have a number)
        nextFormula->propertyNumber = defaultPropertyNumber; 
        defaultPropertyNumber++;
      }
      nextFormula->ltlString = parseWeakUntil(line);
      ltlResults->addProperty(nextFormula->ltlString, nextFormula->propertyNumber);
      explicitPropertyNumber = false;
      nextFormula = new LtlProperty();
    } else if (line.size()==0) {
      // empty line found (no spaces, only '\n')
    } else if (line.size()>=2 && line.at(0) == '#' && line.at(1)=='i') {
      // found inputs line (must match inputs array content)
      cout<<"LTL PARSER: detected inputs line: "<<line<<endl;
    } else if (line.size()>=2 && line.at(0) == '#' && line.at(1)=='o') {
      // found inputs line
      cout<<"LTL PARSER: detected outputs line: "<<line<<endl;
    } else {
      // other lines that neither contain a property number nor a formula nor inputs nor outputs
      cerr<<"Error: LTL Property parsing failed. Unknown input: "<<line<<endl;
      exit(1);
    }
  }
  delete nextFormula;
  nextFormula = NULL;
  return result;   //DEBUG: FIXE ME (REMOVE)
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
  if (returnedCycle.size() > 0) {
    result += "([";
    for (std::list<std::string>::iterator i = returnedCycle.begin(); i != returnedCycle.end() ; ++i) {
      if (i != returnedCycle.begin()) {
	result += ";";
      }
      result += (*i);
    }
    result += "])*";
  }
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

std::string SpotConnection::spinSyntax(std::string ltlFormula) {
  spot::ltl::parse_error_list pel;
  const spot::ltl::formula* formula = spot::ltl::parse(ltlFormula, pel);
  if (spot::ltl::format_parse_errors(std::cerr, ltlFormula, pel)) {
    formula->destroy();						
    cerr<<"Error: ltl format error."<<endl;
    ROSE_ASSERT(0);
  }
  bool prefixAtomicPropositions = true;
  if (prefixAtomicPropositions) {
    spot::ltl::relabeling_map relabeling;
    spot::ltl::atomic_prop_set* sap = spot::ltl::atomic_prop_collect(formula);
    for (spot::ltl::atomic_prop_set::iterator i=sap->begin(); i!=sap->end(); i++) {
      string newName = "p_" + (*i)->name();
      const spot::ltl::atomic_prop* relabeledProp = spot::ltl::atomic_prop::instance(newName, (*i)->env());
      relabeling[*i] = relabeledProp;
    }
    formula = spot::ltl::relabel(formula, spot::ltl::Pnn, &relabeling);
  }

  string result = spot::ltl::to_spin_string(formula);
  formula->destroy();
  return result;
}

set<string> SpotConnection::atomicPropositions(string ltlFormula) {
  set<string> result;
  spot::ltl::atomic_prop_set* sapFormula = getAtomicProps(ltlFormula);
  for (spot::ltl::atomic_prop_set::iterator k=sapFormula->begin(); k!=sapFormula->end(); ++k) {
    result.insert((*k)->name());
  }
  return result;
}

#else

SpotConnection::SpotConnection() {  reportUndefinedFunction(); };
SpotConnection::SpotConnection(std::string ltl_formulae_file) {  reportUndefinedFunction(); }
SpotConnection::SpotConnection(std::list<std::string> ltl_formulae) { reportUndefinedFunction(); }

void SpotConnection::init(std::string ltl_formulae_file) {
  reportUndefinedFunction();
}

void SpotConnection::init(std::list<std::string> ltl_formulae) {
  reportUndefinedFunction();
}

PropertyValueTable* SpotConnection::getLtlResults() {
  reportUndefinedFunction();
  return nullptr;
}

void SpotConnection::resetLtlResults() { 
  reportUndefinedFunction();
}

void SpotConnection::resetLtlResults(int property) { 
  reportUndefinedFunction();
}

void SpotConnection::setModeLTLDriven(bool ltlDriven) {
  reportUndefinedFunction();
}
void SpotConnection::checkSingleProperty(int propertyNum, TransitionGraph& stg, 
						LtlRersMapping ltlRersMapping, bool withCounterexample, bool spuriousNoAnswers) {
  reportUndefinedFunction();
}

PropertyValue SpotConnection::checkPropertyParPro(string ltlProperty, ParProTransitionGraph& stg, set<string> annotationsOfModeledTransitions) {
  reportUndefinedFunction();
  PropertyValue v=PROPERTY_VALUE_UNKNOWN;
  return v;
}

void SpotConnection::checkLtlProperties(TransitionGraph& stg,
						LtlRersMapping ltlRersMapping, bool withCounterexample, bool spuriousNoAnswers) {
  reportUndefinedFunction();
}

ParProSpotTgba* SpotConnection::toTgba(ParProTransitionGraph& stg) {
  reportUndefinedFunction();
  return nullptr;
}

void SpotConnection::checkLtlPropertiesParPro(ParProTransitionGraph& stg, bool withCounterexample, bool spuriousNoAnswers, set<string> annotationsOfModeledTransitions) {
  reportUndefinedFunction();
}

std::string SpotConnection::spinSyntax(std::string ltlFormula) {
  reportUndefinedFunction();
  string s;
  return s;
}

set<string> SpotConnection::atomicPropositions(string ltlFormula) {
  reportUndefinedFunction();
  set<string> s;
  return s;
}

void SpotConnection::reportUndefinedFunction() {
  throw CodeThorn::Exception("Error: Called a function of class SpotConnection even though CodeThorn was compiled without SPOT.");
}

#endif


