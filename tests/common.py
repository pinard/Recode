# Common code for testing the Recode C library.
# Copyright © 1996-2000, 2008 Free Software Foundation, Inc.
# François Pinard <pinard@iro.umontreal.ca>, 1988.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.


import os
from __main__ import py

recode_program = os.environ.get('RECODE')

import Recode
outer = Recode.Outer(iconv=False)
outer_iconv = Recode.Outer(iconv=True)

class Run(dict):

    def __getattr__(self, attribute):
        return self[attribute]

    def __setattr__(self, attribute, value):
        self[attribute] = value

run = Run()

def external(flag):
    run.external = flag

def request(text):
    run.request = text

# Functions only meant to be imported into real testing modules, where
# pytest is meant to find and use them.

def setup_module(module):
    for variable in ('LANG', 'LANGUAGE',
                     'LC_ALL', 'LC_MESSAGES', 'LC_COLLATE'):
        if variable in os.environ:
            del os.environ[variable]
    run.external = outer is None
    import tempfile
    run.work = tempfile.mktemp()

def teardown_module(module):
    if os.path.exists(run.work):
        os.remove(run.work)

# Useful functions for various test modules.

def assert_or_diff(output, expected):
    if output != expected:
        import difflib
        diff = difflib.ndiff(output.splitlines(True),
                             expected.splitlines(True))
        import sys
        sys.stdout.write(''.join(diff))
        assert False, (len(output), len(expected))

def external_output(command):
    if not recode_program:
        py.test.skip()
    command = command.replace('$R', recode_program + ' --ignore=:iconv:')
    return os.popen(command).read()

def recode_output(input):
    if run.external:
        open(run.work, 'w').write(input)
        return external_output('$R %s < %s' % (run.request, run.work))
    if outer is None:
        py.test.skip()
    return outer.recode(run.request, input)

def recode_iconv_output(input):
    if run.external or outer_iconv is None:
        py.test.skip()
    return outer_iconv.recode(run.request, input)

def recode_back_output(input):
    before, after = run.request.split('..')
    if run.external:
        open(run.work, 'w').write(input)
        external_output('$R %s %s' % (run.request, run.work))
        return external_output('$R %s..%s < %s' % (after, before, run.work))
    if outer is None:
        py.test.skip()
    temp = outer.recode(run.request, input)
    return outer.recode('%s..%s' % (after, before), temp)

def validate(input, expected):
    output = recode_output(input)
    assert_or_diff(output, expected)

def validate_back(input):
    output = recode_back_output(input)
    assert_or_diff(output, input)
