#!/usr/bin/env python

import argparse
import os
import datetime
from os.path import join

TESTPROGRAMS = "/usr/lib/openscad/testprograms"
EXAMPLES = "/usr/share/openscad/examples"
TESTDATA = "/usr/share/openscad/testdata"
REGRESSION = "/usr/share/openscad/regression"

def main():
    p = argparse.ArgumentParser(description="Set up and run the OpenSCAD test suite")
    p.add_argument("-d", "--directory", help="Where to set up the files")
    p.add_argument("-n", "--no-run", help="Just set up the files, don't run", action="store_true")
    p.add_argument("additional", nargs="*", help="remaining arguments will be passed to ctest")

    args = p.parse_args()

    dirname = args.directory or "openscad-test-%s"%datetime.datetime.now().strftime("%Y-%m-%d_%H:%M")
    builddir = join(dirname, "build")

    print "Creating test infrastructure in", dirname

    os.mkdir(dirname)
    os.mkdir(builddir)
    os.symlink(EXAMPLES, join(dirname, "examples"))
    os.symlink(TESTDATA, join(dirname, "testdata"))
    os.symlink(REGRESSION, join(builddir, "regression"))

    for f in os.listdir(TESTPROGRAMS):
        os.symlink(join(TESTPROGRAMS, f), join(builddir, f))

    if args.no_run:
        print "Skipping test run"
    else:
        os.chdir(builddir)
        os.execvp("ctest", ["ctest"] + args.additional)

if __name__ == "__main__":
    main()
