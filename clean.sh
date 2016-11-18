#/bin/sh!
rm CMakeCache.txt
cmake .
make clean
make -j4
./ProcDeferl2

