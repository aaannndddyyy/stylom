#!/bin/bash

APP=stylom
PREV_VERSION=1.00
VERSION=1.00
RELEASE=1
BUILDDIR=~/petbuild
CURRDIR=`pwd`
PROJECTDIR=${BUILDDIR}/${APP}-${VERSION}-${RELEASE}

# Update version numbers automatically - so you don't have to
sed -i 's/VERSION='${PREV_VERSION}'/VERSION='${VERSION}'/g' Makefile debian.sh rpm.sh arch.sh ebuild.sh
sed -i 's/Version: '${PREV_VERSION}'/Version: '${VERSION}'/g' rpmpackage/${APP}.spec
sed -i 's/Release: '${RELEASE}'/Release: '${RELEASE}'/g' rpmpackage/${APP}.spec
sed -i 's/pkgrel='${RELEASE}'/pkgrel='${RELEASE}'/g' archpackage/PKGBUILD
sed -i 's/pkgver='${PREV_VERSION}'/pkgver='${VERSION}'/g' archpackage/PKGBUILD
sed -i "s/-${PREV_VERSION}-/-${VERSION}-/g" puppypackage/pet.specs
sed -i "s/|${PREV_VERSION}|/|${VERSION}|/g" puppypackage/pet.specs


# Make directories within which the project will be built
mkdir -p ${BUILDDIR}
mkdir -p ${PROJECTDIR}

# Build the project
make clean
make
make install -B DESTDIR=${PROJECTDIR}

# create directories specific to puppy
mkdir ${PROJECTDIR}/usr
mkdir ${PROJECTDIR}/usr/local
mkdir ${PROJECTDIR}/usr/local/bin

# copy anything in /usr/bin into /usr/local/bin
cp ${PROJECTDIR}/usr/bin/* ${PROJECTDIR}/usr/local/bin/

# Copy the spec file into the build directory
cp puppypackage/stylom-1.00.pet.specs ${PROJECTDIR}

# Compress the build directory
cd ${BUILDDIR}
tar -c -f {APP}-${VERSION}-${RELEASE}.tar .
sync
gzip {APP}-${VERSION}-${RELEASE}.tar
mv {APP}-${VERSION}-${RELEASE}.tar.gz ${CURRDIR}/puppypackage
cd ${CURRDIR}/puppypackage

# Create the PET package
MD5SUM="`md5sum {APP}-${VERSION}-${RELEASE}.tar.gz | cut -f 1 -d ' '`"
echo -n "$MD5SUM" >> {APP}-${VERSION}-${RELEASE}.tar.gz
sync
mv -f {APP}-${VERSION}-${RELEASE}.tar.gz ${APP}-${VERSION}-${RELEASE}.pet
sync
cd ${CURRDIR}

# Remove the temporary build directory
rm -fr ${BUILDDIR}
