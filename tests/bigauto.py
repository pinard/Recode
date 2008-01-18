#!/usr/bin/env python
# Copyright © 1997, 1999, 2000 Progiciels Bourbeau-Pinard inc.
# François Pinard <pinard@iro.umontreal.ca>, 1997.

"""\
Produce statistics from the results of the bigauto check.

Usage: bigauto [RECODE_OPTION]... [CHARSET_OPTION]...

This script makes a simple analysis for the connectivity of the various
charsets and produce a report on standard output.  The reports include
statistics about the number of steps before and after optimisation.

The option `-hNAME' would affect the resulting output, because there are
more merging rules when this option is in effect.  Other options affect
the result: `-d', `-g' and, notably, `-s'.

All non-option arguments are interpreted as charset names.  If any is given,
the study is limited to those recodings having any of the given charsets
both as a starting and ending points.  If there is no such non-option
argument, all possible possible recodings are considered.
"""

import os, string, sys

def main(*arguments):
    recode_options = []
    charset_options = []
    for argument in arguments:
        if arguments[0] == '-':
            recode_options.append(argument)
        else:
            charset_options.append(argument)
    work_name = '/tmp/bigauto-data'
    if os.path.exists(work_name):
        os.remove(work_name)
    create_data(work_name, recode_options, charset_options)
    report = Report()
    report.digest_data(open(work_name).readline)
    report.produce_report(sys.stdout.write)
    os.remove(work_name)

def create_data(name, recode_options, charset_options):
    # Get the list of charsets.
    if charset_options:
        charsets = charset_options
    else:
        charsets = []
        for line in os.popen('recode -l').readlines():
            charset = string.split(line)[0]
            if charset[0] in ':/':
                continue
            charsets.append(charset)
    # Do the work, calling a subshell once per `before' value.
    recode_call = 'recode </dev/null -v %s' % string.join(recode_options)
    for before in charsets:
        if before in ('count-characters', 'dump-with-names',
                      'flat', 'Texinfo'):
            continue
        sys.stderr.write("Before charset: %s\n" % before)
        write = os.popen('sh >>%s 2>&1' % name, 'w').write
        write('export LANGUAGE; LANGUAGE=C\n'
              'export LANG; LANG=C\n'
              'export LC_ALL; LC_ALL=C\n')
        for after in charsets:
            if after != before:
                write("%s '%s..%s'\n" % (recode_call, before, after))

class Report:

    def __init__(self):
        self.recode_calls = 0
        self.original_count = {}
        self.original_example = {}
        self.original_total = 0
        self.shrunk_count = {}
        self.shrunk_example = {}
        self.shrunk_total = 0

    def digest_data(self, readline):
        lensep = len(os.linesep)
        line = readline()
        while line:
            type, request = string.split(line[:-lensep], ':', 1)
            if type == 'Request':
                steps = self.get_steps(request)
                self.count_original_request(steps, request)
                line = readline()
                if line:
                    if len(string.split(line[:-lensep], ':', 1)) != 2:
                        print '*', line,
                    type, shrunk_to = string.split(line[:-lensep], ':', 1)
                    if type == 'Shrunk to':
                        steps = self.get_steps(shrunk_to)
                        self.count_shrunk_request(steps, shrunk_to)
                        line = readline()
                    else:
                        self.count_shrunk_request(steps, request)
                else:
                    self.count_shrunk_request(steps, request)
            else:
                sys.stderr.write('Unrecognized line: ' + line)
                line = readline()

    def get_steps(self, text):
        if text == '*mere copy*':
            return 0
        if text[-1] == '/':
            text = text[:-1]
        text = string.replace(text, '/..', '..')
        count = 0
        for fragment in string.split(text, '..'):
            count = count + len(string.split(fragment, '/'))
        return count - 1

    def count_original_request(self, steps, text):
        self.recode_calls = self.recode_calls + 1
        if self.original_count.has_key(steps):
            self.original_count[steps] = self.original_count[steps] + 1
        else:
            self.original_count[steps] = 1
            self.original_example[steps] = string.strip(text)
            if self.original_total == 0:
                self.original_minimum = self.original_maximum = steps
            else:
                if steps < self.original_minimum:
                    self.original_minimum = steps
                if steps > self.original_maximum:
                    self.original_maximum = steps
        self.original_total = self.original_total + steps

    def count_shrunk_request(self, steps, text):
        if self.shrunk_count.has_key(steps):
            self.shrunk_count[steps] = self.shrunk_count[steps] + 1
        else:
            self.shrunk_count[steps] = 1
            self.shrunk_example[steps] = string.strip(text)
            if self.shrunk_total == 0:
                self.shrunk_minimum = self.shrunk_maximum = steps
            else:
                if steps < self.shrunk_minimum:
                    self.shrunk_minimum = steps
                if steps > self.shrunk_maximum:
                    self.shrunk_maximum = steps
        self.shrunk_total = self.shrunk_total + steps

    def produce_report(self, write):
        if self.recode_calls == 0:
            sys.stderr.write("No call to report\n")
            return
        write("\n"
              "Optimisation     Original  Shrunk\n"
              "              .-------------------\n"
              "Minimum       |  %2d        %2d\n"
              "Maximum       |  %2d        %2d\n"
              "Average       |  %4.1f      %4.1f\n"
              % (self.original_minimum, self.shrunk_minimum,
                 self.original_maximum, self.shrunk_maximum,
                 float(self.original_total) / float(self.recode_calls),
                 float(self.shrunk_total) / float(self.recode_calls)))
        write("\n"
              "Histogram for original requests\n")
        for steps in range(self.original_minimum, self.original_maximum+1):
            if self.original_count.has_key(steps):
                write("%5d steps, %5d times  %s\n"
                      % (steps, self.original_count[steps],
                         self.original_example[steps]))
        write("\n"
              "Histogram for shrunk requests\n")
        for steps in range(self.shrunk_minimum, self.shrunk_maximum+1):
            if self.shrunk_count.has_key(steps):
                write("%5d steps, %5d times  %s\n"
                      % (steps, self.shrunk_count[steps],
                         self.shrunk_example[steps]))

if __name__ == '__main__':
    apply(main, tuple(sys.argv[1:]))
