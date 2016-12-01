#/bin/sh!
rm CMakeCache.txt
cmake .
make clean
make -j4
rm sortie/MTS/file* & rm sortie/Particles/* & rm sortie/OBJ/*
./ProcDeferl2

