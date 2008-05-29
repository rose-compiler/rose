
#include "adaptor_statistics.h"

using namespace std;

void Adaptor_Statistics::make_CS(memoryBlock *block, procedureInfo *proc, 
                                 Location *site,
                                 memoryBlock::DestructiveKind reason) {
  assert(block && proc);
  cs_procs.insert(proc);
  ReasonKind reason_kind;
  switch(reason) {
    case memoryBlock::Control_flow:
      reason_kind = Control_flow;
      break;
    case memoryBlock::Parameter_pass:
      reason_kind = Parameter_pass;
      break;
    case memoryBlock::Weak_update:
      reason_kind = Weak_update;
      break;
    case memoryBlock::Additive:
      reason_kind = Additive;
      break;
    default: assert(false);
  }
  cs_reasons[proc] = (ReasonKind) (cs_reasons[proc] | reason_kind);
  cs_block_reasons[ CS_Block(proc,block) ] =
    (ReasonKind) (cs_block_reasons[ CS_Block(proc,block) ] | reason_kind);
  cs_blocks[proc].insert(block);
  cs_destructions[block].insert(site);
} // make_CS


void Adaptor_Statistics::make_CS(memoryBlock *block, procedureInfo *proc, 
                                 Location *site,
                                 memoryBlock *complicit) {
  assert(block && proc && complicit);
  cs_procs.insert(proc);
  cs_reasons[proc] = (ReasonKind) (cs_reasons[proc] | Complicit);
  cs_block_reasons[ CS_Block(proc,block) ] =
    (ReasonKind) (cs_block_reasons[ CS_Block(proc,block) ] | Complicit);
  cs_blocks[proc].insert(block);
  cs_complicits[block].insert(Complicit_assign(site,complicit));
} // make_CS (complicit)


/*void Adaptor_Statistics::reset() {
  fs_blocks.clear();
  fs_reasons.clear();
  fs_destructions.clear();
  fs_complicits.clear();

  cs_procs.clear();
  cs_reasons.clear();
  cs_blocks.clear();
  fs_destructions.clear();
  cs_complicits.clear();

  cf_blocks.clear();
  cf_sites.clear();
} // reset */

//////////////////////////////////////////////////////////////////////

void Adaptor_Statistics::print_statistics(ostream & out) {
  out << "----------------------------------------------------------------\n";
  print_unify_statistics(out);
  out << "----------------------------------------------------------------\n";
  print_cs_statistics(out);
  out << "----------------------------------------------------------------\n";
  print_fs_statistics(out);
  out << "----------------------------------------------------------------\n";
  print_cf_statistics(out);
  out << "----------------------------------------------------------------\n";
} // print_statistics


