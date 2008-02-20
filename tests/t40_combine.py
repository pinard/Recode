# -*- coding: utf-8 -*-
import common
from common import setup_module, teardown_module

def test_1():
    # That combine does not crash.
    common.request('co..l1')
    common.validate('', '')
