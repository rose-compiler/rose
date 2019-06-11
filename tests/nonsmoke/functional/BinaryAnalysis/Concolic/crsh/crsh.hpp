
#ifndef CRSH_H
#define CRSH_H

void crsh_db(const char*);
void crsh_db_end();

// environment variables
typedef std::pair<std::string, std::string> EnvVar;
typedef std::list<EnvVar>                   EnvVarLst;

EnvVarLst* crsh_envlst(EnvVarLst*,  EnvVar*);
EnvVarLst* crsh_envlst();
EnvVar*    crsh_envvar(const char*, const char*);


// command line arguments
typedef std::string            ClArg;
typedef std::list<std::string> ClArgLst;

ClArgLst* crsh_arglst(EnvVarLst*,  EnvVar*);
ClArgLst* crsh_arglst();
ClArg*    crsh_clarg(const char*);

// test invocation definition
void crsh_test( const char* suite,
                const char* test,
                bool        success,
                EnvVarLst*  envp,
                const char* specimen,
                ClArgLst*   cmdlargs
              );

// run tests
void crsh_run(const char*, int);

#endif /* CRSH_H */
