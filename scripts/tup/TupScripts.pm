# Perl functions used by the tup-related scripts in this same directory
package TupScripts;
use strict;
use warnings;
use Config;
use Cwd;

our($arg0) = $0 =~ m(([^/\\]+)$);
our($osname) = $Config{osname} =~ /^MSWin/ ? "windows" : $Config{osname};

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

# Override the OS name for testing. Normally the OS name is obtained automatically.
sub osname {
    if (@_) {
	my($s) = @_;
	die "bad osname \"$s\"" unless $s =~ /^(linux|macos|windows)$/;
	$osname = $s;
    }
    return $osname;
}

########################################################################################################################
# File name operations
########################################################################################################################

# Given a file system name, split it into up to three parts: a leading component, a component separator, and the rest.
# At most one separator at a time is returned, so an input like "https://foo" will return ("https:", "/", "/foo") if the
# separator is "/".
sub first_file_component {
    my($input) = @_;
    return ("", "", "") if $input eq "";
    if ($osname eq "windows") {
	return ($1, $2, $3) if $input =~ /^(.*?)([\/\\])(.*)/;
    } else {
	return ($1, $2, $3) if $input =~ /^(.*?)(\/)(.*)/;
    }
    return ($input, "", ""); # no separator matched
}

# Same as calling first_file_component until its last return value.
sub last_file_component {
    my($path) = @_;
    while (1) {
	my($component, $separator, $rest) = first_file_component($path);
	return ($component, $separator) if $rest eq "";
	$path = $rest;
    }
}

# Returns a list of components and separators by calling first_file_component over and over.
# E.g., "/foo/bar" => (["", "/"], ["foo", "/"], ["bar", ""])
sub all_components {
    my($path) = @_;
    my(@ret);
    while ($path) {
	my($component, $separator, $rest) = first_file_component($path);
	push @ret, [$component, $separator];
	$path = $rest;
    }
    return @ret;
}

# Joins path components to form a path. Each component is a name and a speparator, either or both of which can be empty
# strings.
sub join_components {
    my(@parts) = @_;
    my($ret) = "";
    for my $part (@parts) {
	$ret .= $part->[0] . $part->[1];
    }
    return $ret;
}
    
# Remove "..", ".", and empty components from a name
sub canonical_name {
    my($path) = @_;
    my(@parts);
    while ($path ne "") {
	my($component, $separator, $rest) = first_file_component($path);
	if ($component eq '..' && @parts) {
	    pop @parts;
	} elsif ($component eq ".") {
	    # no-op
	} elsif ($component eq "" && @parts) {
	    # nothing
	} else {
	    push @parts, [$component, $separator];
	}
	$path = $rest;
    }

    if (@parts > 1) {
	$parts[-1][1] = "";
    } elsif (@parts == 1 && $parts[0][0] ne "") {
	$parts[0][1] = "";
    }

    my($ret) = join_components(@parts);
    $ret = '.' if $ret eq "";
    return $ret;
}

# Given a file name, return the directory part. Examples:
#    /foo/bar  => /foo         ""/ foo/ bar
#    /foo/bar/ => /foo/bar     ""/ foo/ bar/
#    /foo      => empty string ""/ foo
#    /         => empty string ""/
#    ./foo     => .            ./ foo
#    foo       => .            foo
sub directory_name {
    my($path) = @_;
    my(@parts);
    while ($path ne "") {
	my($component, $separator, $rest) = first_file_component($path);
	push @parts, [$component, $separator] if $rest ne "" || $separator ne "";
	$path = $rest;
    }

    # Remove the final separator
    if (@parts) {
	if ($parts[-1][0] eq "") {
	    pop @parts;
	} else {
	    $parts[-1][1] = "";
	}
    } else {
	return ".";
    }

    return join_components(@parts);
}
      
# Given a name in the file system, return the last component. I.e., the non-directory part.
sub base_name {
    my($path) = @_;
    my($last_component) = "";
    while ($path ne "") {
	my($component, $separator, $rest) = first_file_component($path);
	$last_component = $component if $component ne "";
	$path = $rest;
    }

    die "$arg0: invalid name \"$path\"\n" if $last_component eq "";
    return $last_component;
}

# Given a name in the file system, return just the last component of the path without the final dot or anything thereafter.
sub base_name_no_ext {
    my($path) = @_;
    my($component) = base_name($path);
    return $1 if $component =~ /(.*)\./;
    return $component;
}

# Convert a relative name to an absolute name
sub absolute_name {
    my($path, $root) = @_;
    if ($osname eq "windows") {
	my($drive, $rest) = $path =~ /^([a-zA-Z]:)?(.*)/;
	return $path if $rest =~ /^[\/\\]/;
	$root = Cwd::cwd() unless $root;
	my($separator) = (first_file_component($rest))[1];
	$separator = "\\" if $separator eq "";
	return $drive . $root . "\\" . $rest;
    } else {
	return $path if $path =~ /^\//;
	$root = Cwd::cwd() unless $root;
	return $root . "/" . $path;
    }
}

# Given the relative name of the root of the build tree, e.g., tup's $(TUP_CWD) variable, return the name of the current
# working directory relative to the root of the build tree.
sub cwd_build_relative {
    my($tup_cwd, $cwd) = @_;
    $cwd = Cwd::cwd() if $cwd eq "";
    my(@cwd) = all_components($cwd);
    my(@tup) = all_components($tup_cwd);
    my @upward = grep {$_ ne "" && $_ ne "."} map {$_->[0]} @tup;
    die "$arg0: build_relative_cwd: invalid \$(tup_cwd) or equivalent: \"$tup_cwd\"\n" if grep {$_ ne ".."} @upward;
    my(@cwdrel) = @cwd[scalar(@cwd)-scalar(@upward) .. scalar(@cwd)-1];
    return join_components(@cwdrel);
}

########################################################################################################################
# File types, by name
########################################################################################################################

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


########################################################################################################################


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


# Variable name for the compiler. "CXX" for C++ compilers, "CC" for C compilers, etc.
sub compiler_exe {
    my($lang) = @_; # from the &language function
    return "CXX" if $lang eq "c++" || $lang eq "cxx";
    return "UIC" if $lang eq "qtui";
    return "RCC" if $lang eq "qtrc";
    return "MOC" if $lang eq "moc";
    return uc($lang) . "C";
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

# Fix the a group specification by adding angle brackets around the last component of the path.
sub make_group_name {
    my($input) = @_;
    my(@parts) = all_components($input);
    pop @parts while @parts && $parts[-1][0] eq "";
    return "" unless @parts;
    $parts[-1][0] = "<" . $parts[-1][0] . ">" unless $parts[-1][0] =~ /^<.*>$/;
    $parts[-1][1] = "";
    return join_components(@parts);
}
	
1;
