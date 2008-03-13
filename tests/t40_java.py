# -*- coding: utf-8 -*-
import common
from common import setup_module, teardown_module

input = '''\
Dear =DEorvard=F0ur,

=AB O=F9 qu'il r=E9side, =E0 N=EEmes ou m=EAme Capharna=FCm, tout Fran=E7ai=
s inscrit
au r=F4le payera son d=FB d=E8s avant No=EBl, qu'il soit na=EFf ou r=E2leur=
. =BB
'''

output = '''\
Dear \u00deorvard\u00f0ur,

\u00ab O\u00f9 qu'il r\u00e9side, \u00e0 N\u00eemes ou m\u00eame Capharna\u00fcm, tout Fran\u00e7ais inscrit
au r\u00f4le payera son d\u00fb d\u00e8s avant No\u00ebl, qu'il soit na\u00eff ou r\u00e2leur. \u00bb
'''

class Test:

    def test_1(self):
        # Block of lines to JAVA.
        common.request('l1/qp..java')
        common.validate(input, output)

    def test_2(self):
        # Block of lines to JAVA and back.
        common.request('l1/qp..java')
        common.validate_back(input)
