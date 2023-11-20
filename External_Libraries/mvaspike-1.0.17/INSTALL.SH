#
# This is a simple remainder for a local installation of the software
#

# Installation in the prefix directory

./configure --prefix=$HOME/usr
make 
make install


# Clean all installation files

make clean
/bin/rm -rf ./config.log ./config.status ./libtool
find . \( -name Makefile -o -name .deps \) -exec /bin/rm -rf {} \;
