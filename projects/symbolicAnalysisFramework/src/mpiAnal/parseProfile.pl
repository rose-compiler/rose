#!/usr/local/bin/perl

if($#ARGV<0) { die; }
my $fName = $ARGV[0];

open(my $profile, "<$fName") || die "Error opening file \"$fName\" for reading! $!";
my @funcNest=();
my %counters=();
while(my $line=<$profile>)
{
	chomp($line);
	#print "line=\"$line\"\n";
	if($line =~ /^([a-zA-Z]+)\(([^)]*)\) <<</)
	{
		#print "function=$1, args=$2\n";
		push(@funcNest, $1);
	}
	elsif($line =~ /^([a-zA-Z]+)\(([^)]*)\) >>> (.*)/)
	{
		#print "function=$1, args=$2, counters=$3\n";
		#print "#funcNest=$#funcNest\n";
		
		my $funcStr="";
		foreach my $func (reverse @funcNest)
		{
			$funcStr.="${func}_";
		}
		#print "funcStr=$funcStr, #funcNest=$#funcNest\n";
		
		my $newCount = addCounter(\@funcNest, 0, \%counters, "count", 1);
		my @counterStrs = split(/,/, $3);
		foreach my $counterStr (@counterStrs)
		{
			#print "counterStr=$counterStr\n";
			if($counterStr =~ /([a-zA-Z]+)=([eE0-9.-]+)/)
			{ 
				#print "    $1 => $2\n";
				#$counters{$funcStr}{$1}+=$2;
				my $ret;
				if($1 eq "time")
				{ $ret=addCounter(\@funcNest, 0, \%counters, $1, $2); }
				else
				{ $ret=avgCounter(\@funcNest, 0, \%counters, $1, $2, $newCount-1); }
				#print "$1 = $2 ret=$ret newCount=$newCount\n";
			}
			else
			{ die; }
		}
		
		
		#my @hashkeys = keys %counters;
		#print "main() #hashkeys=$#hashkeys\n";
		
		#print "func=$1 funcNest=@funcNest\n";
		my $lastFunc = pop(@funcNest);
		#if($lastFunc ne $1) { die; }
		while($lastFunc ne $1) { $lastFunc = pop(@funcNest); }
	}
}

my $totalTime = printCounter(\%counters);
print "totalTime = $totalTime\n";
print "-----\n";
my %summary = printVarSum([], \%counters, "count", "transitiveClosure");
print "Total transitiveClosure: ";
foreach my $key (keys %summary) { print "$key=$summary{$key} "; } print "\n";
#print "Average transitiveClosure numVars = ",($summary{numVars}/$summary{count}),"\n";

#print "-----\n";
#my $numVarsAvg = printVarWeightedSum(\%counters, "numVars", "transitiveClosure") / $counts;
#print "Average transitiveClosure numVars = $numVarsAvg\n";
#
#print "-----\n";
#my %summary = printVarSum([], \%counters, "time", "transitiveClosure");
#print "Total transitiveClosure ";
#foreach my $key (keys %summary)
#{ print "$key=$summary{$key} "; }
#print "\n";
#die;
print "=====\n";
my @badParents = ("transitiveClosure");
my %summary = printVarSum(\@badParents, \%counters, "time", "isFeasible");
print "Total isFeasible-transitiveClosure";
foreach my $key (keys %summary) { print "$key=$summary{$key} "; } print "\n";

print "-----\n";
@badParents = ("transitiveClosure", "isFeasible");
my %summary = printVarSum(\@badParents, \%counters, "time", "localTransClosure");
print "Total localTransClosure-(isFeasible, transitiveClosure)";
foreach my $key (keys %summary) { print "$key=$summary{$key} "; } print "\n";

print "=====\n";
@badParents = ("transitiveClosure");
my %summary = printVarSum(\@badParents, \%counters, "time", "localTransClosure");
print "Total localTransClosure-transitiveClosure)";
foreach my $key (keys %summary) { print "$key=$summary{$key} "; } print "\n";

