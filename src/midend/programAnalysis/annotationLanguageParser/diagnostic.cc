
#include "diagnostic.h"

using namespace std;

#define contain(L,x) (find(L.begin(), L.end(), x)!=L.end())

class Diagnostic::Segment {
private:
  list<Location*> locations;
  int _id;
  Segments _succs, _preds; // successor Segment's
  memoryBlock *_flow_insensitive_property;

  static int id_count;
  static Segment * _broken;

  Segment(list<Location*> locs) : locations(locs), _id(++id_count),
                                  _flow_insensitive_property(NULL) {}
  Segment() : _id(++id_count), _flow_insensitive_property(NULL) {}

public:
  Segment(memoryBlock *property)
  : _id(++id_count), _flow_insensitive_property(property) {}

  inline int id() const { return _id; }
  inline Segments succs() const { return _succs; }
  inline Segments preds() const { return _preds; }
  inline memoryBlock *flow_insensitive_property() const
  { return _flow_insensitive_property; }

  static Segment * findSegment(list<Location*> locs, Segments & existing) {
    for(Segments::iterator e=existing.begin(); e!=existing.end(); e++)
      if((*e)->locations == locs) return *e;
    Segment *s = new Segment(locs);
    existing.push_back(s);
    return s;
  }

  static Segment * brokenSegment() {
    if(! _broken) _broken = new Segment();
    return _broken;
  }

  void print_path(ostream &out);

  inline void add_succ(Segment *s) {
    if(! contain(_succs, s)) {
      _succs.push_back(s);
      s->_preds.push_back(this);
    }
  }
  inline void remove_succ(Segment *s){ _succs.remove(s);s->_preds.remove(this);}
  inline void remove_pred(Segment *p){ _preds.remove(p);p->_succs.remove(this);}

  static void reset() { Segment::_broken = NULL; }
};
int Diagnostic::Segment::id_count = 0;
Diagnostic::Segment * Diagnostic::Segment::_broken = 0;


void Diagnostic::trace(ostream & out, Location *loc, pointerValue &pv) {
  // Performs "backward" tracing from loc. The property value of interest is in pv.

  assert(loc);
out << ">> begin trace " << *loc << endl;

  bool any_trace = false;
  for (memoryblock_set_p r=pv.blocks.begin(); r!=pv.blocks.end(); ++r) {
    memoryBlock * property_block = _property->lookup_property_block(*r);
    if (property_block) {
      // only perform tracing on proproty block that contains dianostic value.
      enumvalue_set values = _property->construct_now_value(loc, property_block,
                                                            false);
      if(! contain_diagnostic_value(values)) continue;

out << "      property_block " << property_block << " " << property_block->name() << endl;
      // trace starts at the nearest definition of the property block.
      if(! property_block->is_flow_sensitive()) {
        cout << "      -> flow-insensitive, trace not performed.\n";
        continue;
      }
      memoryDef *def = property_block->nearest_def_at(loc);
      if(! def) {
        memorydef_list defs = property_block->defs();
        cout << "      -> no nearest def, #defs=" << defs.size() << ":\n";
        for(memorydef_list_p d=defs.begin(); d!=defs.end(); d++)
          cout << * d->location << endl;
      } else {
        any_trace = true;
        trace(loc, def); // begin backward trace (recursive)
        // segment_path.clear(); // reset
      }
    }

    // note: tracing is repeated on all property blocks, and their results
    // (traces) are not distinguished. If separate traces are desired,
    // note the following:
    // 1. each property may generate more than one path in the trace.
    // 2. Not all the paths actually involve the property_block because
    //    the corresponding real-block is not involved. Ie. some paths
    //    need to be pruned.
  }

  // detect and remove cycles
  int cycles_deleted = 0;
  for(Segments::iterator s=starts.begin(); s!=starts.end(); s++) {
    Segments visited;
    cycles_deleted += detect_cycle(*s, visited);
  }
  cout << "deleted " << cycles_deleted << " cycles (estimated?)\n";

  // erase segments not in any path from start to end
  int segments_deleted = 0;
  Segments worklist = segments;
  for(Segments::iterator n=worklist.begin(); n!=worklist.end();
      n=worklist.begin()) {
    Segment *N = *n;
    worklist.remove(N);
    if(!contain(starts,N) && N->preds().empty()) {
      segments.remove(N);
      Segments succs = N->succs();
      for(Segments::iterator s=succs.begin(); s!=succs.end(); s++) {
        (*s)->remove_pred(N);
        if((*s)->preds().empty()) worklist.push_back(*s);
      }
      delete N;
      segments_deleted++;
    }
  }
  cout << "deleted " << segments_deleted << " segments\n";

  // detect shortest path
  Segments shortest;
  for(Segments::iterator s=starts.begin(); s!=starts.end(); s++) {
    Segments visited;
    find_shortest(*s, visited, shortest);
  }
  cout << "shortest: ";
  for(Segments::iterator n=shortest.begin(); n!=shortest.end(); n++)
    cout << (*n)->id() << ",";
  shortest.clear();
  for(Segments::iterator s=starts.begin(); s!=starts.end(); s++) {
    if(*s == Segment::brokenSegment() ||
       (*s)->flow_insensitive_property())
      continue;
    Segments visited;
    find_shortest(*s, visited, shortest);
  }
  cout << "\nshortest without-break: ";
  for(Segments::iterator n=shortest.begin(); n!=shortest.end(); n++)
    cout << (*n)->id() << ",";
  cout << endl;

  // result: print path segments. First print the start segments.
  if(any_trace) {
    assert(! starts.empty());
    out << "{start} --> ";
    for(Segments::iterator s=starts.begin(); s!=starts.end(); s++) {
      if(s!=starts.begin()) out << ", ";
      out << "{" << (*s)->id() << "}";
    }
    out << endl;
    // now print all segments. Due to the way Segments are pushed into "path",
    // the print order is topological order.
    for(Segments::iterator p=segments.begin(); p!=segments.end(); p++) {
      (*p)->print_path(out);
      if(! (*p)->succs().empty()) {
        out << "  --> ";
        Segments succs = (*p)->succs();
        for(Segments::iterator s=succs.begin(); s!=succs.end(); s++) {
          if(s!=succs.begin()) out << ", ";
          out << "{" << (*s)->id() << "}";
        }
      }
      out << endl;
    }
    segment_path.clear(); // reset
  }

  // reset data structure used in the traces.
  for(Segments::iterator p=segments.begin(); p!=segments.end(); p++)
    delete *p;
  segments.clear();  starts.clear();
  Segment::reset();

out << ">> end trace " << *loc << "\n\n";
} // trace