void Adaptor_Statistics::print_cs_statistics(ostream & out) {
  out << "Adapt CS statistics:\n";

  for(procedureinfo_set_p p=cs_procs.begin(); p!=cs_procs.end(); p++) {
    procNode *proc = (*p)->proc();
    out << "+procedure " << proc->decl()->name() << " at " << proc->coord()
        << " " << reason_str(cs_reasons[*p]) << ":\n";
    if(_ann->lookup_procedure(proc->decl()->name())) {
      out << " (annotated)\n";
      continue;
    }

    int cg_depth_min, cg_depth_max, cg_height_min, cg_height_max;
    (*p)->get_cg_statistics(cg_depth_min, cg_depth_max, cg_height_min,
                            cg_height_max);
    const procedureInfo::callsite_map & callsites = (*p)->callsites();
    int total_callsites = callsites.size();
    set<procedureInfo*> unique_callers;
    for(procedureInfo::callsite_map_cp cs=callsites.begin();
        cs!=callsites.end(); cs++)
      unique_callers.insert(cs->second);
    out << " cg depth=" << cg_depth_min << "/" << cg_depth_max
        << " height=" << cg_height_min << "/" << cg_height_max
        << " recursion=" << (*p)->is_recursive() << " #callsites="
        << unique_callers.size() << "/" << total_callsites
        << " calls=" << (*p)->calls().size() << "/\n";

    decl_list args = ((funcNode*) proc->decl()->type())->args();
    set<int>  destructive_args;
    memoryblock_set blocks = cs_blocks[*p],
                    printed;
    procedureInfo::callsite_map callsites_bak = (*p)->callsites();

    for(memoryblock_set_p b=blocks.begin(); b!=blocks.end(); b++) {
      int i=1;
      decl_list_p arg;
      for(arg=args.begin(); arg!=args.end(); arg++, i++) {
        if(*arg == (*b)->decl()) break;
        if((*b)->property || (*b)->container()) {
          memoryBlock *block = *b;
          while(block) {
            assert(! block->is_unify()); // TBD
            if(*arg == block->decl()) break;
            block = block->container();
          }
          if(block) break;
        }
      }
      if(i <= args.size()) {
        out << "..(argument " << i << ")\n";
        destructive_args.insert(i);
      }
      print_block_chain(*b, cs_destructions, cs_complicits, printed,
                        "  ", "..", out);

      // check out sites with Destructive reason
      CS_Block pb(*p,*b);
      if(cs_destructions.find(*b) != cs_destructions.end() &&
         ((cs_block_reasons[pb] & Parameter_pass) != 0 ||
          (cs_block_reasons[pb] & Additive) != 0) /* possible ??? */ ) {
        assert( i <= args.size() ); // b must be an argument?
        for(Locations::iterator loc = cs_destructions[*b].begin();
            loc != cs_destructions[*b].end(); loc++) {
          if((*loc)->kind() == Location::Procedure) {
            out << " +at Procedure. self-assignment?" << endl;
            continue;
          }
          assert(false); // all destructions must be self-assignments?
          assert((*loc)->kind() == Location::Statement); // ???
          stmtLocation *stmt_loc = (stmtLocation*) *loc;
          assert(stmt_loc->stmt()->typ() == ThreeAddr);
          assert(callsites.find(stmt_loc) != callsites.end());
          procedureInfo *caller = callsites.find(stmt_loc)->second;
          callsites_bak.erase(stmt_loc);

          print_call_args(stmt_loc, caller, *p, i, *arg, out);
        }
      }
    }

    if(! callsites_bak.empty()) {
      out << "..Call sites with unknown status (non-bottom values?):\n";
      for(procedureInfo::callsite_map_cp cs=callsites_bak.begin();
          cs!=callsites_bak.end(); cs++) {
        stmtLocation *stmt_loc = cs->first;
        procedureInfo *caller = cs->second;
        int i=1;
        for(decl_list_p arg=args.begin(); arg!=args.end(); arg++, i++)
          if(destructive_args.find(i) != destructive_args.end()) {
            out << "  (argument " << i << ")\n";
            print_call_args(stmt_loc, caller, *p, i, *arg, out);
          }
      }
    }

    if(proc_site_property.find(*p) != proc_site_property.end()) {
      out << "..Call site partitions:\n";
      map<int,int_list> partition_sizes;

      // re-group, sort according to Arg_Property_Pair
      Site_Partition site_partition;
      for(Site_Property::iterator sp=proc_site_property[*p].begin();
          sp!=proc_site_property[*p].end(); sp++)
        for(Arg_Property::iterator ap=sp->second.begin();
            ap!=sp->second.end(); ap++)
          site_partition[ Arg_Property_Pair(ap->first,ap->second) ]
            .insert(sp->first);

      string summary_str;
      for(Site_Partition::iterator part=site_partition.begin();
          part!=site_partition.end(); part++) {
        ostringstream ost;
        ost << "<" << part->first.first.first << ","
            << part->first.first.second << "|";
        for(enumvalue_sets::iterator v=part->first.second.begin();
            v!=part->first.second.end(); v++) {
          if(v != part->first.second.begin()) ost << ",";
          ost << *v;
        }
        ost << ">: [" << part->second.size() << "]";
        summary_str += ost.str() + ", ";
        ost << " ";
        for(stmtLocations::iterator s=part->second.begin();
            s!=part->second.end(); s++) {
          if(s != part->second.begin()) ost << ",";
          ost << **s;
        }
        int deref = part->first.first.second;
        partition_sizes[deref].push_back(part->second.size());
        out << "  - " << ost.str() << endl;
      }

      out << "  Call site partitions summary: " << proc->decl()->name()
          << " " << total_callsites << " -> ";
          /*
      for(map<int,int_list>::iterator s=partition_sizes.begin();
          s!=partition_sizes.end(); s++) {
        if(s != partition_sizes.begin()) out << ", ";
        out << "[" << s->first << "] ";
        for(int_list_p s1=s->second.begin(); s1!=s->second.end(); s1++) {
          if(s1 != s->second.begin()) out << ",";
          out << *s1;
        }
      }*/
      out << summary_str << endl;
    }
  }
} //print_cs_statistics


void Adaptor_Statistics::print_call_args(stmtLocation *stmt_loc,
                                         procedureInfo *caller,
                                         procedureInfo *callee,
                                         int arg,
                                         declNode *formal,
                                         ostream & out) {
  out << "  +Call site @ " << *stmt_loc << endl;

  threeAddrNode *callsite = (threeAddrNode*) stmt_loc->stmt();
  assert(callsite->op() && callsite->op()->id() == Operator::FUNC_CALL);

  operand_list_p parameter;
  int a = arg;
  for(parameter=callsite->arg_list().begin();
      a-->1 && parameter!=callsite->arg_list().end(); parameter++) ;
  if(parameter == callsite->arg_list().end()) return;

  bool ellipsis = (_analyzer->is_va_list(formal) ||
                   formal->type() && formal->type()->is_ellipsis());

  do {
    pointerValue parameter_value;
    memoryblock_set defs, changes;
    memoryuse_set uses;
    bool never_returns;
    _analyzer->eval(caller, stmt_loc, *parameter, stmt_loc->stmt(), defs,
                    uses, changes, parameter_value, never_returns);

    print_call_arg(formal, arg, *parameter, 0, parameter_value, stmt_loc,
                   caller, callee, _ann->enum_properties(), out);

    if(ellipsis) parameter++;
  } while(ellipsis && parameter != callsite->arg_list().end());
}

