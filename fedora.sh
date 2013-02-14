# Fedora Compilation and packaging script

APP=stylom
VERSION=1.00
SOURCEDIR=.
ARCH_TYPE=`uname -m`

sudo yum groupinstall "Development Tools"
sudo yum install rpmdevtools gnuplot libpng-devel

if [ ${ARCH_TYPE} == "x86_64" ]
then
    LIB=lib64
else
    LIB=lib
fi

if [ $# -ge 1 -a "$1" == "--debug" ]
then
    CFLAGS=-g
else
    CFLAGS=-O2 
fi

INCLUDES="-I/usr/include"

LIBS="-L/usr/${LIB}"

LIBS2=""

make clean
make

rm -rf ~/rpmbuild
sudo rm -rf rpmpackage/$APP-$VERSION
rpmdev-setuptree
mkdir rpmpackage/$APP-$VERSION

mkdir rpmpackage/$APP-$VERSION/usr
mkdir rpmpackage/$APP-$VERSION/usr/bin
mkdir rpmpackage/$APP-$VERSION/usr/share
mkdir rpmpackage/$APP-$VERSION/usr/share/man
mkdir rpmpackage/$APP-$VERSION/usr/share/man/man1
install -m 755 $APP rpmpackage/$APP-$VERSION/usr/bin
install -m 755 man/$APP.1.gz rpmpackage/$APP-$VERSION/usr/share/man/man1

cd rpmpackage
mkdir $APP-$VERSION/etc
mkdir $APP-$VERSION/etc/$APP
install -m 644 $APP.conf $APP-$VERSION/etc/$APP/
tar -zcvf $APP-$VERSION.tar.gz $APP-$VERSION/

rm -rf ~/rpmbuild/BUILD/$APP-$VERSION
rm ~/rpmbuild/SOURCES/$APP*.*
cp $APP-$VERSION.tar.gz ~/rpmbuild/SOURCES/
cp $APP.spec ~/rpmbuild/SPECS/

rpmbuild -ba ~/rpmbuild/SPECS/$APP.spec

sudo rm -rf $APP-$VERSION
rm $APP-$VERSION.tar.gz
cp -r ~/rpmbuild/RPMS/* .
cd ..
echo ---------------------------------------------------------
echo RPM files can be found in the rpmpackage directory
echo under architecture subdirectories.
echo ---------------------------------------------------------
