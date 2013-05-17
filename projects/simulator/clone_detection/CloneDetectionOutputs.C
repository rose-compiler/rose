#include "CloneDetectionOutputs.h"

#include <sstream>

namespace CloneDetection {

using namespace sqlite3x;

bool
OutputGroup::operator==(const OutputGroup &other) const
{
    return (values.size()==other.values.size() &&
            std::equal(values.begin(), values.end(), other.values.begin()) &&
            callees_va.size()==other.callees_va.size() &&
            std::equal(callees_va.begin(), callees_va.end(), other.callees_va.begin()) &&
            syscalls.size()==other.syscalls.size() &&
            std::equal(syscalls.begin(), syscalls.end(), other.syscalls.begin()) &&
            fault == other.fault);
}

void
OutputGroup::print(std::ostream &o, const std::string &title, const std::string &prefix) const
{
    if (!title.empty())
        o <<title <<"\n";
    for (size_t i=0; i<values.size(); ++i)
        o <<prefix <<"value " <<values[i] <<"\n";
    for (size_t i=0; i<callees_va.size(); ++i)
        o <<prefix <<"fcall " <<callees_va[i] <<"\n";
    for (size_t i=0; i<syscalls.size(); ++i)
        o <<prefix <<"scall " <<syscalls[i] <<"\n";
    if (fault)
        o <<prefix <<AnalysisFault::fault_name(fault) <<"\n";
}

void
OutputGroup::print(RTS_Message *m, const std::string &title, const std::string &prefix) const
{
    if (m && m->get_file()) {
        std::ostringstream ss;
        print(ss, title, prefix);
        m->mesg("%s", ss.str().c_str());
    }
}

void
load_output_groups(sqlite3_connection &db, const IdVa *func_id2va/*NULL*/, OutputGroups &output_groups/*out*/)
{
    output_groups.clear();
    sqlite3_command cmd2(db, "select id, pos, val, vtype from semantic_outputvalues order by id, pos");
    sqlite3_reader cursor = cmd2.executereader();
    while (cursor.read()) {
        size_t id = cursor.getint(0);
        int pos = cursor.getint(1);
        assert(pos>=0);
        int value = cursor.getint(2);
        std::string vtype= cursor.getstring(3);
        assert(!vtype.empty());
        if (id>=output_groups.size())
            output_groups.resize(id+1);
        switch (vtype[0]) {
            case 'V':
                if ((size_t)pos>=output_groups[id].values.size())
                    output_groups[id].values.resize(pos+1, 0);
                output_groups[id].values[pos] = value;
                break;
            case 'F':
                assert(output_groups[id].fault == AnalysisFault::NONE);
                output_groups[id].fault = (AnalysisFault::Fault)value;
                break;
            case 'C': {
                // convert function ID to an entry VA if the caller supplied a func_id2va map
                if ((size_t)pos>=output_groups[id].callees_va.size())
                    output_groups[id].callees_va.resize(pos+1, 0);
                if (func_id2va) {
                    IdVa::const_iterator found = func_id2va->find(value);
                    assert(found!=func_id2va->end());
                    value = found->second;
                }
                output_groups[id].callees_va[pos] = value;
                break;
            }
            case 'S':
                if ((size_t)pos>=output_groups[id].syscalls.size())
                    output_groups[id].syscalls.resize(pos+1, 0);
                output_groups[id].syscalls[pos] = value;
                break;
            default:
                assert(!"invalid output value type");
                abort();
        }
    }
}


} // namespace
