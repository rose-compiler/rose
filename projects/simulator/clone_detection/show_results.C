// Shows clone detection results.  The only reason we have this is because SQLite3 insists that all integers should be printed
// in decimal form, but it's more convenient to see function entry addresses in hexadecimal.

#include "rose.h"
#include "sqlite3x.h"
using namespace sqlite3x;

static void
usage(const std::string &arg0, int exit_status)
{
    size_t slash = arg0.rfind('/');
    std::string basename = slash==std::string::npos ? arg0 : arg0.substr(slash+1);
    if (0==basename.substr(0, 3).compare("lt-"))
        basename = basename.substr(3);
    std::cerr <<"usage: " <<basename <<" DATABASE_NAME\n";
    exit(exit_status);
}

struct Row {
    int cluster_id, function_id;
    uint64_t entry_va;
    std::string funcname, filename;
    Row(int cluster_id, int function_id, uint64_t entry_va, const std::string &funcname, const std::string &filename)
        : cluster_id(cluster_id), function_id(function_id), entry_va(entry_va), funcname(funcname), filename(filename) {}
};

typedef std::vector<Row> Rows;

int
main(int argc, char *argv[])
{
    if (argc!=2 || '-'==argv[1][0])
        usage(argv[0], 1);

    // Open the database and suck in the information we need
    sqlite3_connection db(argv[1]);
    sqlite3_command cmd1(db, "select a.cluster_id, b.id, b.entry_va, b.funcname, b.filename"
                         " from combined_clusters a"
                         " join semantic_functions b on a.func_id = b.id"
                         " order by a.cluster_id, a.func_id");
    sqlite3_reader cursor = cmd1.executereader();
    Rows rows;
    while (cursor.read())
        rows.push_back(Row(cursor.getint(0), cursor.getint(1), cursor.getint64(2), cursor.getstring(3), cursor.getstring(4)));

    // Column headers
    std::vector<std::string> headers(5, "");
    headers[0] = "cluster_id";
    headers[1] = "function_id";
    headers[2] = "entry_va";
    headers[3] = "funcname";
    headers[4] = "filename";

    // Figure out how wide to make the columns
    std::vector<int> width(5);
    for (size_t i=0; i<5; ++i)
        width[i] = headers[i].size();
    char buf[64];
    int n;
    for (size_t i=0; i<rows.size(); ++i) {
        n = snprintf(buf, sizeof buf, "%d", rows[i].cluster_id);
        width[0] = std::max(width[0], n);
        n = snprintf(buf, sizeof buf, "%d", rows[i].function_id);
        width[1] = std::max(width[1], n);
        n = snprintf(buf, sizeof buf, "0x%08"PRIx64, rows[i].entry_va);
        width[2] = std::max(width[2], n);
        width[3] = std::max(width[3], (int)rows[i].funcname.size());
        width[4] = std::max(width[4], (int)rows[i].filename.size());
    }

    // Print everything
    for (size_t i=0; i<5; ++i)
        std::cout <<(i>0?" ":"") <<std::setw(width[i]) <<headers[i];
    std::cout <<"\n";
    for (size_t i=0; i<5; ++i)
        std::cout <<(i>0?" ":"") <<std::string(width[i], '-');
    std::cout <<"\n";
    for (size_t i=0; i<rows.size(); ++i) {
        snprintf(buf, sizeof buf, "0x%08"PRIx64, rows[i].entry_va);
        buf[sizeof(buf)-1] = '\0';
        std::cout <<std::setw(width[0]) <<std::right <<rows[i].cluster_id <<" "
                  <<std::setw(width[1]) <<std::right <<rows[i].function_id <<" "
                  <<std::setw(width[2]) <<std::right <<buf <<" "
                  <<std::setw(width[3]) <<std::left  <<rows[i].funcname <<" "
                  <<std::setw(width[4]) <<std::left  <<rows[i].filename <<"\n";
    }

    return 0;
}
