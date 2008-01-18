# Produce statistics from the results of the bigauto check.
# Copyright © 1997 Progiciels Bourbeau-Pinard inc.
# François Pinard <pinard@iro.umontreal.ca>, 1997.

# This script makes a simple analysis for the connectivity of
# the various charsets and produce a report on standard output.
# The reports include statistics about the number of steps before
# and after optimisation.

# The option @code{-h@var{name}} would affect the resulting output,
# because there are more merging rules when this option is in effect.
# Other options affect the result: @code{-d}, @code{-g} and,
# notably, @code{-s}.  These options may be given as arguments to
# `bigauto.sh'.

# FIXME: There might be one non-option argument, in which case it
# could be interpreted as a charset name, possibly abbreviated to
# any non ambiguous prefix.  @code{recode} would then study all
# recodings having the given charset as a starting or ending point.
# If there is no such non-option argument, @code{recode} would
# study @emph{all} possible recodings.

$before_minimum = 1000;
$after_minimum = 1000;

$_ = <>;
while ($_)
{
    if (/^Request: (.*)/)
    {
	$_ = $1;
	&study_sequence;
	$sequence = $_;
	&before_optimisation;

	$_ = <>;
	if (/^Shrunk to: (.*)/)
	{
	    $_ = $1;
	    &study_sequence;
	    $sequence = $_;
	    &after_optimisation;

	    $_ = <>;
	}
	else
	{
	    &after_optimisation;
	}
    }
    else
    {
	warn "Line $. not recognised: $_";
	$_ = <>;
    }
}
&produce_report;
exit 0;

# ----------------------------------------------------------------------

sub study_sequence
{
    if ($_ eq '*mere copy*')
    {
	$steps = 0;
    }
    else
    {
	s/\/$//;
	s/\/\.\./../g;
	$steps = (split /\/|\.\./) - 1;
    }
}

# ----------------------------------------------------------------------

sub before_optimisation
{
    $recode_calls++;

    $before_minimum = $steps if $steps < $before_minimum;
    $before_maximum = $steps if $steps > $before_maximum;
    $before_total += $steps;

    $before_example[$steps] = $sequence if $before_count[$steps] == 0;
    $before_count[$steps]++;
}

# ----------------------------------------------------------------------

sub after_optimisation
{
    $after_minimum = $steps if $steps < $after_minimum;
    $after_maximum = $steps if $steps > $after_maximum;
    $after_total += $steps;

    $after_example[$steps] = $sequence if $after_count[$steps] == 0;
    $after_count[$steps]++;
}

# ----------------------------------------------------------------------

sub produce_report
{
    local $before_mean, $after_mean, $steps;

    if ($recode_calls == 0)
    {
	warn "No call to report\n";
	return;
    }

    $before_mean = $before_total / $recode_calls;
    $after_mean = $after_total / $recode_calls;

    print "\n";
    print "Optimisation     Before    After\n";
    print "              .-------------------\n";
    printf "Minimum       |  %2d        %2d\n", $before_minimum, $after_minimum;
    printf "Maximum       |  %2d        %2d\n", $before_maximum, $after_maximum;
    printf "Average       |  %4.1f      %4.1f\n", $before_mean, $after_mean;

    print "\n";
    print "\`Before' histogram\n";
    for ($steps = $before_minimum; $steps <= $before_maximum; $steps++)
    {
	if ($before_count[$steps])
	{
	    printf "%5d steps, %5d times  (%s)\n",
	    $steps, $before_count[$steps], $before_example[$steps];
	}
    }

    print "\n";
    print "\`After' histogram\n";
    for ($steps = $after_minimum; $steps <= $after_maximum; $steps++)
    {
	if ($after_count[$steps])
	{
	    printf "%5d steps, %5d times  (%s)\n",
	    $steps, $after_count[$steps], $after_example[$steps];
	}
    }
}
