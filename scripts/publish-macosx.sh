#!/bin/sh

# NB! To build a release build, the VERSION environment variable needs to be set.
# See doc/release-checklist.txt

if test -z "$VERSION"; then
  VERSION=`date "+%Y.%m.%d"`
  COMMIT=-c
  SNAPSHOT=true
fi

# Turn off ccache, just for safety
PATH=${PATH//\/opt\/local\/libexec\/ccache:}

# This is the same location as DEPLOYDIR in macosx-build-dependencies.sh
export OPENSCAD_LIBRARIES=$PWD/../libraries/install

# Make sure that the correct Qt tools are used
export PATH=$OPENSCAD_LIBRARIES/bin:$PATH

`dirname $0`/release-common.sh -v $VERSION $COMMIT
if [[ $? != 0 ]]; then
  exit 1
fi

echo "Sanity check of the app bundle..."
`dirname $0`/macosx-sanity-check.py OpenSCAD.app/Contents/MacOS/OpenSCAD
if [[ $? != 0 ]]; then
  exit 1
fi

echo "Uploading..."
LABELS=OpSys-OSX,Type-Executable
if ! $SNAPSHOT; then LABELS=$LABELS,Featured; fi
`dirname $0`/googlecode_upload.py -s 'Mac OS X Snapshot' -p openscad OpenSCAD-$VERSION.dmg -l $LABELS

# Update snapshot filename on wab page
`dirname $0`/update-web.sh OpenSCAD-$VERSION.dmg
