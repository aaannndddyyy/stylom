#!/bin/bash

APP=stylom
PREV_VERSION=1.00
VERSION=1.00
SOURCEDIR=.
ARCH_TYPE=`uname -m`
CURRDIR=`pwd`
SOURCE=~/rpmbuild/SOURCES/${APP}_${VERSION}.orig.tar.gz

#update version numbers automatically - so you don't have to
sed -i 's/VERSION='${PREV_VERSION}'/VERSION='${VERSION}'/g' Makefile debian.sh
sed -i 's/Version: '${PREV_VERSION}'/Version: '${VERSION}'/g' rpmpackage/${APP}.spec
sed -i 's/VERSION '${PREV_VERSION}'/VERSION '${VERSION}'/g' src/${APP}.h 

sudo yum groupinstall "Development Tools"
sudo yum install rpmdevtools gnuplot libpng-devel

# setup the rpmbuild directory tree
rpmdev-setuptree

# create the source code in the SOURCES directory
make clean
mkdir -p ~/rpmbuild/SOURCES
rm -f ${SOURCE}
# having the root directory called name-version seems essential
mv ../${APP} ../${APP}-${VERSION}
tar -cvzf ${SOURCE} ../${APP}-${VERSION} --exclude-vcs
# rename the root directory without the version number
mv ../${APP}-${VERSION} ../${APP}

# copy the spec file into the SPECS directory
cp -f rpmpackage/${APP}.spec ~/rpmbuild/SPECS

# build
cd ~/rpmbuild/SPECS
rpmbuild -ba ${APP}.spec
cd ${CURRDIR}

# Copy the results into the rpmpackage directory
mkdir -p rpmpackage/${ARCH_TYPE}
cp -r ~/rpmbuild/RPMS/${ARCH_TYPE}/${APP}* rpmpackage/${ARCH_TYPE}
cp -r ~/rpmbuild/SRPMS/${APP}* rpmpackage

echo ---------------------------------------------------------
echo RPM files can be found in the rpmpackage directory
echo under architecture subdirectories.
echo ---------------------------------------------------------
