/*
Hi Dan,
   I'm still having problems with K&R and the "-rose:C_only" option.
Here is my program kandr.c:

int main(argc, argv)
     int argc;
     char **argv;
{

  return 0;
}

Here is the output under 0.8.6a:

./defaultTranslator -rose:C_only -o kandr kandr.c
In build_EDG_CommandLine(): Option -c not found (compile AND link) set
autoInstantiation = true ...
In build_EDG_CommandLine(): autoInstantiation = true adding
--auto_instantiation -tused ...
Command-line error: instantiation mode option can be used only when
compiling
          C++

1 catastrophic error detected in this compilation.
Compilation terminated.
abort on exit from EDG front-end processing!
defaultTranslator:
/usr/casc/overture/bwhite/rose-0.8.6a/ROSE/src/frontend/EDG/EDG_3.3/src/host_envir.c:1512:
void exit_compilation(an_error_severity): Assertion `false' failed.
Abort

If I add the '-c' flag, it doesn't die:
./defaultTranslator -rose:C_only -c -o kandr kandr.c

But, of course, I don't get an executable.

[bwhite@tux06 defaultTranslator]$ file kandr
kandr: ELF 32-bit LSB relocatable, Intel 80386, version 1 (SYSV), not
stripped

As I sanity check, I do:
./defaultTranslator -o hi hi.c

While gives me a fine, executable hello world program.

Output is somewhat different under 0.8.4c:

Using the -c flag is the same.  But without it:

./defaultTranslator -rose:C_only -o kandr kandr.c
Command-line error: automatic instantiation mode can be used only when
          compiling C++

1 catastrophic error detected in this compilation.
Compilation terminated.

This isn't meant to be an obscure excerise.  I'd like to compile K&R
because the spec benchmarks which I'd like to run against the alias
analysis are written in K&R.

Thanks,
Brian

*/


int main(argc, argv)
     int argc;
     char **argv;
   {
     return 0;
   }