print "-----\n";
@badParents = ("transitiveClosure", "localTransClosure");
my %summary = printVarSum(\@badParents, \%counters, "time", "isFeasible");
print "Total isFeasible-(localTransClosure, transitiveClosure)";
foreach my $key (keys %summary) { print "$key=$summary{$key} "; } print "\n";

print "#####\n";
@badParents = ();
my %summary = printVarSum(\@badParents, \%counters, "time", "localTransClosure");
print "Total localTransClosure";
foreach my $key (keys %summary) { print "$key=$summary{$key} "; } print "\n";

print "-----\n";
@badParents = ("localTransClosure");
my %summary = printVarSum(\@badParents, \%counters, "time", "transitiveClosure");
print "Total transitiveClosure-localTransClosure";
foreach my $key (keys %summary) { print "$key=$summary{$key} "; } print "\n";

print "-----\n";
@badParents = ("transitiveClosure", "localTransClosure");
my %summary = printVarSum(\@badParents, \%counters, "time", "isFeasible");
print "Total isFeasible-(transitiveClosure,localTransClosure)\n";
foreach my $key (keys %summary) { print "$key=$summary{$key} "; } print "\n";
foreach my $key (keys %summary) { if($key ne "time" && $key ne "count") { $summary{$key}/=$summary{"count"}; } }
foreach my $key (keys %summary) { print "$key=$summary{$key} "; } print "\n";


#foreach my $funcStr (keys %counters)
#{
#	foreach my $counter (keys %{$counters{$funcStr}})
#	{
#		print "$funcStr | $counter => $counters{$funcStr}{$counter}\n";
#	}
#}

