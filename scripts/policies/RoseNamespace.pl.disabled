#!/usr/bin/perl
# DO NOT DISABLE without first checking with a ROSE core developer
my $desc = <<EOF;
The \$ROSE/src/Rose directory is somewhat more structured than many other
parts of ROSE because the files here are organized according to
namespaces. For instance, given the namespace Rose::Foo::Bar, the following
are required:

1. The header src/Rose/Foo/Bar.h must exist. It must use ".h" as its
   file name extension.

2. If Rose::Foo::Bar contains sub-namespaces then the directory
   src/Rose/Foo/Bar must exist to hold those namespaces.  It may exist even if
   Rose::Foo::Bar has no sub-namespaces.

3. If src/Rose/Foo/Bar directory exists, then all implementation (.C) and
   header files (.h) must live in that directory. Otherwise the implementation
   may be named src/Rose/Foo/Bar.C.

4. There must exist a header named src/Rose/Foo/Bar.h. This header directly or
   indirectly declares everything that's necessary for a user to use
   Rose::Foo::Bar. If the src/Rose/Foo/Bar directory exists then
   Rose::Foo::Bar's headers are in that directory (see previous requirement)
   and src/Rose/Foo/Bar.h should simply #include them all.

5. The parent namespace header (src/Rose/Foo.h) must exist and must #include
   the namespace header (src/Rose/Foo/Bar.h). The only exception is that we
   don't (currently) have src/Rose.h since all users are required to "#include
   <rose.h>" and also having "#include <Rose.h>" would be confusing.

6. All #include's must be of the <> variety, not the "" variety. This is
   because most compilers treat the two forms differently and we want a
   consistent user experience for all headers. The standards don't define how
   they're different, or whether they're even different, but most compilers
   search the current directory when using quotes. Searching the current
   directory for ROSE headers is never necessary when compiling the ROSE
   library, and is plain wrong when compiling user code. Therefore use angle
   brackets.

7. Every header must be protected from multiple inclusion within a single
   compilation unit by using "#ifndef X" followed by "#define X" where X is
   described in the next requirement. Do not use "#pragma once"; it's not
   portable.

8. The CPP symbol used for multiple-inclusion-protection (see previous
   requirement) must be of the form "ROSE_*_H" where "*" is the fully
   qualified namespace (except the ::Rose part) with "_" instead of "::". For
   instance, ROSE_Foo_Bar_H. This consistency is so that users can check for
   ROSE features after they #include a namespace header, and conditionally
   compile their code based on the present/absent features. Furthermore, all
   ROSE global symbols must start with the string "ROSE_" (not "Rose_")
   checked by a different policy checker.

