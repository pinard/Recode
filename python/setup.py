#!/usr/bin/env python
# -*- coding: utf-8 -*-

from distutils.core import setup
from Pyrex.Distutils import build_ext, Extension

setup(name='Recode', version='3.7',
      description="Conversion between charsets, surfaces and structures.",
      author='François Pinard',
      author_email='pinard@iro.umontreal.ca',
      url='http://recode.progiciels-bpi.ca',
      ext_modules=[Extension("Recode", ["Recode.pyx"])],
      cmdclass={'build_ext': build_ext})
