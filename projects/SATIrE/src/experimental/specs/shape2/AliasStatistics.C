#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

#ifndef ALIASSTATISTICS_H
#define ALIASSTATISTICS_H

class AliasStatistics {
    public: enum AliasCounter {
        GRAPH = 0,
        PRE_NO,
        PRE_MAY,
        PRE_MUST,
        POST_NO,
        POST_MAY,
        POST_MUST,
        COUNTER_MAX
    };

    private:
        int expr_pair_count;
        vector<int> counter[COUNTER_MAX];
        static const char* counter_label[];
  
    public:
        AliasStatistics() { reset() ; }
       ~AliasStatistics() {}

        void setExprPairCount(int n)      { expr_pair_count = n; }

        void addGraphCount(int n)      { counter[GRAPH].push_back(n); }
        void addPreMayAliasCount(int n)   { counter[PRE_MAY].push_back(n);   }
        void addPreMustAliasCount(int n)  { counter[PRE_MUST].push_back(n);  }
        void addPostMayAliasCount(int n)   { counter[POST_MAY].push_back(n);   }
        void addPostMustAliasCount(int n)  { counter[POST_MUST].push_back(n);  }
        
    public: void reset() {
        int i;
        for (i=0; i<COUNTER_MAX; i++) {
            counter[i].clear();
        }
        expr_pair_count = -1;
    }
      
    public: string to_s() {
        stringstream str;
        vector<int>::const_iterator it;

        assert(counter[GRAPH].size() == counter[PRE_MAY].size() &&
               counter[PRE_MAY].size() == counter[PRE_MUST].size() &&
               counter[PRE_MUST].size() == counter[POST_MAY].size() &&
               counter[POST_MAY].size() == counter[POST_MUST].size());
        
        // compute NO_ALIAS counter
        for (it=counter[PRE_MAY].begin(); it!=counter[PRE_MAY].end(); it++) {
            counter[PRE_NO].push_back(expr_pair_count - *it);
        }
        for (it=counter[POST_MAY].begin(); it!=counter[POST_MAY].end(); it++) {
            counter[POST_NO].push_back(expr_pair_count - *it);
        }

        str <<
            "--Alias Statistics--\n" <<
            to_s_listline(GRAPH)     <<
            to_s_sumline(GRAPH)      <<
            "count(statements): "    << counter[GRAPH].size() << "\n" <<
            "\n" <<
            to_s_listline(PRE_NO)    <<
            to_s_listline(PRE_MAY)   <<
            to_s_listline(PRE_MUST)  <<
            "\n" <<
            to_s_sumline(PRE_NO)     <<
            to_s_sumline(PRE_MAY)    <<
            to_s_sumline(PRE_MUST)   <<
            "\n" <<
            to_s_avgline(PRE_NO)     <<
            to_s_avgline(PRE_MAY)    <<
            to_s_avgline(PRE_MUST)   <<
            "\n" <<
            to_s_listline(POST_NO)   <<
            to_s_listline(POST_MAY)  <<
            to_s_listline(POST_MUST) <<
            "\n" <<
            to_s_sumline(POST_NO)    <<
            to_s_sumline(POST_MAY)   <<
            to_s_sumline(POST_MUST)  <<
            "\n" <<
            to_s_avgline(POST_NO)    <<
            to_s_avgline(POST_MAY)   <<
            to_s_avgline(POST_MUST)  <<
            std::endl;

        return str.str();
    }


    private: string to_s_avgline(enum AliasCounter id) {
        stringstream str;
        str << "avg(" << counter_label[id] << "): ";

        if (counter[GRAPH].size() > 0) {
            str <<
                ((double)computeSum(id)) / ((double)(counter[GRAPH].size())) << "\n";
        } else {
            str <<
                "0\n";
        }
        return str.str();
    }

    private: string to_s_sumline(enum AliasCounter id) {
        stringstream str;
        str << "sum(" << counter_label[id] << "): " << computeSum(id) << "\n";
        return str.str();
    }


    private: string to_s_listline(enum AliasCounter id) {
        vector<int>::const_iterator it;
        stringstream str;
        str << counter_label[id] << ":";
        for (it=counter[id].begin(); it!=counter[id].end(); it++) {
            str << " " << *it;
        }
        str << "\n";
        return str.str();
    }

    private: int computeSum(enum AliasCounter id) {
        vector<int>::const_iterator it;
        int sum = 0;
        for (it=counter[id].begin(); it!=counter[id].end(); it++) {
            sum += *it; 
        }
        return sum;
    }

    public: void writeFile(const char *fn) {
        ofstream out;
        out.open(fn);
        if (out) {
            out << to_s();
            out.close();
        }
    }
};

const char * AliasStatistics::counter_label[] = {
    "nnh_graphs",
    "pre no_alias",
    "pre may_alias",
    "pre must_alias",
    "post no_alias",
    "post may_alias",
    "post must_alias",
};

#endif
