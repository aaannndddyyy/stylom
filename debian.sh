#!/bin/bash

APP=stylom
PREV_VERSION=1.00
VERSION=1.00
ARCH_TYPE=`uname -m`
DIR=${APP}-${VERSION}

#update version numbers automatically - so you don't have to
sed -i 's/VERSION='${PREV_VERSION}'/VERSION='${VERSION}'/g' Makefile rpm.sh
sed -i 's/Version: '${PREV_VERSION}'/Version: '${VERSION}'/g' rpmpackage/${APP}.spec
sed -i 's/VERSION '${PREV_VERSION}'/VERSION '${VERSION}'/g' src/${APP}.h 

# change the parent directory name to debian format
mv ../${APP} ../${DIR}

# Create a source archive
make clean
make source

# Build the package
dpkg-buildpackage -F

# sign files
gpg -ba ../${APP}_${VERSION}-1_${ARCH_TYPE}.deb
gpg -ba ../${APP}_${VERSION}.orig.tar.gz

# restore the parent directory name
mv ../${DIR} ../${APP}
