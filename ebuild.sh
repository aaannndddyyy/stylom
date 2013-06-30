#!/bin/bash

APP=stylom
PREV_VERSION=1.00
VERSION=1.00
RELEASE=1
SOURCEDIR=.
ARCH_TYPE=`uname -m`
CURRDIR=`pwd`
SOURCE=~/ebuild/${APP}-${VERSION}.tar.gz


# Update version numbers automatically - so you don't have to
sed -i 's/VERSION='${PREV_VERSION}'/VERSION='${VERSION}'/g' Makefile debian.sh rpm.sh arch.sh puppy.sh
sed -i 's/Version: '${PREV_VERSION}'/Version: '${VERSION}'/g' rpmpackage/${APP}.spec
sed -i 's/Release: '${RELEASE}'/Release: '${RELEASE}'/g' rpmpackage/${APP}.spec
sed -i 's/pkgrel='${RELEASE}'/pkgrel='${RELEASE}'/g' archpackage/PKGBUILD
sed -i 's/pkgver='${PREV_VERSION}'/pkgver='${VERSION}'/g' archpackage/PKGBUILD
sed -i "s/-${PREV_VERSION}-/-${VERSION}-/g" puppypackage/pet.specs
sed -i "s/|${PREV_VERSION}|/|${VERSION}|/g" puppypackage/pet.specs

# create the source code in the SOURCES directory
make clean
mkdir -p ~/ebuild
rm -f ${SOURCE}
mv ../${APP} ../${APP}-${VERSION}
tar -cvzf ${SOURCE} ../${APP}-${VERSION} --exclude-vcs
# rename the root directory without the version number
mv ../${APP}-${VERSION} ../${APP}
