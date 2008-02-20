essai: install
	python Essai.py

all:
	python setup.py build_ext \
	  --include-dir=../essai:../src --libraries=recode

install: all
	python setup.py install --skip-build

xx: xx.c
	gcc -o xx -I ../essai -I ../src xx.c -lrecode