void Adaptor_Statistics::print_call_arg(declNode *formal,
                                        int arg,
                                        operandNode *parameter,
                                        int dereference,
                                        pointerValue & parameter_value,
                                        stmtLocation *stmt_loc,
                                        procedureInfo *caller,
                                        procedureInfo *callee,
	                                      const enum_property_map & props,
                                        ostream & out) {
  if(parameter_value.blocks.empty()) return;
  bool printed_b = false;
  if(! parameter_value.is_address)
    for(memoryblock_set_p b=parameter_value.blocks.begin();
        b!=parameter_value.blocks.end(); b++) {

      bool printed_property = false;
      for (enum_property_map_cp p = props.begin(); p != props.end(); ++p) {
        enumPropertyAnn * property = (*p).second;
        memoryBlock * property_block = property->lookup_property_block(*b);
        if(property_block) {
          if(! printed_b) {
            out << "  - parameter ";
            int d = dereference;
            while(d--) out << "*";
            out << formal->name() << " (=\"";
            output_context oc(out);
            parameter->output(oc,NULL);
            out << "\")\n";
            printed_b = true;
          }

          out << "    - value = " << (*b)->name() << " " << *b << " "
              << property->name() << "=";
          enumvalue_set values = property->construct_now_value(stmt_loc,
                                                               property_block,
                                                               false);
          out << property->to_string(values) <<" ("<< values.to_ulong() << ") ";
          printed_property = true;

          int_pair i(arg,dereference);
          proc_site_property[callee][stmt_loc][i].insert(values.to_ulong());
        }
      }
      if(printed_property) out << endl;
    }

  pointerValue star;
  memoryblock_set defs, changes;
  memoryuse_set uses;
  _analyzer->star_operator(caller, stmt_loc, parameter_value, defs, uses,
                           changes, star);
  // somewhere, it is possible that star could be the same as parameter_values.
  // check this to prevent infinite recursion.
  if(parameter_value.blocks == star.blocks) return;
  print_call_arg(formal, arg, parameter, dereference+1, star, stmt_loc, caller,
                 callee, props, out);
} //print_call_arg



void Adaptor_Statistics::print_fs_statistics(ostream & out) {
  out << "Adapt FS statistics: (TBD)\n";
} //print_fs_statistics

void Adaptor_Statistics::print_cf_statistics(ostream & out) {
  out << "Adapt control-flow statistics: (TBD)\n";
} //print_cf_statistics

void Adaptor_Statistics::print_unify_statistics(ostream & out) {
  out << "Adapt unify statistics: (TBD)\n";
} //print_unify_statistics


void Adaptor_Statistics::print_block_chain(memoryBlock *block,
                                           Destructions & destructions,
                                           Complicits & complicits,
                                           memoryblock_set & printed,
                                           string indent,
                                           string block_indent,
                                           ostream & out) {

  out << block_indent << "Block " << block->name() << " " << block << endl;
  if(printed.find(block) != printed.end()) {
    out << indent << "(printed)\n";
    return;
  }
  printed.insert(block);

  if(destructions.find(block) != destructions.end()) {
    out << indent << "destructions: ";
    const memoryBlock::destructive_assignment_map & assigns
      = block->destructive_assignments();
    for(Locations::iterator loc=destructions[block].begin();
        loc!=destructions[block].end(); loc++) {
      assert(assigns.find(*loc) != assigns.end());
      memoryBlock::DestructiveKind r = assigns.find(*loc)->second;
      switch(r) {
        case memoryBlock::Parameter_pass:
          out << " parameter pass"; break;
        case memoryBlock::Weak_update:
          out << " multiplicity weak update"; break;
        case memoryBlock::Additive:
          out << " flow-insensitive assignment"; break;
        default: out << " -- ERROR unknown cause" << r << endl;
          assert(false);
      }
      out << " @" << **loc << ", ";
    }
    out << endl;
  }

  if(complicits.find(block) != complicits.end()) {
    out << indent << "complicits:\n";
    indent += " ";
    if((block_indent.length()+1) % 5 == 0)
      block_indent += "|";
    else
      block_indent += ".";

    for(Complicit_assigns::iterator a=complicits[block].begin();
        a!=complicits[block].end(); a++) {
      out << block_indent << "at " << *a->first << endl;
      print_block_chain(a->second, destructions, complicits, printed,
                        indent, block_indent, out);
    }
  }
} // print_block_chain


//////////////////////////////////////////////////////////////////////


string Adaptor_Statistics::reason_str(ReasonKind r) {
  assert(r != 0);
  string s = "(";
  bool one = false;
  if(r & Control_flow) {
    if(one) s+="+";
    s += "Control_flow";
    one = true;
  }
  if(r & Parameter_pass) {
    if(one) s+="+";
    s += "Parameter_pass";
    one = true;
  }
  if(r & Weak_update ) {
    if(one) s+="+";
    s += "Weak_update";
    one = true;
  }
  if(r & Additive ) {
    if(one) s+="+";
    s += "Additive";
    one = true;
  }
  if(r & Complicit) {
    if(one) s+="+";
    s += "Complicit";
    one = true;
  }
  s += ")";
  return s;
}

