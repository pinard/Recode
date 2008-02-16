#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright © 2007 Progiciels Bourbeau-Pinard inc.
# François Pinard <pinard@iro.umontreal.ca>, 2008.

"""\
After "git clone" or "git checkout", file timestamps may be a bit random,
and unless you have all maintainer tools handy, it can put you in
some misery.  This script makes all timestamps to be identical.
"""

__metaclass__ = type
import os, sys

class Main:

    def main(self, *arguments):
        # Decode arguments.  Should be none!
        import getopt
        options, arguments = getopt.getopt(arguments, '')
        for option, valeur in options:
            pass
        assert not arguments
        # Use a reference file.
        if not os.path.exists('configure.ac'):
            sys.exit(
                "You should first cd to the distribution directory first.")
        timestamp = os.path.getmtime('configure.ac')
        # Walk all files, changing their timestamp.
        stack = ['.']
        while stack:
            directory = stack.pop()
            for base in os.listdir(directory):
                if base == '.git':
                    continue
                name = os.path.join(directory, base)
                if os.path.isdir(name):
                    stack.append(name)
                elif os.path.isfile(name):
                    os.utime(name, (os.path.getatime(name), timestamp))

run = Main()
main = run.main

if __name__ == '__main__':
    main(*sys.argv[1:])
