# Perl functions used by the tup-related scripts in this same directory
package TupScripts;
use strict;
use warnings;
use Cwd;

our($arg0) = $0 =~ m(([^/]+)$);


# Pretty-prints the POD at the top of the main perl script
sub help {
  local $_ = `(pod2man $0 |nroff -man) 2>/dev/null` ||
             `pod2text $0 2>/dev/null` ||
             `sed -ne '/^=pod/,/^=cut/p' $0 2>/dev/null`;
  die "$0: see source file for documentation\n" unless $_;
  if (open LESS, "|less -R") {
    print LESS $_;
    close LESS;
  } else {
    print $_;
  }
};

# Remove "..", ".", and empty components from a name
sub canonical_name {
    my(@parts) = split /\/+/, $_[0];
    my(@ret);
    for my $part (@parts) {
	if ($part eq ".." && @ret) {
	    pop @ret;
	} elsif ($part eq ".") {
	    # nothing
	} else {
	    push @ret, $part;
	}
    }
    @ret = (".") unless @ret;
    return join "/", @ret;
}

# Given a file name, return the directory part. Examples:
#    /foo/bar  => /foo
#    /foo/bar/ => /foo/bar
#    /foo      => empty string
#    /         => empty string
#    ./foo     => .
#    foo       => .
sub directory_name {
    return $1 if $_[0] =~ /^(.*)\//;
    return ".";
}
      
# Given a name in the file system, return the last component. I.e., the non-directory part.
sub base_name {
    my @components = split /\/+/, $_[0];
    pop @components while @components && !$components[-1];
    die "$arg0: invalid name \"$_[0]\"\n" unless @components;
    return $components[-1];
}

# Given a name in the file system, return just the part after the last
# slash and before the last dot.
sub base_name_no_ext {
    my $final = base_name($_[0]);
    $final = $1 if $final =~ /(.*)\./;
    return $final;
}

# Convert a relative name to an absolute name
sub absolute_name {
    my($name, $root) = @_;
    return $name if $name =~ /^\//;
    $root = Cwd::cwd() unless $root;
    return "$root/$name";
}

# Given the relative name of the root of the build tree, e.g., tup's $(TUP_CWD) variabe, return the
# name of the current working directory relative to the root of the build tree.
sub cwd_build_relative {
    my ($tup_cwd) = @_;
    my @cwd = split /\//, Cwd::cwd();
    my @upward = grep {$_ ne "" && $_ ne "."} split /\//, $tup_cwd;
    die "$arg0: build_relative_cwd: invalid \$(tup_cwd) or equivalent: \"$tup_cwd\"\n" if grep {$_ ne ".."} @upward;
    my @cwdrel = @cwd[scalar(@cwd)-scalar(@upward) .. scalar(@cwd)-1];
    return @cwdrel ? join("/", @cwdrel) : ".";
}

# Escape a string for the shell
sub shell_escape {
    local($_) = @_;

    # No escapes necessary if non-empty and contains no shell meta characters
    return $_ if /^[-._+=\w\/]+$/;

    # For readability, |var=v1 v2| becomes |var='v1 v2'| rather than |'var=v1 v2'|
    # Similarly for switches: |--switch=v1 v2| becomes |--switch='v1 v2'|
    return $1 . shell_escape($2) if /^(-*\w+=)(.*)/;

    s/'/'"'"'/g;
    return "'$_'";
}

# Return the language for a file based only on the file name (because the file might not exist or might be empty or
# might have incorrect contents). The by-name restriction is because this needs to be called to create Tup rules, which
# happens before files are created.
sub language {
    my($dflt, $file) = @_;

    # Some things are so obvious that we shouldn't use the $dflt
    return $dflt || "c"       	if $file =~ /\.c$/;
    return $dflt || "c++"     	if $file =~ /\.(C|cc|cxx|cpp)$/;
    return $dflt || "d"       	if $file =~ /\.d$/;
    return $dflt || "header"  	if $file =~ /\.(h|hh|H|hpp|hxx)$/;
    return "library" 	        if $file =~ /\.(so|a|dll)$/;
    return "library"            if $file =~ /^(.*\/)?lib[^.\/][^\/]*$/;
    return $dflt || "shell"   	if $file =~ /\.sh$/;
    return "object"  	        if $file =~ /\.o$/;
    return "executable"         if $file =~ /\.exe$/;
    return $dflt || "qtui"      if $file =~ /\.ui$/;    # Qt GUI description file (XML) from Qt's "designer" tool
    return $dflt || "qtrc"      if $file =~ /\.qrc$/;   # Qt Resource description file (XML) from Qt's "creator" tool
    return $dflt || $1          if $file =~ /\.([^\/]+)$/;
    return $dflt;
}

