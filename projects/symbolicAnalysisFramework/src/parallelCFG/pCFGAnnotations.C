#include "pCFGAnnotations.h"

string pcfgSplitAnnotation::toString()
{
    ostringstream _str;
    _str << "[" << size << " : ";

    for(int i = 0; i < size; i++) {
        _str << names[i] << ", ";
    }
    _str << "]" << endl;
    return _str.str();
}

string pcfgMatchAnnotation::toString()
{
    ostringstream ostr;
    ostr << "[source : " << source.first << ", index : " << source.second << " ";
    ostr << "target : " << target.first << ", index : " << target.second << "]";
    return ostr.str();
}

// match a string separted by delim
string pcfgAnnotationParser::match_any_string_constant()
{
    // extract a string separated by non-identifier char
    assert(AstFromString::c_char != NULL);
    char buffer[OFS_MAX_LEN];
    int i = 0;
    // cout << AstFromString::c_char << endl;
    while(AstFromString::afs_is_identifier_char()) {
        //cout << *AstFromString::c_char << endl;
        buffer[i] = *AstFromString::c_char;
        AstFromString::c_char++;
        i++;
    }
    string result(buffer, i);
    return result;
}

// parse and annotate next_sgs with the annotation
void pcfgAnnotationParser::parseAnnotate(string _annotation, SgNode* sgn)
{
    // set the variables for the AstFromString namespace
    AstFromString::c_char = _annotation.c_str();
    Annotation* annotation;
    //pcfg_split
    if(AstFromString::afs_match_substr("pcfg_split")) {
        AstFromString::afs_skip_whitespace();
        annotation = parseSplitAnnotation();        
        // add annotation to the associated node
        SgNode* next_sgs = dynamic_cast<SgNode*> (SageInterface::getNextStatement(isSgStatement(sgn)));
        annotation->addAnnotation(next_sgs);
    }
    else if(AstFromString::afs_match_substr("pcfg_merge")) {
        annotation = new pcfgMergeAnnotation(pcfg_merge);
        annotation->addAnnotation(sgn);        
    }
    else if(AstFromString::afs_match_substr("pcfg_match")) {
        AstFromString::afs_skip_whitespace();
        annotation = parseMatchAnnotation();
        SgExprStatement* next_sgs = dynamic_cast<SgExprStatement*> (SageInterface::getNextStatement(isSgStatement(sgn)));
        if(isSgExprStatement(next_sgs)) {
            SgExpression* exp = next_sgs->get_expression();
            //cout << isSgFunctionCallExp(exp)->unparseToString() << endl;
            annotation->addAnnotation(isSgFunctionCallExp(exp));
        }
        else {
            cerr << "error : adding match annotation\n";
        }
    }

    //pcfg_none
    else {
    }
}

// parse the pcfg_split annotation
Annotation* pcfgAnnotationParser::parseSplitAnnotation()
{
    pcfgSplitAnnotation* annotation;
    // c_char contains the remaining string to parse
    assert(AstFromString::c_char != NULL);
    int size;
    if(AstFromString::afs_match_integer_const(&size)) {
        annotation = new pcfgSplitAnnotation(size, pcfg_split);
        if(AstFromString::afs_skip_whitespace()) {
            for(int i = 0; i < size; i++) {
                string _pset_name = match_any_string_constant();
                cout << _pset_name << endl;
                assert(_pset_name.size() > 0);
                annotation->insert(_pset_name);
                AstFromString::afs_skip_whitespace();
            }
        }
        else {
            cerr << "error in parsing split annotation\n";
        }
    }

    assert(annotation != NULL);
    return annotation;
}

Annotation* pcfgAnnotationParser::parseMatchAnnotation()
{
    pcfgMatchAnnotation* pcfgannotation = new pcfgMatchAnnotation(pcfg_match);
    assert(AstFromString::c_char != NULL);

    if(AstFromString::afs_match_char('(')) {
        //NOTE: pragma string contains a white space after '(' 
        // eventhough it does not exist in the source code
        AstFromString::afs_skip_whitespace();
        string _psetname = match_any_string_constant();
        AstFromString::afs_match_char(',');
        int _psetindex;
        AstFromString::afs_match_integer_const(&_psetindex);
        //cout << _psetname << ", " << _psetindex << endl;
        pcfgannotation->setSource(_psetname, _psetindex);
        AstFromString::afs_skip_whitespace();

        if(!AstFromString::afs_match_char(')')) {
            cerr << "error : parsing match annotation";
        }
        
        // now parse the match
        if(AstFromString::afs_match_char('(')) {
            AstFromString::afs_skip_whitespace();
            string _matchname = match_any_string_constant();
            AstFromString::afs_match_char(',');
            int _matchindex;
            AstFromString::afs_match_integer_const(&_matchindex);
            //cout << _matchname << ", " << _matchindex << endl;
            pcfgannotation->setTarget(_matchname, _matchindex);
            AstFromString::afs_skip_whitespace();

            if(!AstFromString::afs_match_char(')')) {
                cerr << "error : parsing match annotation";
            }
        }        
     }
     else{
         cerr << "error : parsing match annotation\n";
     }

    return pcfgannotation;
}

void PragmaParse::visit(SgNode* sgn)
{
    if(isSgPragmaDeclaration(sgn)) {
        SgPragma* pragma = isSgPragmaDeclaration(sgn)->get_pragma();
        string annot_str = pragma->get_pragma();
        pcfgAParser.parseAnnotate(annot_str, sgn);
        
    }
}


void PragmaParseTest::visit(SgNode* sgn)
{
    if(isSgIfStmt(sgn)) {
        // check if we have the attribute
        AstAttribute* attribute = sgn->getAttribute("pCFGAnnotation");
        if(attribute) {
            cout << attribute->toString() << endl;
        }
    }
    else if(isSgPragmaDeclaration(sgn)) {
        AstAttribute* attribute = sgn->getAttribute("pCFGAnnotation");
        pcfgMergeAnnotation* annotation = dynamic_cast<pcfgMergeAnnotation*> (attribute);
        if(annotation) {
            cout << "Merge at this point in the code\n";
            //SgNode* next_sgs = dynamic_cast<SgNode*> (SageInterface::getNextStatement(isSgStatement(sgn)));
            cout << "current Statement : " << sgn->unparseToString() << endl;
        }
    }
    else if(isSgFunctionCallExp(sgn)) {
        AstAttribute* attribute = sgn->getAttribute("pCFGAnnotation");
        if(attribute) {
            cout << attribute->toString() << endl;
        }
    }
}   
