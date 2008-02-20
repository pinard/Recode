# -*- coding: utf-8 -*-

__metaclass__ = type
import os
from __main__ import SkipTest, raises

try:
    import Recode
except ImportError:
    # The Python API has not been installed.
    Recode = None

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
    run.external = Recode is None
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
    return os.popen(command.replace('$R', 'recode'), 'rb').read()

def recode_output(input):
    if run.external:
        file(run.work, 'wb').write(input)
        return external_output('$R %s < %s' % (run.request, run.work))
    if Recode is None:
        raise SkipTest
    return Recode.recode(run.request, input)

def recode_back_output(input):
    before, after = run.request.split('..')
    if run.external:
        file(run.work, 'wb').write(input)
        external_output('$R %s %s' % (run.request, run.work))
        return external_output('$R %s..%s < %s' % (after, before, run.work))
    if Recode is None:
        raise SkipTest
    temp = Recode.recode(run.request, input)
    return Recode.recode('%s..%s' % (after, before), temp)

def validate(input, expected):
    output = recode_output(input)
    assert_or_diff(output, expected)

def validate_back(input):
    output = recode_back_output(input)
    assert_or_diff(output, input)
