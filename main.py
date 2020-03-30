#!/usr/bin/env python3

import argparse
import os
import subprocess as sp
import sys
from sys import argv, stdin, stdout, stderr

def call_process(args):
    p = sp.Popen(args, stdin=stdin, stdout=stdout, stderr=stderr)  
    p.communicate()
    if p.returncode: # Error ocurred
        sys.exit(1) 
if __name__ == "__main__":
    p = argparse.ArgumentParser(prog="la")
    p.add_argument("-if", "--ifile", dest="if_", default="file.la")
    p.add_argument("-of", "--ofile", dest="of", default="file.lac")
    p.add_argument("-c", dest="c", action="store_true", default=False)  # compile
    p.add_argument("-r", dest="r", action="store_true", default=False)  # run
    p.add_argument("-g", dest="g", action="store_true", default=False)  # gcc-compile
    p.add_argument("-w", dest="w", action="store_true", default=False)  # Ignore GCC Warnings
    a = p.parse_args(argv[1:])
    print(a)
    os.system("cd %s" % os.path.dirname(os.path.abspath(__file__)))

    if a.c:
        l = ["python3", "la/main.py", a.if_, a.of]
        call_process(l)
    if a.g:
        l = ["gcc", "-x", "c", "file.lac"]
        if a.w:
            l.insert(1, '-w')
        call_process(l)
    if a.r:
        call_process(['./a.out'])
        print()