# Variable name for the compiler. "CXX" for C++ compilers, "CC" for C compilers, etc.
sub compiler_exe {
    my($lang) = @_; # from the &language function
    return "CXX" if $lang eq "c++" || $lang eq "cxx";
    return "UIC" if $lang eq "qtui";
    return "RCC" if $lang eq "qtrc";
    return "MOC" if $lang eq "moc";
    return uc($lang) . "C";
}

# True if file is a source language like C or C++
sub is_source_code {
    my($file) = @_;
    my($lang) = language("", $file);
    return ($lang eq "c" || $lang eq "c++" || $lang eq "d" || $lang eq "header");
}

# True if file is a library, static or shared
sub is_library {
    return language("", $_[0]) eq "library";
}

# True if file is an object file
sub is_object {
    return language("", $_[0]) eq "object";
}

# True if file is a Qt UI file. I.e., XML generated from Qt's "designer" program.
sub is_qt_ui {
    return language("", $_[0]) eq "qtui";
}

# True if file is a Qt resource description file generated from Qt's "creator" program.
sub is_qt_resource {
    return language("", $_[0]) eq "qtrc";
}

# Fix bin names so "{foo}.C" becomes just "{foo}". The ".C" is necessary because it
# tells various tup-scripts that the bin contains *.C files.
sub fix_bins {
    my @result;
    for my $item (@_) {
	if ($item =~ /^(\{\w+\})\./) {
	    push @result, $1;
	} else {
	    push @result, $item;
	}
    }
    return wantarray ? @result : $result[0];
}

# Turn a bin like "{foo}.ext" (the ".ext" is optional) into just "foo". If the pattern isn't matched
# then return the argument as-is.
sub make_bin_variable {
    return $1 if $_[0] =~ /^\{(\w+)\}(\.[^\/]*)?$/;
    return $_[0];
}

# Fix group names so "../../<foo>.C" becomes "../../<foo>", without affecting other arguments.
sub fix_groups {
    my @result;
    for my $item (@_) {
	if ($item =~ /^(.*<\w+>)\.[^\/]*$/) {
	    push @result, $1;
	} else {
	    push @result, $item;
	}
    }
    return wantarray ? @result : $result[0];
}

# Remove extensions from all groups and bins
sub fix_groups_bins {
    return fix_bins fix_groups @_;
}

# Given a group like "../../<foo>.C" (the path and extension are optional) return just "foo".
# If the argument doesn't follow this pattern, return the argument as-is.  If the $tup_var is
# true then return "%<foo>" instead of "foo".
sub make_group_variable {
    my($in, $tup_var) = @_;
    my($var) = $in =~ /<(\w+)>(\.[^\/]*)?$/ ? $1 : $in;
    $var = "%<$var>" if $tup_var && $var ne $in;
    return $var;
}
	
# Turn a file, bin, or group into something that's a suitable variable
sub make_variable {
    my($v) = make_bin_variable(make_group_variable($_[0]));
    return $v if $v ne $_[0];
    $v = base_name_no_ext($v);
    $v =~ s/[^-\w]/_/;
    return $v;
}

# Organize source file names by language. Return value is a hash
# indexed by the language and whose value is an array reference for
# the corresponding source file names.
#
# C/C++ source files are indicated by "c" or "c++", but header files
# are grouped under "header".
#
# Unrecognized file names are grouped by their extension(s). Files
# with no extension are grouped under the empty string.
#
# Remember, this is only based on file extensions. It cannot be based
# on file content since it's called with file names that might not
# actually exist yet (such as when building Tup rules).
sub by_language {
    my($dflt, @files) = @_;
    my %ret;
    for my $file (@files) {
	my $lang = language($dflt, $file);
	$ret{$lang} ||= [];
	push @{$ret{$lang}}, fix_groups fix_bins $file;
    }
    return %ret;
}

# Given a list of inputs, return a suitable variable, usually "%f".
sub tup_input_variable {
    my @inputs = @_;
    if (@inputs == 1) {
	my $groupvar = make_group_variable($inputs[0]);
	return "\%<$groupvar>" if $groupvar ne $inputs[0];
    }
    return '%f';
}
	
1;
