# -*- coding: utf-8 -*-
import common
from common import setup_module, teardown_module

import os, sys
input_name = '%s/../COPYING' % os.path.dirname(sys.argv[0])
input = open(input_name).read()

def test_1():

    # No step at all.
    yield validate, 'texte..texte', 'memory', 'filter'
    yield validate, 'texte..texte', 'memory', 'squash'
    yield validate, 'texte..texte', 'files', 'filter'
    yield validate, 'texte..texte', 'files', 'squash'
    yield validate, 'texte..texte', 'pipe', 'filter'
    yield validate, 'texte..texte', 'pipe', 'squash'

    # One single step.
    yield validate, 'latin1..ibmpc/', 'memory', 'filter'
    yield validate, 'latin1..ibmpc/', 'memory', 'squash'
    yield validate, 'latin1..ibmpc/', 'files', 'filter'
    yield validate, 'latin1..ibmpc/', 'files', 'squash'
    yield validate, 'latin1..ibmpc/', 'pipe', 'filter'
    yield validate, 'latin1..ibmpc/', 'pipe', 'squash'

    # One single step and a surface
    yield validate, 'latin1..ibmpc', 'memory', 'filter'
    yield validate, 'latin1..ibmpc', 'memory', 'squash'
    yield validate, 'latin1..ibmpc', 'files', 'filter'
    yield validate, 'latin1..ibmpc', 'files', 'squash'
    yield validate, 'latin1..ibmpc', 'pipe', 'filter'
    yield validate, 'latin1..ibmpc', 'pipe', 'squash'

    # One single step.
    yield validate, 'texte..latin1', 'memory', 'filter'
    yield validate, 'texte..latin1', 'memory', 'squash'
    yield validate, 'texte..latin1', 'files', 'filter'
    yield validate, 'texte..latin1', 'files', 'squash'
    yield validate, 'texte..latin1', 'pipe', 'filter'
    yield validate, 'texte..latin1', 'pipe', 'squash'

    # Two single steps.
    yield validate, 'texte..bangbang', 'memory', 'filter'
    yield validate, 'texte..bangbang', 'memory', 'squash'
    yield validate, 'texte..bangbang', 'files', 'filter'
    yield validate, 'texte..bangbang', 'files', 'squash'
    yield validate, 'texte..bangbang', 'pipe', 'filter'
    yield validate, 'texte..bangbang', 'pipe', 'squash'

    # Two single steps and a surface.
    yield validate, 'texte..ibmpc', 'memory', 'filter'
    yield validate, 'texte..ibmpc', 'memory', 'squash'
    yield validate, 'texte..ibmpc', 'files', 'filter'
    yield validate, 'texte..ibmpc', 'files', 'squash'
    yield validate, 'texte..ibmpc', 'pipe', 'filter'
    yield validate, 'texte..ibmpc', 'pipe', 'squash'

    # Three single steps.
    yield validate, 'texte..iconqnx', 'memory', 'filter'
    yield validate, 'texte..iconqnx', 'memory', 'squash'
    yield validate, 'texte..iconqnx', 'files', 'filter'
    yield validate, 'texte..iconqnx', 'files', 'squash'
    yield validate, 'texte..iconqnx', 'pipe', 'filter'
    yield validate, 'texte..iconqnx', 'pipe', 'squash'

    # Four single steps, optimized into three (with iconv) or two (without).
    yield validate, 'ascii-bs..ebcdic', 'memory', 'filter'
    yield validate, 'ascii-bs..ebcdic', 'memory', 'squash'
    yield validate, 'ascii-bs..ebcdic', 'files', 'filter'
    yield validate, 'ascii-bs..ebcdic', 'files', 'squash'
    yield validate, 'ascii-bs..ebcdic', 'pipe', 'filter'
    yield validate, 'ascii-bs..ebcdic', 'pipe', 'squash'

def validate(request, sequence, mode):
    before, after = request.split('..')
    if mode == 'filter':
        command = ('$R --quiet --force --sequence=%s < %s %s'
                   '| $R --quiet --force --sequence=%s %s..%s'
                   % (sequence, input_name, request, sequence, after, before))
        output = common.external_output(command)
    elif mode == 'squash':
        open(common.run.work, 'w').write(input)
        command1 = ('$R --quiet --force --sequence=%s %s %s'
                    % (sequence, request, common.run.work))
        command2 = ('$R --quiet --force --sequence=%s %s..%s %s'
                    % (sequence, after, before, common.run.work))
        common.external_output(command1)
        common.external_output(command2)
        output = open(common.run.work).read()
    else:
        assert False, mode
    common.assert_or_diff(output, input)