9. Namespaces must use PascalCase (https://en.wikipedia.org/wiki/Pascal_case),
   also known as upper camel case, even for abbreviations. This is for
   consistent user experience and because names like AstToPdf and CfgCsvIo are
   easier to read than ASTToPDF and CFGCSVIO.  It is best to avoid using
   abbreviations except where they're already common in the ROSE API.

The following policy violations were detected:
EOF

BEGIN {push @INC, $1 if $0 =~ /(.*)\//}
use strict;
use FileLister;
use Policies;

my $nfail = 0;
my %notExists; # list of headers that were found to not exist

# True if the specified header is deprecated using the approved deprecation mechanism.
sub headerIsDeprecated {
    my($filename) = @_;
    my($isDeprecated);
    open HEADER, "<$filename" or return;
    while (<HEADER>) {
	if (/ROSE_PRAGMA_MESSAGE\("This header is deprecated/) {
	    $isDeprecated = 1;
	    last;
	}
    }
    close HEADER;
    return $isDeprecated;
}

# The symbol used to protect this header from mutliple inclusion. The return value
# is a pair consisting of the #ifndef symbol and the following #define symbol.
sub multiInclusionProtectionSymbols {
    my($filename) = @_;
    open HEADER, "<$filename" or return;
    while (<HEADER>) {
	if (/^\s*#\s*ifndef\s+(\w+)/) {
	    my $ifndef = $1;
	    while (<HEADER>) {
		if (/^\s*#\s*define\s+(\w+)/) {
		    my $def = $1;
		    close HEADER;
		    return ($ifndef, $def);
		}
	    }
	    close HEADER;
	    return ($ifndef, undef);
	}
    }
    close HEADER;
    return ();
}
			
my $files = FileLister->new(@ARGV);
while (my $filename = $files->next_file) {
    if ($filename =~ /^(.*\/src)\/(Rose\/.*)\.(h|hpp|h\+\+|H)/) {
	my ($root, $nsFile, $extension) = ($1, $2, $3);
	my @nsParts = split("/", $nsFile);
	my $ns = join("::", @nsParts);

	# If this header is deprecated, then none of these rules apply
	next if headerIsDeprecated($filename);

	# All public header files should use ".h" for their file extension, otherwise users become confused
	# whether it's .h, .hpp, .h++, .H, etc.
	if ($extension ne "h") {
	    print $desc unless $nfail++;
	    print "  $filename should use \".h\" extension\n";
	}

	# Check that the namespace is capitalized correctly. Abbreviations should have only the
	# first letter capitalized, as in "AstToPdfConverter" rather than "ASTToPDFConverter", or
	# "CfgCsvIo" rather than "CFGCSVIO". The way we check this is we look for names that have
	# three or more consecutive capital letters.
	my $nsLast = $nsParts[-1];
	$nsLast =~ s/[A-Z]/X/g;
	if ($nsLast =~ /XXX|_|-/ || $nsLast eq "XX") {
	    print $desc unless $nfail++;
	    print "  $filename should use PascalCase, not \"$nsParts[-1]\"\n";
	} else {
	    # Check the the multi-inclusion-protection symbol is correct
	    my($expectedMipSymbol) = join("_", "ROSE", @nsParts[1 .. $#nsParts], "H");
	    my($ifndef,$def) = multiInclusionProtectionSymbols($filename);
	    if ($ifndef eq "") {
		print $desc unless $nfail++;
		print "  $filename does not have multi-inclusion protection #ifndef/#define\n";
	    } elsif ($ifndef ne $expectedMipSymbol) {
		print $desc unless $nfail++;
		print "  $filename uses wrong multi-inclusion protection symbol $ifndef (should be $expectedMipSymbol)\n";
	    } elsif ($def eq "") {
		print $desc unless $nfail++;
		print "  $filename has incorrect multi-inclusion protection: missing #define\n";
	    }
	}

	# The parent header must exist.
	my $parentHeader = join("/", $root, @nsParts[0 .. $#nsParts-1]) . ".h";
	if (! -r $parentHeader) {
	    if ($#nsParts eq 1 && $nsParts[0] eq "Rose") {
		# For the time being, there is no src/Rose.h file because it would be confusing
		# to have both <rose.h> and <Rose.h>, especially on file systems that don't follow
		# POSIX specifications (like macOS).
	    } elsif (exists($notExists{$parentHeader})) {
		# already printed this error
	    } else {
		$notExists{$parentHeader} = 1;
		print $desc unless $nfail++;
		print "  $parentHeader does not exist (needed for $filename)\n";
	    }
	    next;
	}

	# There must be a "#include <" . $nsFile . ".h>" in the parent
	my $foundInclude = 0;
	open PARENT, "<$parentHeader";
	while (<PARENT>) {
	    if (my($sep, $name) = /^\s*#\s*include\s*([<"])(.*)[">]/) {
		if ($name eq "$nsFile.h") {
		    if ($sep ne "<") {
			print $desc unless $nfail++;
			print "  $parentHeader should use <> not \"\" when including $nsFile.h\n"
		    }
		    $foundInclude++;
		}
	    }
	}
	close PARENT;
	if ($foundInclude eq 0) {
	    print $desc unless $nfail++;
	    print "  $parentHeader does not #include <$nsFile.h>\n";
	} elsif ($foundInclude gt 1) {
	    print $desc unless $nfail++;
	    print "  $parentHeader #includes $nsFile more than once\n";
	}
    }
}

exit($nfail > 0 ? 1 : 0);
