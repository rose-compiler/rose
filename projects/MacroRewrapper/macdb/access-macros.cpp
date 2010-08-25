#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sqlite3.h>
#include "macro.h"
#include <iostream>
using namespace std;

static sqlite3 * macdb;
// NOTE! BUG with sqlite3_mprintf! Don't let '%q' be the last format!

static int default_callback(void *NotUsed, int argc, char **argv, char **azColName){
  int i;
  for(i=0; i<argc; i++){
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

// TODO: had some troubles with the following two callback funs (misunderstanding sqlite3_exec...)
static int get_macro_def_row(void *pArg, int nArg, char **azArg, char **azCol)
{
  int i;
  macro_def_t *p = (macro_def_t*)pArg;

  if ( p==NULL ) {
    fprintf(stderr, "No pointer specified for storing data\n");
    return 1;
  }

  i = 0;
  if ( i<nArg )
    p->id = atoi(azArg[i++]);
  if ( i<nArg )
    p->filename = sqlite3_mprintf("%s", azArg[i++]);
  if ( i<nArg )
    p->lineno = atoi(azArg[i++]);
  if ( i<nArg )
    p->colno = atoi(azArg[i++]);
  if ( i<nArg )
    p->name = sqlite3_mprintf("%s", azArg[i++]);
  if ( i<nArg )
    p->def = sqlite3_mprintf("%s", azArg[i++]);

  return 0;
}

static int get_macro_call_row(void *pArg, int nArg, char **azArg, char **azCol)
{
  int i;
  macro_call_t *p = (macro_call_t*)pArg;

  if ( p==NULL ) {
    fprintf(stderr, "No pointer specified for storing data\n");
    return 1;
  }

  i = 0;
  if ( i<nArg )
    p->id = atoi(azArg[i++]);
  if ( i<nArg )
    p->filename = sqlite3_mprintf("%s", azArg[i++]);
  if ( i<nArg )
    p->lineno = atoi(azArg[i++]);
  if ( i<nArg )
    p->colno = atoi(azArg[i++]);
  if ( i<nArg )
    p->macdef_id = atoi(azArg[i++]);
  return 0;
}

static char *appendText(char *zIn, char const *zAppend, char quote) {
  int len;
  int i;
  int nAppend = strlen(zAppend);
  int nIn = (zIn?strlen(zIn):0);

  len = nAppend+nIn+1;
  if( quote ){
    len += 2;
    for(i=0; i<nAppend; i++){
      if( zAppend[i]==quote ) len++;
    }
  }

  zIn = (char *)realloc(zIn, len);
  if( !zIn ){
    return 0;
  }

  if( quote ){
    char *zCsr = &zIn[nIn];
    *zCsr++ = quote;
    for(i=0; i<nAppend; i++){
      *zCsr++ = zAppend[i];
      if( zAppend[i]==quote ) *zCsr++ = quote;
    }
    *zCsr++ = quote;
    *zCsr++ = '\0';
    assert( (zCsr-zIn)==len );
  }else{
    memcpy(&zIn[nIn], zAppend, nAppend);
    zIn[len-1] = '\0';
  }

  return zIn;
}


// NOTE: to check uniqueness before actual insertion.
int add_macro_def(const string& filename, int lineno, int colno, const string& name, 
		  const string& def, vector<string>& args)
{
  char * sqlcmd = 0;
  int rc = 0;
  char *zErrMsg = 0;

  /* insert the macro def: */
  sqlcmd = sqlite3_mprintf("INSERT INTO macrodefs(filename, name, def, lineno, colno) VALUES('%q','%q','%q',%d,%d);",
                           filename.c_str(), name.c_str(), def.c_str(), lineno, colno);
  //  sqlcmd = appendText(sqlcmd, "INSERT INTO macrodefs(filename, lineno, colno, name, def) VALUES(", 0); too cumbersome to do it this way; use sqlite3_mprintf instead.
  cout << "sql for add_macro_def: " << sqlcmd << endl;

  rc = sqlite3_exec(macdb, sqlcmd, NULL, 0, &zErrMsg);
  if ( rc!=SQLITE_OK ) {
    fprintf(stderr, "SQL command error: %s\n\tduring insertion of macro def: %s\n", sqlcmd, zErrMsg);
    sqlite3_free(zErrMsg);
  }
  sqlite3_free(sqlcmd); sqlcmd = 0;

  /* insert the macro's formal parameters: TODO */

  return rc;
}

int add_macro_call(const string& filename, int lineno, int colno, vector<string>& tokenlist,
                   vector<string>& args, const string& macdeffilename, int macdeflineno,
		   const string& expanded, const string& unparsed)
{
  char * sqlcmd = 0;
  int rc = 0;
  char *zErrMsg = 0;
  long long int rowid = 0;
  sqlite3_stmt * sqlstmt;

  // TODO: all the following database operations should be ONE transaction.

  /* search for the id of the macro def: */
  macro_def_t macdefid;
  macdefid.id = -1;
  sqlcmd = sqlite3_mprintf("SELECT id, filename, lineno FROM macrodefs WHERE filename=='%q' AND lineno==%d;", macdeffilename.c_str(), macdeflineno);
  rc = sqlite3_prepare(macdb, sqlcmd, -1, &sqlstmt, 0);
  if ( rc!=SQLITE_OK || !sqlstmt ) {
    fprintf(stderr, "SQL command error during insertion of maccall: %s\n", sqlcmd);
    sqlite3_free(sqlcmd); sqlcmd = 0;
    return rc;
  }
  rc = sqlite3_step(sqlstmt);
  while( rc==SQLITE_ROW ) {
    rowid++;
    if ( rowid==1 ) {
      macdefid.id = sqlite3_column_int64(sqlstmt, 0); // should >0
    } else {
      fprintf(stderr, "not unique mac def: %s %d.\n", sqlite3_column_text(sqlstmt, 1), sqlite3_column_int(sqlstmt, 2));
      sqlite3_finalize(sqlstmt);
      sqlite3_free(sqlcmd); sqlcmd = 0;
      return -2;
    }
    rc = sqlite3_step(sqlstmt);
  }
  sqlite3_finalize(sqlstmt);
  sqlite3_free(sqlcmd); sqlcmd = 0;
/*  rc = sqlite3_exec(macdb, sqlcmd, get_macro_def_row, &macdefid, &zErrMsg);
  if ( rc!=SQLITE_OK ) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_free(sqlcmd); sqlcmd = 0;
    return rc;
  }
  sqlite3_free(sqlcmd); sqlcmd = 0;
*/

  /* check whether we find the "id": */
  if ( macdefid.id == -1 ) {
    fprintf(stderr, "No mac def found. Insert it first.\n");
    return -1;
  }

  /* insert the macro call: */

  sqlcmd = sqlite3_mprintf("INSERT INTO macrocalls(filename, lineno, colno, expanded,  unparsed, macrodef) VALUES('%q',%d,%d,'%q','%q',%d);",
                           filename.c_str(), lineno, colno,  expanded.c_str(), unparsed.c_str(),  macdefid.id);
  cout << "sql for add_macro_call: " << sqlcmd << endl;

  rc = sqlite3_exec(macdb, sqlcmd, NULL, 0, &zErrMsg);
  if ( rc!=SQLITE_OK ) {
    fprintf(stderr, "SQL command error: %s\n\tduring insertion of macro call: %s\n", sqlcmd, zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_free(sqlcmd); sqlcmd = 0;
    return rc;
  }
  rowid = sqlite3_last_insert_rowid(macdb);
  sqlite3_free(sqlcmd); sqlcmd = 0;

  /* insert the macro call's tokenlist: */
  // TODO: maybe better to add NO. for each token; not rely on the order of the primary key.
  for (std::vector<std::string>::iterator it_tok = tokenlist.begin(); it_tok != tokenlist.end(); ++it_tok) {
    sqlcmd = sqlite3_mprintf("INSERT INTO tokenlist(token, macrocall) VALUES('%q', %d)", it_tok->c_str(), rowid); // Q: weird error if switching the order of columns "token" and "macrocall"
    rc = sqlite3_exec(macdb, sqlcmd, NULL, 0, &zErrMsg);
    if ( rc!=SQLITE_OK ) {
      fprintf(stderr, "SQL command error: %s\n\tduring insertion of tokenlist: %s\n", sqlcmd, zErrMsg);
      sqlite3_free(zErrMsg);
    }
    sqlite3_free(sqlcmd); sqlcmd = 0;
  }

  /* insert the macro call's actual arguments: TODO */

  return rc;
}

int replace_macro_call(const string& filename, int lineno, int colno, vector<string>& tokenlist,
                       vector<string>& args, const string& macdeffilename, int macdeflineno,
		       const string& expanded, const string& unparsed)
{
  char * sqlcmd = 0;
  int rc = 0;
  char *zErrMsg = 0;
  long long int rowid = 0;
  sqlite3_stmt * sqlstmt;

  // TODO: all the following database operations MUST be one
  // transaction; simultaneous replaces many cause problems with
  // tokenlist.

  // search for the macro call:
  sqlcmd = sqlite3_mprintf("SELECT id FROM macrocalls WHERE filename=='%q' AND lineno=%d AND colno=%d;", filename.c_str(), lineno, colno);
  rc = sqlite3_prepare(macdb, sqlcmd, -1, &sqlstmt, 0);
  if ( rc!=SQLITE_OK || !sqlstmt ) {
    fprintf(stderr, "SQL command error during searching macro call: %s\n", sqlcmd);
  } else {
    rc = sqlite3_step(sqlstmt);
    while( rc==SQLITE_ROW ) {
      rowid = sqlite3_column_int64(sqlstmt, 0);
      break;
      rc = sqlite3_step(sqlstmt); // dead code
    }
  }
  sqlite3_finalize(sqlstmt);
  sqlite3_free(sqlcmd); sqlcmd = 0;

  if ( rowid<=0 ) {
    fprintf(stderr, "Can't replace non-existing macro call: %s %d %d\n", filename.c_str(), lineno, colno);
    return -1;
  }

  /* search for the id of the macro def: */
  macro_def_t macdefid;
  macdefid.id = -1;
  sqlcmd = sqlite3_mprintf("SELECT id, filename, lineno FROM macrodefs WHERE filename=='%q' AND lineno==%d;", macdeffilename.c_str(), macdeflineno);
  rc = sqlite3_prepare(macdb, sqlcmd, -1, &sqlstmt, 0);
  if ( rc!=SQLITE_OK || !sqlstmt ) {
    fprintf(stderr, "SQL command error during insertion of maccall: %s\n", sqlcmd);
    sqlite3_free(sqlcmd); sqlcmd = 0;
    return rc;
  }
  rc = sqlite3_step(sqlstmt);
  while( rc==SQLITE_ROW ) {
    rowid++;
    if ( rowid==1 ) {
      macdefid.id = sqlite3_column_int64(sqlstmt, 0); // should >0
    } else {
      fprintf(stderr, "not unique mac def: %s %d.\n", sqlite3_column_text(sqlstmt, 1), sqlite3_column_int(sqlstmt, 2));
      sqlite3_finalize(sqlstmt);
      sqlite3_free(sqlcmd); sqlcmd = 0;
      return -2;
    }
    rc = sqlite3_step(sqlstmt);
  }
  sqlite3_finalize(sqlstmt);
  sqlite3_free(sqlcmd); sqlcmd = 0;

  /* check whether we find the "id": */
  if ( macdefid.id == -1 ) {
    fprintf(stderr, "No mac def found. Insert it first.\n");
    return -1;
  }

  /* replace the macro call: */
  sqlcmd = sqlite3_mprintf("UPDATE macrocalls SET expanded='%q', unparsed='%q', macrodef=%d WHERE filename='%q' AND lineno=%d AND colno=%d;",
                           expanded.c_str(), unparsed.c_str(), macdefid.id, filename.c_str(), lineno, colno);
  cout << "sql for replace_macro_call: " << sqlcmd << endl;

  rc = sqlite3_exec(macdb, sqlcmd, NULL, 0, &zErrMsg);
  if ( rc!=SQLITE_OK ) {
    fprintf(stderr, "SQL command error: %s\n\tduring replacing macro call: %s\n", sqlcmd, zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_free(sqlcmd); sqlcmd = 0;
    return rc;
  }
  sqlite3_free(sqlcmd); sqlcmd = 0;

  // delete the old tokenlist:
  sqlcmd = sqlite3_mprintf("DELETE FROM tokenlist WHERE macrocall=%d;", rowid);
  rc = sqlite3_exec(macdb, sqlcmd, NULL, 0, &zErrMsg);
  if ( rc!=SQLITE_OK ) {
    fprintf(stderr, "SQL cmd error: %s\n\tduring deleting of old tokenlist: %s\n", sqlcmd, zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_free(sqlcmd); sqlcmd = 0;
    return rc;
  }

  /* insert the macro call's tokenlist: */
  // TODO: maybe better to add NO. for each token; not rely on the order of the primary key.
  for (std::vector<std::string>::iterator it_tok = tokenlist.begin(); it_tok != tokenlist.end(); ++it_tok) {
    sqlcmd = sqlite3_mprintf("INSERT INTO tokenlist(token, macrocall) VALUES('%q', %d)", it_tok->c_str(), rowid); // Q: weird error if switching the order of columns "token" and \"macrocall"...'%q' cannot be the last formatting flag...
    rc = sqlite3_exec(macdb, sqlcmd, NULL, 0, &zErrMsg);
    if ( rc!=SQLITE_OK ) {
      fprintf(stderr, "SQL command error: %s\n\tduring insertion of tokenlist: %s\n", sqlcmd, zErrMsg);
      sqlite3_free(zErrMsg);
    }
    sqlite3_free(sqlcmd); sqlcmd = 0;
  }

  /* insert the macro call's actual arguments: TODO */

  return rc;
}


bool exist_macro_def(const string& filename, int lineno)
{
  char * sqlcmd = 0;
  int rc = 0;
  char *zErrMsg = 0;
  long long int rowid = 0;
  sqlite3_stmt * sqlstmt;
  long long int rowCount = 0;

  bool rslflag = false;
  sqlcmd = sqlite3_mprintf("SELECT id FROM macrodefs WHERE filename=='%q' AND lineno=%d;", filename.c_str(), lineno);
  rc = sqlite3_prepare(macdb, sqlcmd, -1, &sqlstmt, 0);
  if ( rc!=SQLITE_OK || !sqlstmt ) {
    fprintf(stderr, "SQL command error during searching macro call: %s.\n", sqlcmd);
  } else {
    rc = sqlite3_step(sqlstmt);
    while( rc==SQLITE_ROW ) {
      rslflag = true;
      break;
      rc = sqlite3_step(sqlstmt); // dead code
    }
  }
  sqlite3_finalize(sqlstmt);
  sqlite3_free(sqlcmd); sqlcmd = 0;

  return rslflag;
}

bool exist_macro_call(const string& filename, int lineno, int colno)
{
  char * sqlcmd = 0;
  int rc = 0;
  char *zErrMsg = 0;
  long long int rowid = 0;
  sqlite3_stmt * sqlstmt;
  long long int rowCount = 0;

  bool rslflag = false;
  sqlcmd = sqlite3_mprintf("SELECT id FROM macrocalls WHERE filename=='%q' AND lineno=%d AND colno=%d;", filename.c_str(), lineno, colno);
  rc = sqlite3_prepare(macdb, sqlcmd, -1, &sqlstmt, 0);
  if ( rc!=SQLITE_OK || !sqlstmt ) {
    fprintf(stderr, "SQL command error during searching macro call: %s\n", sqlcmd);
  } else {
    rc = sqlite3_step(sqlstmt);
    while( rc==SQLITE_ROW ) {
      rslflag = true;
      break;
      rc = sqlite3_step(sqlstmt); // dead code
    }
  }
  sqlite3_finalize(sqlstmt);
  sqlite3_free(sqlcmd); sqlcmd = 0;

  return rslflag;
}

macro_def_t find_macro_def(const string& filename, int lineno)
{
  char * sqlcmd = 0;
  int rc = 0;
  char *zErrMsg = 0;
  long long int rowid = 0;
  sqlite3_stmt * sqlstmt;
  long long int rowCount = 0;

  macro_def_t rsl; rsl.id = -1;
  sqlcmd = sqlite3_mprintf("SELECT * FROM macrodefs WHERE filename=='%q' AND lineno=%d;", filename.c_str(), lineno);
  rc = sqlite3_prepare(macdb, sqlcmd, -1, &sqlstmt, 0);
  if ( rc!=SQLITE_OK || !sqlstmt ) {
    fprintf(stderr, "SQL command error during searching macro def: %s\n", sqlcmd);
  } else {
    rc = sqlite3_step(sqlstmt);
    while( rc==SQLITE_ROW ) {
      rowid++;
      if ( rowid==1 ) {
	rsl.id = sqlite3_column_int64(sqlstmt, 0); // .id should be >0
	rsl.filename = sqlite3_mprintf("%s", sqlite3_column_text(sqlstmt, 1)); // should not be NULL
	rsl.lineno = sqlite3_column_int(sqlstmt, 2); // should >0
	rsl.colno = sqlite3_column_int(sqlstmt, 3); // could be 0
	rsl.name = sqlite3_mprintf("%s", sqlite3_column_text(sqlstmt, 4)); // could be '(NULL)'
	rsl.def = sqlite3_mprintf("%s", sqlite3_column_text(sqlstmt, 5)); // could be '(NULL)'
      } else {
	fprintf(stderr, "Not unique mac def: %s %d %d!\n", rsl.filename, rsl.lineno, rsl.colno);
	break;
      }
      rc = sqlite3_step(sqlstmt);
    }
  }
  sqlite3_finalize(sqlstmt);
  sqlite3_free(sqlcmd); sqlcmd = 0;
/*  rc = sqlite3_exec(macdb, sqlcmd, get_macro_def_row, &rsl, &zErrMsg); // this is causing seg fault. I don't know how to use the callback correctly...
  if ( rc!=SQLITE_OK ) {
    fprintf(stderr, "SQL error during searching for macro def: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  sqlite3_free(sqlcmd); sqlcmd = 0;
*/
  return rsl;
}

macro_call_t find_macro_call(const string& filename, int lineno, int colno)
{
  char * sqlcmd = 0;
  int rc = 0;
  char *zErrMsg = 0;
  long long int rowid = 0;
  sqlite3_stmt * sqlstmt;
  long long int rowCount = 0;

  macro_call_t rsl; rsl.id = -1;
  sqlcmd = sqlite3_mprintf("SELECT * FROM macrocalls LEFT JOIN tokenlist ON macrocalls.id==tokenlist.macrocall WHERE filename=='%q' AND lineno=%d AND colno=%d ORDER BY tokenlist.id ASC;", filename.c_str(), lineno, colno);
//  sqlcmd = sqlite3_mprintf("SELECT * FROM macrocalls, tokenlist WHERE filename=='%q' AND lineno=%d AND colno=%d ORDER BY tokenlist.id ASC;", filename.c_str(), lineno, colno);

//  std::cout << sqlcmd << std::endl;
  rc = sqlite3_prepare(macdb, sqlcmd, -1, &sqlstmt, 0);
  if ( rc!=SQLITE_OK || !sqlstmt ) {
    fprintf(stderr, "SQL command error during searching macro call: %s\n", sqlcmd);
  } else {
    rc = sqlite3_step(sqlstmt);
    while( rc==SQLITE_ROW ) {
      // There should only be ONE such mac call:
      if ( rsl.id>0 && rsl.id!=sqlite3_column_int64(sqlstmt, 0) ) {
	fprintf(stderr, "mac call not unique: %s %d %d!\n", sqlite3_column_text(sqlstmt, 1),
		sqlite3_column_int(sqlstmt, 2), sqlite3_column_int(sqlstmt, 3));
	break;
      }

      rowid++;
      if ( rowid==1 ) {
	rsl.id = sqlite3_column_int64(sqlstmt, 0);
	rsl.filename = sqlite3_mprintf("%s", sqlite3_column_text(sqlstmt, 1));
	rsl.lineno = sqlite3_column_int(sqlstmt, 2);
	rsl.colno = sqlite3_column_int(sqlstmt, 3);
	rsl.macdef_id = sqlite3_column_int(sqlstmt, 4);
	rsl.expanded = sqlite3_mprintf("%s", sqlite3_column_text(sqlstmt, 7));
	rsl.unparsed_statements = sqlite3_mprintf("%s", sqlite3_column_text(sqlstmt, 8));
      }
      if ( sqlite3_column_text(sqlstmt, 11)!=NULL )
	rsl.tokenlist.push_back(sqlite3_mprintf("%s", sqlite3_column_text(sqlstmt, 11)));

      rc = sqlite3_step(sqlstmt);
    }
  }
  sqlite3_finalize(sqlstmt);
  sqlite3_free(sqlcmd); sqlcmd = 0;

  return rsl;
}

void iterate_macro_defs_calls()
{
  char * sqlcmd = 0;
  int rc = 0;
  char *zErrMsg = 0;
  long long int rowid = 0;
  sqlite3_stmt * sqlstmt;
  long long int rowCount = 0;

  /* "for all macro defs:
         for all macro calls:"
     may not be appropriate for the database,
     because it seems that sqlite can't execute a sqlcmd nested in another.
     So, better to just use:
     "for all macro calls whose macdef are the same (based on ORDER BY, GROUP BY and aggregate functions):"
  */
//   sqlcmd = "SELECT * FROM macrodefs;";
//   rc = sqlite3_prepare(macdb, sqlcmd, -1, &sqlstmt, 0);
//   if ( rc!=SQLITE_OK || !sqlstmt ) {
//     fprintf(stderr, "SQL command compilation error during iteration.\n");
//     return;
//   }
//   rc = sqlite3_step(sqlstmt);
//   while( rc==SQLITE_ROW ){
//     // search for all macro calls
//     char * sqlcmd2 = 0;
//     sqlite3_stmt * sqlstmt2;
//     sqlcmd2
//     rc = sqlite3_step(pSelect);
//   }

//  sqlcmd = "SELECT * FROM macrodefs, macrocalls, tokenlist WHERE macrodefs.id==macrocalls.macrodef AND macrocalls.id==tokenlist.macrocall GROUP BY macrocall ORDER BY macrodef, macrocall, tokenlist.id;"; // Q: Is "order by" executed before "group by" or after (it seemly matters)?
  sqlcmd = "SELECT * FROM macrodefs LEFT JOIN macrocalls ON macrodefs.id==macrocalls.macrodef LEFT JOIN tokenlist ON macrocalls.id==tokenlist.macrocall ORDER BY macrodefs.id ASC, macrocalls.id ASC, tokenlist.id ASC;"; // return all mac def, mac calls with their tokenlists; each token is a row; be careful that some tokens may be NULL.
  rc = sqlite3_prepare(macdb, sqlcmd, -1, &sqlstmt, 0);
  if ( rc!=SQLITE_OK || !sqlstmt ) {
    fprintf(stderr, "SQL command error during iteration: %s\n", sqlcmd);
    return;
  }

  // NOTE: better to use pointer types if the vectors may be passed around:
  vector<macro_def_t*> vec_macrodefs;
  vector<pair<macro_def_t*, vector<macro_call_t> > > vec_macrocalls; // TODO: change the template types to pointer types.

  rc = sqlite3_step(sqlstmt);
  while( rc==SQLITE_ROW ) {
    // Collect ALL data into vec_macrodefs and vec_macrocalls (may run into scalability problem in the future: TODO.)
    // The following relies on the "ORDER BY..." in the above sql command.

    // (1) store the macro def if it is not stored yet;
    bool anewmacdef = true;
    if ( vec_macrodefs.size()>0 && sqlite3_column_int64(sqlstmt, 0)==vec_macrodefs.back()->id )
      anewmacdef = false;	// no need to store the macro def
    else {
      macro_def_t* tmpmacdef = new macro_def_t();
      tmpmacdef->id = sqlite3_column_int64(sqlstmt, 0); // should >0
      tmpmacdef->filename = sqlite3_mprintf("%s", sqlite3_column_text(sqlstmt, 1)); // should not NULL
      tmpmacdef->lineno = sqlite3_column_int(sqlstmt, 2); // should >0
      tmpmacdef->colno = sqlite3_column_int(sqlstmt, 3);
      tmpmacdef->name = sqlite3_mprintf("%s", sqlite3_column_text(sqlstmt, 4)); // could be NULL, but that would be invalid mac def. TODO: enforce data validity.
      tmpmacdef->def = sqlite3_mprintf("%s", sqlite3_column_text(sqlstmt, 5)); // could be NULL, but sqlite3_mprintf returns "" instead of NULL
      vec_macrodefs.push_back(tmpmacdef);
    }

    if ( vec_macrodefs.size()<=0 ) {
      fprintf(stderr, "No mac def found for the mac call: %s %d %d...weird\n", sqlite3_column_text(sqlstmt, 8),
	      sqlite3_column_int(sqlstmt, 9), sqlite3_column_int(sqlstmt, 10));
    } else {
      // (2) store the macro call if it is not stored yet;
      if ( vec_macrocalls.size()>0 && vec_macrocalls.back().second.size()>0 && vec_macrocalls.back().second.back().id==sqlite3_column_int64(sqlstmt, 7) )
	;
      else {			// a new macro call
	if ( vec_macrocalls.size()<=0 || anewmacdef==true ) { // mac calls to a new def
	  vec_macrocalls.push_back( pair<macro_def_t*, vector<macro_call_t> >( vec_macrodefs.back(), vector<macro_call_t>() ) ); // TODO: the pointer is not good
	}

	if ( sqlite3_column_int64(sqlstmt, 7)>0 ) {
	  macro_call_t tmpmaccall;
	  tmpmaccall.id = sqlite3_column_int64(sqlstmt, 7); // should >0
	  tmpmaccall.filename = sqlite3_mprintf("%s", sqlite3_column_text(sqlstmt, 8));	// should not NULL
	  tmpmaccall.lineno = sqlite3_column_int(sqlstmt, 9); // should >0
	  tmpmaccall.colno = sqlite3_column_int(sqlstmt, 10); // should >0
	  tmpmaccall.macdef_id = sqlite3_column_int(sqlstmt, 11); // should >0
	  tmpmaccall.expanded = sqlite3_mprintf("%s", sqlite3_column_text(sqlstmt, 14)); // Q: what happens if a string is assigned NULL? Seg faults! c++'s fault.
	  tmpmaccall.unparsed_statements = sqlite3_mprintf("%s", sqlite3_column_text(sqlstmt, 15)); // for NULL, sqlite3_mprintf returns "" instead of NULL.
	  vec_macrocalls.back().second.push_back(tmpmaccall);
	} else ;		// some mac defs may have no calls.
      }

      // (3) store tokens into the macro call;
      if ( sqlite3_column_text(sqlstmt, 18)!=NULL ) {
	if ( vec_macrocalls.size()<=0 || vec_macrocalls.back().second.size()<=0 ||
             vec_macrocalls.back().second.back().id!=sqlite3_column_int(sqlstmt, 17) ) {
          fprintf(stderr, "No mac call found for this token: last mac call id=%d %s %d...weird.\n", vec_macrocalls.back().second.back().id,
		  sqlite3_column_text(sqlstmt, 18), sqlite3_column_int(sqlstmt, 17));
	} else {
	  char * tmptoken = sqlite3_mprintf("%s", sqlite3_column_text(sqlstmt, 18));
	  vec_macrocalls.back().second.back().tokenlist.push_back(tmptoken);
	}
      }	else ;			// some mac call may have empty tokenlist, e.g. #define EMPTY_MACRO /* empty macro */
    }

    rc = sqlite3_step(sqlstmt);
  }
  sqlite3_finalize(sqlstmt);

  // Now, we may do any with "vec_macrodefs" and "vec_macrocalls" here,
  // including the bug detecting algorithm:
  compare_macro_tokenlists(vec_macrodefs, vec_macrocalls);

  // TODO: better to release all memory allocated by sqlite3_mprintf here.
  return;
}

bool is_type(std::string currentStr){
   bool a_rose_type = false;
   static std::string typeStr[] = {
    "SgArrayType"
    ,"SgComplex"
    ,"SgFunctionType"
    ,"SgMemberFunctionType"
    ,"SgPartialFunctionType"
    ,"SgPartialFunctionModifierType"
    ,"SgUnknownMemberFunctionType"
    ,"SgModifierType"
    ,"SgNamedType"
    ,"SgClassType"
    ,"SgEnumType"
    ,"SgTypedefType" 
    ,"SgPointerType"
    ,"SgPointerMemberType"
    ,"SgQualifiedNameType"
    ,"SgReferenceType"
    ,"SgTemplateType"
    ,"SgTypeBool"
    ,"SgTypeChar"
    ,"SgTypeDefault"
    ,"SgTypeDouble"
    ,"SgTypeEllipse"
     ,"SgTypeFloat"
     ,"SgTypeGlobalVoid"
     ,"SgTypeInt"
     ,"SgTypeLong"
     ,"SgTypeLongDouble"
     ,"SgTypeLongLong"
     ,"SgTypeShort"
     ,"SgTypeSignedChar"
     ,"SgTypeSignedInt"
     ,"SgTypeSignedLong"
     ,"SgTypeSignedShort"
     ,"SgTypeString"
     ,"SgTypeUnknown"
     ,"SgTypeUnsignedChar"
     ,"SgTypeUnsignedInt"
     ,"SgTypeUnsignedLong"
     ,"SgTypeUnsignedLongLong"
     ,"SgTypeUnsignedShort"
     ,"SgTypeVoid"
     ,"SgTypeWchar"};

     int lengthOfTypeVec = sizeof(typeStr)/sizeof(typeStr[0]);

     
     for(int i=0 ; i < lengthOfTypeVec ; i++){
         if(typeStr[i]==currentStr){
             a_rose_type = true;
             break;
         }
            

     }

 
      return a_rose_type;

};

bool random_filtered_node(std::string nodeName){
     bool a_random_filtered_node = false;
     static std::string typeStr[] = {
        "SgCastExp",
        "SgExprStatement"
      };

     int lengthOfTypeVec = sizeof(typeStr)/sizeof(typeStr[0]);

     
     for(int i=0 ; i < lengthOfTypeVec ; i++){
         if(typeStr[i]==nodeName){
             a_random_filtered_node = true;
             break;
         }
      
     }

      
      return a_random_filtered_node;


}
bool compare_macro_tokenlists(std::vector<std::string> mcall, std::vector<std::string> first_mcall, bool filter){
	bool is_consistent = true;
	if(filter==true){
                std::vector<std::string> new_mcall;

                 
		//new analysis which filters on types
                std::cout << "mcall:       ";
		for ( std::vector<std::string>::iterator  i = mcall.begin(); i != mcall.end(); ++i)
		{
			std::string currentString( *i ); 

			if( (is_type(currentString) == true ) 
					||(random_filtered_node(currentString) == true ) ){
                         
                   }else{
                           std::cout << *i << " ";

                     new_mcall.push_back(*i);
                   }

		}

                mcall = new_mcall;

                new_mcall.clear();

                std::vector<std::string> new_mcall2;

                std::cout << std::endl;
                std::cout << "first_mcall: ";
		for ( std::vector<std::string>::iterator  i = first_mcall.begin(); i != first_mcall.end(); ++i)
		{

			std::string currentString( *i ); 
			if( (is_type(currentString) == true ) 
					||(random_filtered_node(currentString) == true ) ){
                        }else{
                           std::cout << *i << " ";

                             new_mcall2.push_back(*i);

}

		}
                 std::cout << std::endl;

                first_mcall = new_mcall2;

	}

	//Doing old analysis

	if( mcall.size() == first_mcall.size() ){
		for ( int i = 0; i < mcall.size(); i++)
		{
			if( mcall[i] != first_mcall[i]){
				is_consistent=false;
				break;
			}
		}
	}else{
		is_consistent = false;
	}


	return is_consistent;
};

bool compare_macro_tokenlists(std::vector<char*> mcall, std::vector<char*> first_mcall, bool macro_def_printed, bool filter){
	bool is_consistent = true;


	if(filter==true){
                std::vector<char*> new_mcall;

                 
		//new analysis which filters on types
		for ( std::vector<char*>::iterator  i = mcall.begin(); i != mcall.end(); ++i)
		{
			std::string currentString( *i ); 

			if( (is_type(currentString) == false ) 
					||(random_filtered_node(currentString) == false ) ){
                         
                   }else{
                     new_mcall.push_back(*i);
                   }

		}

                mcall = new_mcall;

                new_mcall.clear();

                std::vector<char*> new_mcall2;

		for ( std::vector<char*>::iterator  i = first_mcall.begin(); i != first_mcall.end(); ++i)
		{

			std::string currentString( *i ); 
			if( (is_type(currentString) == false ) 
					||(random_filtered_node(currentString) == false ) ){
                        }else
                             new_mcall2.push_back(*i);

		}
 
                first_mcall = new_mcall2;

	}

	//Doing old analysis

	if( mcall.size() == first_mcall.size() ){
		for ( int i = 0; i < mcall.size(); i++)
		{
			if( strcmp(mcall[i], first_mcall[i])!=0 ){
				is_consistent=false;
				break;
			}
		}
	}else{
		is_consistent = false;
	}


	return is_consistent;

};

void compare_macro_tokenlists(vector<macro_def_t*>& macdefs, vector<pair<macro_def_t*, vector<macro_call_t> > >& maccalls)
{
	/* Andreas: you can put the bug detection alg. here. */

	/*	cout << "****** MAC DEFs *********" << endl;
		for (vector<macro_def_t*>::const_iterator itr=macdefs.begin(); itr!=macdefs.end(); itr++)
		output_macdef(cout, **itr);
		cout << "****** MAC Calls ********" << endl;*/

	int number_of_matches = 0;
	int number_of_inconsistencies = 0;

	for ( vector<pair<macro_def_t*, vector<macro_call_t> > >::const_iterator itr=maccalls.begin(); itr!=maccalls.end(); itr++)
	{
		bool macro_def_printed = false;
		for ( vector<macro_call_t>::const_iterator itr2=(*itr).second.begin(); itr2!=(*itr).second.end(); itr2++)
		{

		//output_maccall(cout, *itr2);
			macro_call_t mcall = *itr2;
			macro_call_t first_mcall = itr->second[0];



			if(  (compare_macro_tokenlists(mcall.tokenlist, first_mcall.tokenlist, true, true) == false )  &&
					(mcall.tokenlist.size()>1) &&
					(mcall.unparsed_statements != first_mcall.unparsed_statements ) &&
					//Filter out macro calls from the system header files
					( strncmp(mcall.filename,"/usr/include",12) != 0) &&
					( strncmp(first_mcall.filename,"/usr/include",12) != 0)

			  ){

			if(macro_def_printed == false){
				cout << "NEW MACRO DEF at " << hex << (*itr).first << ": " << dec; 
				output_macdef(cout, *((*itr).first)); cout << endl;
				macro_def_printed = true;
			}


	

				output_maccall(cout, mcall);
				std::cout << "Is inconsistent with:" << std::endl;
				output_maccall(cout, first_mcall);


				if( compare_macro_tokenlists(mcall.tokenlist, first_mcall.tokenlist, true, true)  ==true){
					std::cout << "MATCH when types are filtered out\n";
				}else{
					std::cout << "NO MATCH when types are filtered out\n";
				}
                                std::cout << std::endl;
				number_of_inconsistencies+=1;
			}else{

			if(macro_def_printed == false){
				cout << "NEW MACRO DEF at " << hex << (*itr).first << ": " << dec; 
				output_macdef(cout, *((*itr).first)); cout << endl;
				macro_def_printed = true;
			}

                std::cout << std::endl; 
				output_maccall(cout, mcall);
				std::cout << "Is consistent with:" << std::endl;
				output_maccall(cout, first_mcall);
                std::cout << std::endl; 

				number_of_matches+=1;
			}


		}
	}


	std::cout << " NUMBER OF MATCHES: " << number_of_matches << " NUMBER OF INCONSISTENCIES: " << number_of_inconsistencies << endl;
}

void output_macdef(ostream & out, const macro_def_t & mdef)
{
	out << "ID:" << mdef.id << " FILE:" << mdef.filename << " LINE:" << mdef.lineno << " COL:" << mdef.colno
		<< " MAC:" << mdef.name << " DEF:" << mdef.def << endl;
}
void output_maccall(ostream & out, const macro_call_t & mcall)
{
	out << "ID:" << mcall.id << " FILE:" << mcall.filename << " LINE:" << mcall.lineno << " COL:" << mcall.colno
		<< " MACDEF:" << mcall.macdef_id << " \nTOKENS:";
	for (vector<char *>::const_iterator itr=mcall.tokenlist.begin(); itr!=mcall.tokenlist.end(); ++itr)
		out << (*itr) << ",";
	out << " \nEXPANDED:" << mcall.expanded << " \nUNPARSED:" << mcall.unparsed_statements << endl;
}

char * get_varchar_in_macro_def(macro_def_id_t mid, const char * field);
int get_int_in_macro_def(macro_def_id_t mid, const char * field);

char * get_varchar_in_macro_call(macro_call_id_t mid, const char * field);
int get_int_in_macro_call(macro_call_id_t mid, const char * field);
char * get_macro_def_from_call(macro_call_id_t mid);



int init_macdb(const char * pathtodb)
{
	int rc;
	if ( pathtodb==NULL )
		rc = sqlite3_open(MACDB_NAME, &macdb);
	else
		rc = sqlite3_open(pathtodb, &macdb);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(macdb));
		sqlite3_close(macdb);
	}
	return rc;
}

int close_macdb()
{
	return sqlite3_close(macdb);
}


int delete_macro_defs_in_file(const char * filename)
{
	char * sqlcmd = 0;
	int rc = 0;
	char *zErrMsg = 0;
	long long int rowid = 0;
	sqlite3_stmt * sqlstmt;
	long long int rowCount = 0;

	sqlcmd = sqlite3_mprintf("DELETE FROM macrodefs WHERE filename=='%q';", filename);
	rc = sqlite3_exec(macdb, sqlcmd, NULL, 0, &zErrMsg);
	if ( rc!=SQLITE_OK ) {
		fprintf(stderr, "SQL error during deletion of macro def: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	sqlite3_free(sqlcmd); sqlcmd = 0;

	// TODO: better to delete corresponding macro calls to maintain consistency.
	return rc;
}

int delete_macro_calls_in_file(const char * filename)
{
	char * sqlcmd = 0;
	int rc = 0;
	char *zErrMsg = 0;
	long long int rowid = 0;
	sqlite3_stmt * sqlstmt;
	long long int rowCount = 0;

	sqlcmd = sqlite3_mprintf("DELETE FROM macrocalls WHERE filename=='%q';", filename);
	rc = sqlite3_exec(macdb, sqlcmd, NULL, 0, &zErrMsg);
	if ( rc!=SQLITE_OK ) {
		fprintf(stderr, "SQL error during deletion of macro call: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	sqlite3_free(sqlcmd); sqlcmd = 0;

	// TODO: better also to delete corresponding tokenlist.
	return rc;
}

int delete_all_defs_calls()
{
	char * sqlcmd = 0;
	int rc = 0;
	char *zErrMsg = 0;
	long long int rowid = 0;
	sqlite3_stmt * sqlstmt;
	long long int rowCount = 0;

	sqlcmd = "DELETE FROM macrodefs;";
	rc = sqlite3_exec(macdb, sqlcmd, NULL, 0, &zErrMsg);
	if ( rc!=SQLITE_OK ) {
		fprintf(stderr, "SQL error during deletion: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

	sqlcmd = "DELETE FROM macrocalls;";
	rc = sqlite3_exec(macdb, sqlcmd, NULL, 0, &zErrMsg);
	if ( rc!=SQLITE_OK ) {
		fprintf(stderr, "SQL error during deletion: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

	sqlcmd = "DELETE FROM formalargs;";
	rc = sqlite3_exec(macdb, sqlcmd, NULL, 0, &zErrMsg);
	if ( rc!=SQLITE_OK ) {
		fprintf(stderr, "SQL error during deletion: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

	sqlcmd = "DELETE FROM actualargs;";
	rc = sqlite3_exec(macdb, sqlcmd, NULL, 0, &zErrMsg);
	if ( rc!=SQLITE_OK ) {
		fprintf(stderr, "SQL error during deletion: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

	sqlcmd = "DELETE FROM tokenlist;";
	rc = sqlite3_exec(macdb, sqlcmd, NULL, 0, &zErrMsg);
	if ( rc!=SQLITE_OK ) {
		fprintf(stderr, "SQL error during deletion: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

	return rc;
}