sub addCounter
{
	my ($list, $index, $subhash, $var, $value, $indent) = @_;
	
	#if($index==0)
	#{
	#	print "${indent}addCounter() #list=$#{@$list} index=$index, list[$index]=$$list[$index]\n";
	#}
	     
	if($index<$#{@$list})
	{
		my $key = $$list[$index];
		return addCounter($list, $index+1, \%{$$subhash{$key}}, $var, $value, $indent."    ");
	}
	elsif($index==$#{@$list})
	{
		my $key = $$list[$index];
		$$subhash{$key}{"$var"} += $value;
		return $$subhash{$key}{"$var"};
		#print "${indent}addCounter() var=$var value=$value, subhash{$key}{\"$var\"}=",$$subhash{$key}{"$var"},"\n";
	}
}

sub avgCounter
{
	my ($list, $index, $subhash, $var, $value, $oldCount, $indent) = @_;
	
	#if($index==0)
	#{
	#	print "${indent}addCounter() #list=$#{@$list} index=$index, list[$index]=$$list[$index]\n";
	#}
	     
	if($index<$#{@$list})
	{
		my $key = $$list[$index];
		return avgCounter($list, $index+1, \%{$$subhash{$key}}, $var, $value, $indent."    ");
	}
	elsif($index==$#{@$list})
	{
		my $key = $$list[$index];
		#print "old value = $$subhash{$key}{$var}\n";
		$$subhash{$key}{$var} = (($$subhash{$key}{$var} * oldCount) + $value)/(oldCount+1);
		#print "new value = $$subhash{$key}{$var}\n";
		return $$subhash{$key}{$var};
		#print "${indent}addCounter() var=$var value=$value, subhash{$key}{\"$var\"}=",$$subhash{$key}{"$var"},"\n";
	}
}

sub printCounter
{
	my ($subhash, $prefix, $indent) = @_;
	
	#print "printCounter() subhash=$subhash\n";
	#my @hashkeys = keys %$subhash;
	#print "printCounter() #hashkeys=$#hashkeys\n";
	
	# Print all the variables associated with the current call stack
	print "${prefix} ";
	my $vals=0;
	foreach my $key (keys %$subhash)
	{
		#print "${indent}printCounter() key=$key\n";
		#if(!(ref $key eq 'HASH'))
		if(!($$subhash{$key} =~ /HASH/))
		{
			if($vals>0)
			{ print ", "; }
			print "$key=$$subhash{$key}";
			$vals++;
		}
	}
	print "\n";
	
	my $sum=0;
	# Print deeper call stacks
	foreach my $key (keys %$subhash)
	{
		#print "${indent}printCounter() key=$key\n";
		#if(!(ref $key eq 'HASH'))
		if($$subhash{$key} =~ /HASH/)
		{
			#print "$key ";
			$sum+=printCounter($$subhash{$key}, $indent."$key() ", $indent."    ");
		}
	}
	
	if($vals>0)
	{ return $$subhash{"time"}; }
	else
	{ return $sum; }
}

sub printVarSum
{
	my ($badParents, $subhash, $var, $func, $doExit, $prefix, $indent) = @_;
	#print "${indent}printVarSum($#{@$badParents}, $subhash, $var, $func, $doExit, $prefix, $indent)\n";
	#print "printCounter() subhash=$subhash\n";
	#my @hashkeys = keys %$subhash;
	#print "printCounter() #hashkeys=$#hashkeys\n";
	
	my $foundIncl=0;
	my $foundSubFunc=0;
	# Check if the function we want to include/exclude exists at this level
	# Also check if this level has sub-functions
	foreach my $key (keys %$subhash)
	{
		#print "${indent}printCounter() key=$key\n";
		#if(!(ref $key eq 'HASH'))
		if($$subhash{$key} =~ /HASH/)
		{
			$foundSubFunc=1;
			if($key eq $func)
			{
				$foundIncl=1;
				last;
			}
		}
	}
	#print "${indent}include=$include, foundSubFunc=$foundSubFunc, foundIncl=$foundIncl\n";
	
	# Print all the variables associated with the current call stack
	if($doExit)
	{
		my %summary=();
		print "${indent}${prefix} ";
		my $vals=0;
		foreach my $key (keys %$subhash)
		{
			#print "${indent}printCounter() key=$key\n";
			#if(!(ref $key eq 'HASH'))
			if(!($$subhash{$key} =~ /HASH/))
			{
				#$summary{$key} = $$subhash{$key};
				if($vals>0)
				{ print ", "; }
				print "$key=$$subhash{$key}";
				$vals++;
			}
		}
		$summary{"time"} = $$subhash{"time"};
#		$summary{"count"} = $$subhash{"count"};
		print "\n";
		
		# Remove from $ret any values that come from @badParents
		foreach my $bad (@$badParents)
		{
			foreach my $key (keys %$subhash)
			{
				if($$subhash{$key} =~ /HASH/)
				{
					my $ret = printVarSum([], $$subhash{$key}, $var, $bad, $key eq $bad, $prefix."$key() ", $indent."....");
					#foreach my $key (keys %summary)
					#{ $summary{$key} -= $$ret{$key}; }
					$summary{"time"} -= $$ret{"time"};
					#$summary{"count"} = $$subhash{"count"};
				}
			}
		}
		#print ";${indent}"; foreach my $key (keys %summary) { print "$key=$summary{$key} "; } print "\n";
		return %summary;
	}
	
	my %summary=();
	#$summary{"time"}=$$subhash{"time"};
	#$summary{"count"}=$$subhash{"count"};
	# Print deeper call stacks
	foreach my $key (keys %$subhash)
	{
		#print "${indent}${prefix} $key => $$subhash{$key}\n";
		#if(!(ref $key eq 'HASH'))
		if($$subhash{$key} =~ /HASH/)
		{
			my $sub_notInclSum=0, $sub_inclSum=0;
			#print "${indent}key = $key\n";
			#printCounter($$subhash{$key}, $prefix."$key() ", $indent."    ");
			#print "#badParents=$#{@$badParents} isInList($key, $badParents)=",isInList($key, $badParents),"\n";
			my %ret;	
			if($#{@$badParents} == -1)
			{
				#($sub_notInclSum, $sub_inclSum) = 
				%ret = printVarSum($badParents, $$subhash{$key}, $var, $func, $key eq $func, $prefix."$key() ", $indent."    ");
				#$inclSum += $sub_inclSum;
			}
			# We're excluding @badParents but $func is not one of them
			elsif(!isInList($key, $badParents))
			{
				%ret = printVarSum($badParents, $$subhash{$key}, $var, $func, $key eq $func, $prefix."$key() ", $indent."    ");
			}
			$summary{"time"} += $ret{"time"};  
			#$summary{"count"} += $ret{"count"};
			
			#if($ret{"count"}>0)
			#{
			#	#foreach my $key (keys %ret)
			#	#{
			#	#	if($key ne "time" && $key ne "count")
			#	#	{ 
			#	#		print "${indent}summary{$key} = $ret{$key}*",$ret{"count"}," (",($ret{$key}*$ret{"count"}),") = ",($summary{$key} + $ret{$key}*$ret{"count"}),"\n";
			#	#		$summary{$key} += $ret{$key}*$ret{"count"}; }
         #   #
			#	#}
			#	#print "${indent}summary{time} = $ret{time} = ",($summary{"time"} + $ret{"time"}),"\n";
			#	$summary{"time"} += $ret{"time"};
			#	$summary{"count"} += $ret{"count"};
			#}
			#print ":${indent}"; foreach my $key (keys %summary) { print "$key=$summary{$key} "; } print "\n";
		}
	}
	#return ($notInclSum, $inclSum);
	#print "${indent}Returning $sum\n";
	return %summary;
}

#sub printVarSum
#{
#	my ($include, $subhash, $var, $func) = @_;
#
#	#my ($notInclSum, $inclSum) = printVarSum_aux($include, $subhash, $var, $func);
#	#if($include)
#	#{ return $inclSum; }
#	#else
#	#{ return $notInclSum; }
#	return printVarSum_aux($include, $subhash, $var, $func);
#}
#
#sub printVarSum_aux
#{
#	my ($include, $subhash, $var, $func, $doExit, $includedFunc, $prefix, $indent) = @_;
#	print "${indent}printVarSum($include, $subhash, $var, $func, $doExit, $prefix, $indent)\n";
#	#print "printCounter() subhash=$subhash\n";
#	#my @hashkeys = keys %$subhash;
#	#print "printCounter() #hashkeys=$#hashkeys\n";
#	
#	my $foundIncl=0;
#	my $foundSubFunc=0;
#	# Check if the function we want to include/exclude exists at this level
#	# Also check if this level has sub-functions
#	foreach my $key (keys %$subhash)
#	{
#		#print "${indent}printCounter() key=$key\n";
#		#if(!(ref $key eq 'HASH'))
#		if($$subhash{$key} =~ /HASH/)
#		{
#			$foundSubFunc=1;
#			if($key eq $func)
#			{
#				$foundIncl=1;
#				last;
#			}
#		}
#	}
#	#print "${indent}include=$include, foundSubFunc=$foundSubFunc, foundIncl=$foundIncl\n";
#	
#	# Print all the variables associated with the current call stack
#	if($doExit || !$foundSubFunc)
#	{
#		my $ret;
#		print "${indent}${prefix} ";
#		my $vals=0;
#		foreach my $key (keys %$subhash)
#		{
#			#print "${indent}printCounter() key=$key\n";
#			#if(!(ref $key eq 'HASH'))
#			if(!($$subhash{$key} =~ /HASH/) && ($key eq $var))
#			{
#				if($vals>0)
#				{ print ", "; }
#				print "$key=$$subhash{$key}";
#				$ret = $$subhash{$key};
#				$vals++;
#			}
#		}
#		print "\n";
#		#if($includedFunc)
#		#{ return (0, $ret); }
#		#else
#		#{ return ($ret, 0); }
#		return $ret;
#	}
#	
#	#my $notInclSum=0, $inclSum=0;
#	my $sum;
#	if($include) { $sum=0; }
#	else { $sum=$$subhash{$var}; }
#	# Print deeper call stacks
#	foreach my $key (keys %$subhash)
#	{
#		#print "${indent}printCounter() key=$key\n";
#		#if(!(ref $key eq 'HASH'))
#		if($$subhash{$key} =~ /HASH/)
#		{
#			my $sub_notInclSum=0, $sub_inclSum=0;
#			#print "${indent}key = $key\n";
#			#printCounter($$subhash{$key}, $prefix."$key() ", $indent."    ");
#			if($include)
#			{ 
#				#($sub_notInclSum, $sub_inclSum) = 
#				$sum += printVarSum_aux($include, $$subhash{$key}, $var, $func, $key eq $func, $key eq $func, $prefix."$key() ", $indent."    ");
#				#$inclSum += $sub_inclSum;
#			}
#			# Exlude $func 
#			else
#			{
#				## If we're at a level where $func appears, print out the immediate sub-level's variables
#				## Otherwise, go one level deeper
#				#if($key ne $func)
#				#{ 
#				#	#($sub_notInclSum, $sub_inclSum) = 
#				#	my $ret = printVarSum_aux($include, $$subhash{$key}, $var, $func, 0, 0, $prefix."$key() ", $indent."    ");;
#				#	print "${indent}sum+=ret($ret)=",($sum+$ret);
#				#	$sum += $ret;
#				#	#print "${indent}$key : A notInclSum += $$subhash{$key}{$var} - $sub_inclSum (",($$subhash{$key}{$var} - $sub_inclSum),") = ",($notInclSum + $$subhash{$key}{$var} - $sub_inclSum),"\n";
#				#	#$notInclSum += $$subhash{$key}{$var} - $sub_inclSum;
#				#	
#				#}
#				#else
#				#{ 
#				#	#($sub_notInclSum, $sub_inclSum) = 
#				#	$sum -= printVarSum_aux(1, $$subhash{$key}, $var, $func, 1, 1, $prefix."$key() ", $indent."    ");
#				#	#print "${indent}$key : B notInclSum += $$subhash{$var} - $sub_inclSum (",($$subhash{$var} - $sub_inclSum),") = ",($notInclSum + $$subhash{$var} - $sub_inclSum),"\n";
#				#	#$notInclSum += $$subhash{$var} - $sub_inclSum;
#				#	#print "${indent}$key : inclSum += $$subhash{$key}{$var} = ",($inclSum + $$subhash{$key}{$var}),"\n";
#				#	#$inclSum += $$subhash{$key}{$var};
#				#}
#				$sum -= printVarSum_aux(1, $$subhash{$key}, $var, $func, $key eq $func, 1, $prefix."$key() ", $indent."    ");
#			}
#		}
#	}
#	#return ($notInclSum, $inclSum);
#	print "${indent}Returning $sum\n";
#	return $sum;
#}


sub printVarWeightedSum
{
	my ($subhash, $var, $func, $doExit, $prefix, $indent) = @_;
	#print "printVarSum($subhash, $var, $func, $doExit, $prefix, $indent)\n";
	#print "printCounter() subhash=$subhash\n";
	#my @hashkeys = keys %$subhash;
	#print "printCounter() #hashkeys=$#hashkeys\n";
	
	# Print all the variables associated with the current call stack
	if($doExit)
	{
		my $ret;
		print "${prefix} ";
		my $vals=0;
		foreach my $key (keys %$subhash)
		{
			#print "${indent}printCounter() key=$key\n";
			#if(!(ref $key eq 'HASH'))
			if(!($$subhash{$key} =~ /HASH/) && ($key eq $var))
			{
				if($vals>0)
				{ print ", "; }
				print "$key=$$subhash{$key}";
				$ret = $$subhash{$key}*$$subhash{"count"};
				$vals++;
			}
		}
		print "\n";
		return $ret;
	}
	
	my $ret = 0;
	# Print deeper call stacks
	foreach my $key (keys %$subhash)
	{
		#print "${indent}printCounter() key=$key\n";
		#if(!(ref $key eq 'HASH'))
		if($$subhash{$key} =~ /HASH/)
		{
			#print "$key ";
			#printCounter($$subhash{$key}, $prefix."$key() ", $indent."    ");
			$ret += printVarWeightedSum($$subhash{$key}, $var, $func, $key eq $func, $prefix."$key() ", $indent."    ");
		}
	}
	return $ret;
}

sub isInList
{
	my ($val, $list) = @_;
	
	#print ":isInList: #list = $#{@$list}\n";
	foreach my $listval (@$list)
	{
		#print ":isInList:val=$val, listval=$listval\n";
		if($val eq $listval)
		{ return 1; }
	}
	return 0;
}