Diagnostic::Segment *Diagnostic::trace(Location *from, memoryDef *def) {
  // Perform backward trace on a segment starting from "from" to the location
  // at the "def".  This function is recursive.
  if(! def) {
    Segment *s = Segment::brokenSegment();
    if(!contain(segments,s)) segments.push_back(s);
    if(!contain(starts,s)) starts.push_back(s);
    return s;
  }
  Location *step = def->where(); // location of definition.
  assert(step);
  if(from == step) return NULL;
  Location_pair lp(from,def);
  if(segment_path[lp])
    return segment_path[lp];

/*cout << "trace from " << from << " " << *from <<" step "<< *step
     <<" def "<< def << " owner=" << def->owner() << "-" <<
     def->owner()->name() << endl;*/

  /* if(step->kind() == Location::BasicBlock) {
    cout << *step << endl;
    cout << "from @" << *from << endl;
  }
  assert(step->kind() == Location::Statement ||
         step->kind() == Location::Procedure); // ?? */
  // step = step->parent()->parent(); // else obtain Statement from Procedure

  if(_def2uses.find(step) == _def2uses.end()) { // end of trace
    // assert(false);  // not reachable ?
    Segment *s = segment_path[lp] = trace(from, step);
    if(s) {
      if(!contain(segments,s)) segments.push_back(s);
      if(!contain(starts, s))  starts.push_back(s);
    }
// exit(1);
    return s;
  }

  // verify: _def2uses on step should contain def.
  if(_def2uses[step].find(def) != _def2uses[step].end()) ;
  else if(! def->owner()->is_flow_sensitive()) {
    Segment *s = segment_path[lp] = new Segment(def->owner());
    if(!contain(starts, s)) starts.push_back(s);
    if(!contain(segments, s)) segments.push_back(s);
    return s;
  } else {
    cout << "step " << step << " " << *step << endl;
    cout << "from " << *from << endl;
    cout << "def=" << def << " owner=" << def->owner() << "-"
         << def->owner()->name() << endl;
    memoryBlock *owner = def->owner();
    cout << "write-protected? " << owner->write_protected()
         << owner->container()->write_protected() << endl;
    cout << "other in _def2uses:\n";
    for(def2uses::iterator m=_def2uses[step].begin();
        m!=_def2uses[step].end(); m++) {
      cout << "   " << m->first << " " << *m->first->where() << " owner="
           << m->first->owner() << "-" << m->first->owner()->name() << endl;
    }
    assert(false);
  }

  // obtain a Segment "step --> .. --> from".
  Segment *current_segment = segment_path[lp] = trace(from, step);
  assert(current_segment);

  // the uses on the rhs.
  memoryuse_set uses = _def2uses[step][def];
  if(uses.empty()) {
    if(!contain(starts, current_segment))
      starts.push_back(current_segment); // end of trace
  }
/*else
cout << "#uses=" << uses.size() << endl;*/
  
  // for each use, trace back (recurse).
  for(memoryuse_set_p u=uses.begin(); u!=uses.end(); u++) {
    memoryDef *def1 = (*u)->reaching_def();
/*cout << "def " << def << " use " << *u << " def " << def1 << " use@"
     << * (*u)->where() << endl;*/
/*if(!def1) {
  cout << "null def1, use @" << * (*u)->where() << " owner "
       << (*u)->owner()->name() << endl;
  (*u)->print(cout);
}*/
if(def1 && def1->owner() != (*u)->owner()) {
  cout << "def1's owner " << def1->owner()->name() << endl;
  cout << "use's owner " << (*u)->owner()->name() << endl;
  assert(def1->owner() == (*u)->owner());
}
/*{
memoryBlock *owner = def->owner();
memoryBlock *property = _property->lookup_property_block(owner);
assert(property);
// memoryUse *old_use = property->current_use();
property->set_current_def_use(step);
enumvalue_set val = _property->construct_now_value(step, property, true);
assert(contain_diagnostic_value(val));
// property->current_use(old_use);
} */
    Segment *prev_segment = trace(step, def1); // recursion
    // remember that current_segment is successor of prev_segment.
    if(prev_segment) prev_segment->add_succ(current_segment);
  }

  // add current segment to segments. Do this here after recursion above, so
  // that Segment's in segments is topological-order.
  if(!contain(segments,current_segment)) segments.push_back(current_segment);
  return current_segment;
} // trace


