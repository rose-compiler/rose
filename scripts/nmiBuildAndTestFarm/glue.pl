#!/usr/bin/perl
use Config;
use Sys::Hostname;
use Getopt::Long;
use File::Spec;
use Cwd;
use Time::localtime;

require 'env.pl';

my ( $platform, $taskhook ) = ( 'NOT DEFINED', 'NOT DEFINED' );
my $fail;

GetOptions(
   'taskhook=s' => \$taskhook,
   'fail'       => \$fail,
);
my @args  = @ARGV;
my %table = (
    platform_info     => \&platform_info,
    dump_environment  => \&dump_environment,
    configure         => \&configure,
    make              => \&make,
    check             => \&check,
    remote_declare    => \&generate_tasklist_nmi,
    remote_post       => \&generate_results_file,
);

# NMI Macros expected
# NMI_ARGS_<taskname> -- arguments for a task
# NMI_ARGS_<taskname>_<platform> -- arguments for a task running on a particular platform
# NMI_TASKLIST -- Lists of remote tasks to run format = "taskname1,taskname2,..."
# NMI_TIMEOUT_<taskname> -- timeout value for a task
# NMI_TIMEOUT_<taskname>_<platform> -- timeout value for a task running on a particular platform
# _NMI_TASKNAME -- Used as the taskname if defined otherwise the value of --taskhook is used

my $startdir = cwd();
my $builddir = "build";
$platform = $ENV{'NMI_PLATFORM'};

if ( $platform =~ /x86(?!_[0-9]{2})/ )
{
   $ENV{'LD_LIBRARY_PATH'} = $ENV{'JAVA_HOME'}."/jre/lib/i386/server:".$ENV{'LD_LIBRARY_PATH'};
   $ENV{'DYLD_LIBRARY_PATH'} = $ENV{'JAVA_HOME'}."/jre/lib/i386/server:".$ENV{'DYLD_LIBRARY_PATH'};
   $ENV{'LD_LIBRARY64_PATH'} = $ENV{'JAVA_HOME'}."/jre/lib/i386/server:".$ENV{'LD_LIBRARY64_PATH'};
   $ENV{'LD_LIBRARYN32_PATH'} = $ENV{'JAVA_HOME'}."/jre/lib/i386/server:".$ENV{'LD_LIBRARYN32_PATH'};
   $ENV{'LIBPATH'} = $ENV{'JAVA_HOME'}."/jre/lib/i386/server:".$ENV{'LIBPATH'};
   $ENV{'SHLIB_PATH'} = $ENV{'JAVA_HOME'}."/jre/lib/i386/server:".$ENV{'SHLIB_PATH'};
}
elsif ( $platform =~ /x86_[0-9]{2}/ )
{
   $ENV{'LD_LIBRARY_PATH'} = $ENV{'JAVA_HOME'}."/jre/lib/amd64/server:".$ENV{'LD_LIBRARY_PATH'};
   $ENV{'DYLD_LIBRARY_PATH'} = $ENV{'JAVA_HOME'}."/jre/lib/amd64/server:".$ENV{'DYLD_LIBRARY_PATH'};
   $ENV{'LD_LIBRARY64_PATH'} = $ENV{'JAVA_HOME'}."/jre/lib/amd64/server:".$ENV{'LD_LIBRARY64_PATH'};
   $ENV{'LD_LIBRARYN32_PATH'} = $ENV{'JAVA_HOME'}."/jre/lib/amd64/server:".$ENV{'LD_LIBRARYN32_PATH'};
   $ENV{'LIBPATH'} = $ENV{'JAVA_HOME'}."/jre/lib/amd64/server:".$ENV{'LIBPATH'};
   $ENV{'SHLIB_PATH'} = $ENV{'JAVA_HOME'}."/jre/lib/amd64/server:".$ENV{'SHLIB_PATH'};
}

# Figure out the name of the task
my $name = $taskhook;
$name = $ENV{'_NMI_TASKNAME'}
  if defined $ENV{'_NMI_TASKNAME'};

# Look up the function in the table not that the table keys are
# prefixes so that multiple calls can be made with different
# arguments.  $found is used to handle the case where the task name is not recognized

