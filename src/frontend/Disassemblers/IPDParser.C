/* Parser for the Instruction Partitioning Description (IPD) files. See documentation in Partitioner.h */
#include "sage3basic.h"
#include "Partitioner.h"

std::ostream&
operator<<(std::ostream &o, const Partitioner::IPDParser::Exception &e)
{
    return o <<e.format();
}

/* Output styles
 *     /foo/filename.ipd:15: error message      (all info is available)
 *     /foo/filename.ipd: error message         (missing line number)
 *     line 15: error message                   (missing file name)
 *     error message                            (missing file name and line number)
 *
 * The "error message" defaults to "IPD parse error"
 */
std::string
Partitioner::IPDParser::Exception::format() const 
{
    std::string retval = mesg.empty() ? "IPD parse error" : mesg;

    if (name.empty()) {
        if (lnum>0)
            retval = "line " + StringUtility::numberToString(lnum) + ": " + retval;
    } else if (lnum>0) {
        retval = name + ":" + StringUtility::numberToString(lnum) + ": " + retval;
    } else {
        retval = name + ": " + retval;
    }
    return retval;
}

void
Partitioner::IPDParser::parse()
{
    try {
        parse_File();
    } catch (Exception e) {
        if (e.name.empty())
            e.name = input_name;
        if (e.lnum==0) {
            e.lnum = 1;
            for (size_t i=0; i<at && i<len; i++) {
                if ('\n'==input[i])
                    e.lnum++;
            }
        }
        throw e;
    }
}

void
Partitioner::IPDParser::skip_space()
{
    do {
        while (at<len && isspace(input[at])) at++;
        if (at<len && '#'==input[at]) {
            while (at<len && '\n'!=input[at]) at++;
        }
    } while (at<len && isspace(input[at]));
}

bool
Partitioner::IPDParser::is_terminal(const char *to_match)
{
    skip_space();
    size_t n = strlen(to_match);
    if (n > len-at) return false;
    return !strncmp(input+at, to_match, n);
}

bool
Partitioner::IPDParser::is_symbol(const char *to_match)
{
    skip_space();
    size_t n = strlen(to_match);
    if (n > len-at) return false;
    return !strncmp(input+at, to_match, n) && (at+n>=len || !isalnum(input[at+n]));
}

bool
Partitioner::IPDParser::is_string()
{
    skip_space();
    return at<len && '"'==input[at];
}

bool
Partitioner::IPDParser::is_number()
{
    skip_space();
    return at<len && isdigit(input[at]);
}

void
Partitioner::IPDParser::match_terminal(const char *to_match)
{
    if (!is_terminal(to_match))
        throw Exception(std::string("expected terminal \"") + to_match + "\"");
    at += strlen(to_match);
}

void
Partitioner::IPDParser::match_symbol(const char *to_match)
{
    if (!is_symbol(to_match))
        throw Exception("expected symbol");
    at += strlen(to_match);
}

std::string
Partitioner::IPDParser::match_symbol()
{
    skip_space();
    if (at>=len || !isalpha(input[at]))
        throw Exception("expected symbol");
    std::string retval;
    while (at<len && isalnum(input[at]))
        retval += input[at++];
    return retval;
}

std::string
Partitioner::IPDParser::match_string()
{
    if (!is_string())
        throw Exception("expected string");
    at++; /*quote*/
    std::string retval;
    while (at>=len || '"'!=input[at]) {
        if (at<len && '\\'==input[at]) at++;
        if (at>=len)
            throw Exception("unterminated string");
        retval += input[at++];
    }
    at++; /*quote*/
    return retval;
}

rose_addr_t
Partitioner::IPDParser::match_number()
{
    if (!is_number())
        throw Exception("expected number");
    char *rest;
#ifdef _MSC_VER
		//tps - added Win specific function
    rose_addr_t retval = _strtoui64(input+at, &rest, 0);
#else
	rose_addr_t retval = strtoull(input+at, &rest, 0);
#endif
    at = rest-input;
    return retval;
}

bool
Partitioner::IPDParser::parse_File()
{
    while (parse_Declaration()) /*void*/;
    skip_space();
    if (at<len)
        throw Exception("expected declaration");
    return true;
}

bool
Partitioner::IPDParser::parse_Declaration()
{
    return parse_FuncDecl() || parse_BlockDecl();
}

bool
Partitioner::IPDParser::parse_FuncDecl()
{
    if (!is_symbol("function")) return false;
    match_symbol("function");
    rose_addr_t entry_va = match_number();
    std::string name = is_string() ? match_string() : "";
    try {
        cur_func = partitioner->add_function(entry_va, SgAsmFunctionDeclaration::FUNC_USERDEF, name);
        parse_FuncBody();
        cur_func = NULL;
    } catch (const Exception&) {
        cur_func = NULL;
        throw;
    }
    return true;
}

bool
Partitioner::IPDParser::parse_FuncBody()
{
    if (!is_terminal("{")) return false;
    match_terminal("{");
    parse_FuncStmtList();
    match_terminal("}");
    return true;
}

bool
Partitioner::IPDParser::parse_FuncStmtList()
{
    do {
        while (is_terminal(";")) match_terminal(";");
    } while (parse_FuncStmt());
    return true;
}

bool
Partitioner::IPDParser::parse_FuncStmt()
{
    return parse_BlockDecl() || parse_ReturnSpec();
}

bool
Partitioner::IPDParser::parse_ReturnSpec()
{
    if (is_symbol("return") || is_symbol("returns")) {
        match_symbol();
        cur_func->returns = true;
        return true;
    }
    if (is_symbol("noreturn")) {
        match_symbol("noreturn");
        cur_func->returns = false;
        return true;
    }
    return false;
}

bool
Partitioner::IPDParser::parse_BlockDecl()
{
    using namespace StringUtility;

    if (!is_symbol("block")) return false;
    match_symbol("block");
    rose_addr_t va = match_number();
    size_t ninsns = match_number();

    if (partitioner->block_config.find(va)!=partitioner->block_config.end())
        throw Exception("multiple definitions for basic block at " + addrToString(va));
    cur_block = new BlockConfig;
    cur_block->ninsns = ninsns;
    partitioner->block_config.insert(std::make_pair(va, cur_block));
    parse_BlockBody();
    return true;
}

bool
Partitioner::IPDParser::parse_BlockBody()
{
    if (!is_terminal("{")) return false;
    match_terminal("{");
    parse_BlockStmtList();
    match_terminal("}");
    return true;
}

bool
Partitioner::IPDParser::parse_BlockStmtList()
{
    do {
        while (is_terminal(";")) match_terminal(";");
    } while (parse_BlockStmt());
    return true;
}

bool
Partitioner::IPDParser::parse_BlockStmt()
{
    return parse_Alias() || parse_Successors();
}

bool
Partitioner::IPDParser::parse_Alias()
{
    if (!is_symbol("alias")) return false;
    match_symbol("alias");
    rose_addr_t alias_va = match_number();
    cur_block->alias_for = alias_va;
    return true;
}

bool
Partitioner::IPDParser::parse_Successors()
{
    if (!is_symbol("successor") && !is_symbol("successors")) return false;
    match_symbol();
    cur_block->sucs_specified = true;
    cur_block->sucs.clear();
    cur_block->sucs_complete = true;

    while (is_number()) {
        rose_addr_t succ_va = match_number();
        cur_block->sucs.insert(succ_va);
        if (!is_terminal(",")) break;
        match_terminal(",");
    }

    if (is_terminal("...")) {
        match_terminal("...");
        cur_block->sucs_complete = false;
    }

    return true;
}

