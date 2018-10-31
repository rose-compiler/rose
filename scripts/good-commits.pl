#!/usr/bin/perl
# Classify commit messages based on how well they conform to ROSE policies.
# Commit messages are described here: [http://rosecompiler.org/ROSE_HTML_Reference/group__commit__messages.html]
use strict;

sub usage {
    print "usage: $0 [--histogram] GIT_LOG_SWITCHES...\n";
    exit(0);
}

# Commit message should begin with a parenthesized word or two to say what was changed. Returns false
# if the commit doesn't have a bad category, and an error message if it does have a bad category.
sub badTitleCategory {
    my($title) = @_;
    return "missing title category" unless $title =~ /^\((.+?)\)/;
    my $category = $1;
    return "title category too long" if length($category) > 20;
    return "category is not a word" unless $category =~ /^[A-Za-z]/;
    return;
}

# Return an error message if the title contains what looks like a JIRA issue.
# Jira issues should be in the body of the commit because they don't mean much
# to users that are looking at the titles.
sub badTitleIssue {
    my($title) = @_;
    return "jira issue should not be in title" if $title =~ /\b[A-Z]+-\d+/;
    return;
}

# Returns an error message if the commit title violates any policies.
sub badTitle {
    my($title) = @_;
    my($err) = badTitleCategory($title) || badTitleIssue($title);
    return $err if $err;
    return "title is too long" if length($title) > 80;
    return;
}

# Return an error message if the commit lacks at least one JIRA issue
sub badBodyIssue {
    my($body) = @_;
    return "body lacks JIRA issue line(s)" unless $body =~ /^[A-Z]+-\d+\s*$/m;
    return;
}

# Returns an error message if anything is wrong with the commit message body
sub badBody {
    my($body) = @_;
    return badBodyIssue($body);
}

# Returns an error message if a commit message violates any policies     
sub badMessage {
    my($message) = @_;
    my(@parts) = split /\n/, $message, 3;
    return "title and body not separated by a blank line" if @parts == 3 && $parts[1] =~ /\S/;
    my($title,$body) = (@parts[0,2]);
    return badTitle($title) || badBody($body);
}

# Run a GIT command and return the standard output as a single string without the final
# line feed
sub git {
    my($cmd) = "git " . join(" ", map {"'$_'"} @_);
    my($output) = join "", `$cmd`;
    chomp $output;
    return $output;
}

# Get info about each commit: author and message
sub getCommitInfo {
    my($hash) = @_;
    my($info) = git "log", "-1", "--format=%ae%n%B", $hash;
    my($author,$message) = split /\n/, $info, 2;
    $author =~ s/@.*//;
    return { author => $author, message => $message };
}

# Build a histogram for each user and display it
sub showHistogram {
    my %histogram;
    for my $hash (split /\n/, git "log", @_, '--format=%H') {
	my($commit) = getCommitInfo($hash);
	my($err) = badMessage($commit->{message});
	$histogram{$commit->{author}} ||= [0, 0]; # errors and non-errors
	++$histogram{$commit->{author}}[$err?0:1];
	print STDERR ".";
    }
    print STDERR "\n";

    printf "Percent Good/Total Author\n";
    for my $author (sort keys %histogram) {
	my($nbad,$ngood) = @{$histogram{$author}};
	my($total) = $ngood + $nbad;
	printf "%-7d %4d/%-4d %s\n", 100 * $ngood / $total, $ngood, $total, $author;
    }
}

# Show errors for specified commits
sub showEachCommit {
    for my $hash (split /\n/, git "log", @_, '--format=%H') {
	my($commit) = getCommitInfo($hash);
	my($err) = badMessage($commit->{message}) || "good";
	printf "%8s %-32s %s\n", substr($hash,0,8), $commit->{author}, $err;
    }
}
    
####################################################################################################

if (grep {$_ eq "--histogram"} @ARGV) {
    showHistogram grep {$_ ne "--histogram"} @ARGV;
} else {
    showEachCommit @ARGV;
}