Diagnostic::Segment *Diagnostic::trace(Location *from, Location *to) {
  // obtain a Segment "to --> ... --> from".
  // how: each "from","to" is basically a context, such as
  // "a --> b --> ... -- > c". First, prune away common prefix. If they
  // result in "f1->...->fn" and "t1->...->tn", the desired Segment is
  // "tn->...->t1->f1->...->fn".

  assert(from && to);
  list<Location*> l_from, l_to, result;
  // break down the Location's of the two contexts.
  Location *p = from;
  while(p) {
    l_from.push_front(p);
    p = p->parent();
  }
  p = to;
  while(p) {
    l_to.push_front(p);
    p = p->parent();
  }

  // prune away common prefix.
  while(l_from.front() == l_to.front()) {
    l_from.pop_front();
    l_to.pop_front();
    if(l_from.empty() || l_to.empty()) break;
  }

  // result part 1: the reverse order from l_to.
  for(list<Location*>::reverse_iterator l=l_to.rbegin(); l!=l_to.rend(); l++)
    result.push_back(*l);

  // result part 2: append l_from to result.
  for(list<Location*>::iterator l=l_from.begin(); l!=l_from.end(); l++)
    result.push_back(*l);
  // result.merge(l_from);

  if(result.empty()) return NULL;
  return Segment::findSegment(result, segments);
} // trace(Location)


void Diagnostic::Segment::print_path(ostream &out) {
  if(this == _broken) {
    out << "{" << id() << "} broken path (use without def)";
    return;
  }

  if(_flow_insensitive_property) {
    out << "}" << id() << "} broken path (flow-insensitive property "
        << _flow_insensitive_property->name() << ")";
    return;
  }

#define print_location \
    switch((*l)->kind()) { \
      case Location::Statement: out << ((stmtLocation*)*l)->stmt()->coord(); break; \
      case Location::BasicBlock: { \
        basicblockLocation *bb = (basicblockLocation*)*l; \
        out << bb->proc_location()->proc()->decl()->name() \
            << ':' << bb->block()->dfn(); \
        break; \
      } \
      case Location::Procedure: out << ((procLocation*)*l)->proc()->decl()->name(); \
    }

  out << "{" << id() << "} ";
  for(list<Location*>::iterator l=locations.begin(); l!=locations.end(); l++) {
    if(l!=locations.begin()) out << " --> ";
    print_location
  }
} // trace(Location)



int Diagnostic::detect_cycle(Segment *cur, Segments & visited, bool specific) {
  if(contain(visited,cur)) {
    if(specific) {
      Segment *first = visited.front();
      for(Segments::iterator v=visited.begin(); v!=visited.end(); ) {
        Segment *now = *v;
        v++;
        if(v==visited.end())
          now->remove_succ(first);
        else
          now->remove_succ(*v);
      }
      return 1;
    }
    Segments visited1;
    return detect_cycle(cur, visited1, true);
  }
  visited.push_back(cur);
  Segments succs = cur->succs();
  int cycles_deleted = 0;
  for(Segments::iterator s=succs.begin(); s!=succs.end(); s++)
    cycles_deleted += detect_cycle(*s, visited, specific);
  if(specific) visited.remove(cur);
  return cycles_deleted;
} // detect_cycle


void Diagnostic::find_shortest(Segment *cur, Segments path, Segments &shortest){
  assert(!contain(path,cur));
  path.push_back(cur);
  Segments succs = cur->succs();
  if(succs.empty()) {
    if(path.size() < shortest.size() || shortest.empty())
      shortest = path;
    return;
  }
  for(Segments::iterator s=succs.begin(); s!=succs.end(); s++)
    find_shortest(*s, path, shortest);
} // find_shortest