my $return;
my $found = 0;
for my $k ( keys %table ) {
   if ( $name =~ m!^\s*$k! ) {
      $return = &{ $table{$k} }($name);\
      $found++;
      last;
   }
}

die "Task $name not found" unless $found;
die "Task $name has failed" if $return;
exit 0;

# task to display platform information
sub platform_info {
   print "Environment Variables\n";
   for my $e ( sort keys %ENV )
   {
      print "\t$e=" . $ENV{$e} . "\n";
   }
   print "\n";

   print "Host: ";
   system("hostname");
   print "\n";

   print "program versions:\n\n";

   print "Gnu Compiler edition:\n";
   system( "g++ --version" );
   print "\n";
   print "libtool:\n";
   system( "libtool --version" );
   print "\n";
   print "autoconf:\n";
   system( "autoconf --version" );
   print "\n";
   print "automake:\n";
   system( "automake --version" );
   print "\n";

   return 0;
}

# This should produce env.sh, which, when sourced, will reproduce the
# environment used by the run.
sub dump_environment {
	open FILE, ">env.sh";
    for my $e ( sort keys %ENV )
    {
	  # Really we should export the entire environment, but I haven't figured
	  # out how to do this with variables whose names include '#'.
      print FILE "export $e='" . $ENV{$e} . "'\n" unless $e =~ /[#:]/;
    }
	close FILE;
	
	print "Dumped environment to env.sh\n";

    return 0;
}

# task to run configure
sub configure {
   my ($taskhook) = @_;
   my $args = assemble($taskhook,"args");
   my $command = "$startdir/trunk/configure $args >& /dev/stdout | tee configure.log; exit \${PIPESTATUS[0]}";
   my $return = 0;
   chdir $startdir;
   
   chdir 'trunk';
   print "Creating configure script";
   $return = system( "./build >& /dev/stdout | tee build.log; exit \${PIPESTATUS[0]}" );
   chdir $startdir;

   if( $return != 0 )
   {
      return $return;
   }

   mkdir $builddir or die "Error creating directory \"$builddir\" : $!" unless -d $builddir;
   print "Created build directory $startdir/$builddir\n";

   print "Executing [$command] in $builddir\n";
   chdir $builddir;
   $return = system($command);
   chdir $startdir;
   return $return
}

#task to run make
sub make {
   my ($taskhook) = @_;
   my $args = assemble($taskhook,"args");
   my $command = "make $args >& /dev/stdout | tee make.log; exit \${PIPESTATUS[0]}";
   print "Executing [$command] in builddir($builddir)\n";
   chdir $builddir;
   my $return = system($command);
   chdir $startdir;
   return $return
}

# task to run make check
sub check {
   my ($taskhook) = @_;
    my $args = assemble($taskhook,"args");
   chdir $startdir;
   my $command = "make $args check >& /dev/stdout | tee check.log; exit \${PIPESTATUS[0]}";
   print "Executing [$command] in builddir ($builddir)\n";
   chdir $builddir;
   my $return = system($command);
   chdir $startdir;
   return $return
}

sub generate_tasklist_nmi {
   my ($taskhook) = @_;
   my @tasklist = split /,/, $ENV{'NMI_TASKLIST'};
   
   open LIST, ">tasklist.nmi";
   print "Generating tasklist.nmi for $taskhook\n";
   for my $l (@tasklist) {
      my $time = assemble($l,"timeout");
      $time = 1 unless defined $time;
      print LIST "$l $time\n";
      print "$l $time\n";
   }
   close LIST;
   return 0;
}

sub generate_results_file {
#   my ($taskhook) = @_;
    print "Creating results.tar.gz file for $taskhook\n";
    system("tar czvf results.tar.gz --exclude='.svn' env.sh build trunk");
}

sub assemble {
   my ($taskhook,$type) = @_;
   %assemble_types = 
   (
      args=>'NMI_ARGS_',
      timeout => 'NMI_TIMEOUT_',
   );
   my $prefix = $assemble_types{$type};
   die "[$type] not recognized" unless defined $prefix;
   my $args = $ENV{($prefix . $taskhook)};
   $args = "" unless defined $args;
   if (defined $ENV{($prefix . $taskhook . "_" . $platform)}) {
      $args .= " " . $ENV{($prefix . $taskhook . "_" . $platform)};
   }
   return $args;
}
