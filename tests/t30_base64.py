# -*- coding: utf-8 -*-
import common
from common import setup_module, teardown_module

class Test:

    inputs = [
        '\n',
        'a\n',
        'ab\n',
        'abc\n',
        'abcd\n',
        'abcdefghi\n',
        'abcdefghijklmnopqrs\n',
        'abcdefghijklmnopqrstuvwzyzABC\n',
        'abcdefghijklmnopqrstuvwzyzABCDEFGHIJKLM\n',
        'abcdefghijklmnopqrstuvwzyzABCDEFGHIJKLMNOPQRSTUVW\n',
        'abcdefghijklmnopqrstuvwzyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456\n',
    ]

    def test_1(self):
        # Single lines to Base64.
        common.request('../64')
        output = ['''\
Cg==
''', '''\
YQo=
''', '''\
YWIK
''', '''\
YWJjCg==
''', '''\
YWJjZAo=
''', '''\
YWJjZGVmZ2hpCg==
''', '''\
YWJjZGVmZ2hpamtsbW5vcHFycwo=
''', '''\
YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd6eXpBQkMK
''', '''\
YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd6eXpBQkNERUZHSElKS0xNCg==
''', '''\
YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd6eXpBQkNERUZHSElKS0xNTk9QUVJTVFVWVwo=
''', '''\
YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd6eXpBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWjAxMjM0
NTYK
'''
                  ]
        for input, output in zip(self.inputs, output):
            common.validate(input, output)

    def test_2(self):
        # Block of lines to Base64.
        common.request('/../64')
        output = '''\
CmEKYWIKYWJjCmFiY2QKYWJjZGVmZ2hpCmFiY2RlZmdoaWprbG1ub3BxcnMKYWJjZGVmZ2hpamts
bW5vcHFyc3R1dnd6eXpBQkMKYWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd6eXpBQkNERUZHSElKS0xN
CmFiY2RlZmdoaWprbG1ub3BxcnN0dXZ3enl6QUJDREVGR0hJSktMTU5PUFFSU1RVVlcKYWJjZGVm
Z2hpamtsbW5vcHFyc3R1dnd6eXpBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWjAxMjM0NTYK
'''
        common.validate(''.join(self.inputs), output)

    def test_3(self):
        # Single lines to Base64 and back.
        common.request('/../64')
        for input in self.inputs:
            common.validate_back(input)

    def test_4(self):
        # Block of lines to Base64 and back.
        common.request('/../64')
        common.validate_back(''.join(self.inputs))
