# Functions for comparing version numbers
use strict;

# Compare two things. If they're both integers, compare as integers, otherwise compare as strings.
# Returns -1, 0, or 1 depending on whether $a is less-than, equal-to, or grater-than $b.
sub compare {
    my($a, $b) = @_;
    if (!ref($a) && !ref($b)) {
	if ($a =~ /^\d+$/ && $b =~ /^\d+$/) {
	    return $a <=> $b;
	} else {
	    return $a cmp $b;
	}
    } else {
	die unless ref($a) eq 'ARRAY' && ref($b) eq 'ARRAY';
	die unless @$a == @$b;
	for (my $i = 0; $i < @$a; ++$i) {
	    my($c) = compare($a->[$i], $b->[$i]);
	    return -1 if $c < 0;
	    return 1 if $c > 0;
	}
	return 0;
    }
}

# Given two version numbers, split them into arrays of equal length by padding the shorter one with zeros.
sub pad {
    my($a, $b) = @_;
    my @a = split /\./, $a;
    my @b = split /\./, $b;
    push @b, "0" while @a > @b;
    push @a, "0" while @b > @a;
    return (\@a, \@b);
}

# True if two version numbers are equal. The shorter version is padded with zeros first, which means that
# "5" and "5.0" are considered equal. If you want string equality, use "$a eq $b" instead of veq($a, $b).
sub veq {
    return compare(pad(@_)) == 0;
}

# True if $a is greater than $b
sub vgt {
    return compare(pad(@_)) > 0;
}
	   
# True if $a is greater than or equal to $b
sub vge {
    return compare(pad(@_)) >= 0;
}

# True if $a is less than $b
sub vlt {
    return compare(pad(@_)) < 0;
}

# True if $a is less than or equal to $b
sub vle {
    return compare(pad(@_)) <= 0;
}

1;
