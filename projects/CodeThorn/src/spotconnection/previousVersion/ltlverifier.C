
// argv[1]: ltl-string
// argv[2]: stg-file-name

#include <iostream>
#include <fstream>
#include <string>

#include "ltlparse/public.hh"
#include "ltlvisit/destroy.hh"

#include <iostream>
#include <cassert>
#include <cstdlib>
#include "tgba/tgbaexplicit.hh"
#include "tgba/tgbaproduct.hh"
#include "tgbaparse/public.hh"
#include "tgbaalgos/save.hh"
#include "ltlast/allnodes.hh"
#include "tgbaalgos/scc.hh"
#include "tgbaalgos/cutscc.hh"
#include "ltlparse/ltlfile.hh"
#include "tgbaalgos/ltl2tgba_fm.hh"
#include "misc/timer.hh"
#include "ltlast/atomic_prop.hh"
#include "ltlvisit/apcollect.hh"
#include "ltlenv/environment.hh"
#include "ltlparse/public.hh"
#include "tgbaalgos/ltl2tgba_fm.hh"
#include "tgba/tgbaproduct.hh"
#include "tgbaalgos/gtec/gtec.hh"

using namespace std;

int main(int argc, char* argv[]) {
  std::string ltl_string;
  ltl_string = argv[1];

  std::string stg_filename;
  stg_filename=argv[2]; 

  cout<<"STATUS: parsing LTL: "<<ltl_string<<endl;
  spot::ltl::parse_error_list pel;
  const spot::ltl::formula* f = spot::ltl::parse(ltl_string, pel);
  if (spot::ltl::format_parse_errors(std::cerr, ltl_string, pel)) {
    //spot::ltl::destroy(f);
    cerr<<"Error: ltl format error."<<endl;
    return 2;
  }
  spot::ltl::atomic_prop_set* sap = spot::ltl::atomic_prop_collect(f);


  cout<<"STATUS: reading STG from file: "<<stg_filename<<endl;
  spot::bdd_dict* dict = new spot::bdd_dict();
  spot::ltl::environment& env(spot::ltl::default_environment::instance());
  spot::tgba_parse_error_list pel2;
  spot::tgba_explicit_string* a2 = spot::tgba_parse(stg_filename.c_str(), pel2, dict, env);
  if (spot::format_tgba_parse_errors(std::cerr, stg_filename.c_str(), pel2)) {
    cerr<<"Error: stg format error."<<endl;
    return 1;
  }

  bool ce_expected = false;

  bool fm_exprop_opt = false;
  bool fm_symb_merge_opt = true;
  bool post_branching = false;
  bool fair_loop_approx = false;
  spot::bdd_dict* dict_ltl=new spot::bdd_dict();
  //spot::ltl::atomic_prop_set* sap = spot::ltl::atomic_prop_collect(f);
  spot::tgba* formula_tgba = spot::ltl_to_tgba_fm(f, dict, fm_exprop_opt,
                         fm_symb_merge_opt, post_branching, fair_loop_approx);

  cout<<"STATUS: computing product."<<endl;
  spot::tgba_product p(formula_tgba,a2);
  //spot::tgba_save_reachable(std::cout, &p);
  //spot::ltl::atomic_prop_set* sap = spot::ltl::atomic_prop_collect(f);
  //pn_tgba p(stg, sap, &dict);


    //model_check(n, f, ce_expected, fm_exprop_opt, fm_symb_merge_opt,
    //            post_branching, fair_loop_approx);
    //spot::ltl::destroy(f);

  //spot::tgba_parse() 

  cout<<"STATUS: emptiness check."<<endl;
  spot::emptiness_check *ec= new spot::couvreur99_check(&p);
  spot::emptiness_check_result* res = ec->check();
  ec->print_stats(std::cout);

  cout<<"STATUS: reporting result."<<endl;
  if (res) {
    std::cout << "YES (an accepting run exists)" << std::endl;
#if 0
    spot::tgba_run* run = res->accepting_run();
    if (run) {
      spot::print_tgba_run(std::cout, &p, run);
      delete run;
    }
#endif
    delete res;
  } else {
    std::cout << "NO (no accepting run found)" << std::endl;
  }

#if 0
  cout<<"STATUS: deallocating ressources."<<endl;
  delete ec;
  delete a2;
  delete dict;
  delete dict_ltl;
#endif
  //delete f;
  //delete formula_tgba;

  cout<<"STATUS: finished."<<endl;
  return 0;
}
