#!/bin/sh
# Compile matmul kernel

input_file=${1:-'input_matrix.in'}
build_type=${2:-'normal'}

if [[ $build_type = "clean" ]];
then
    echo 'clean build'
    rm -rf bin/
else
    echo 'recursive build'
fi

mkdir -p bin/

echo Compiling Matmul_boostlib
g++ -std=c++11 -Wall -O3 -g src/matmul_boostlib.cpp -o bin/matmul_boostlib.o -pedantic

echo Compiling Matmul_strassenalgo
g++ -std=c++11 -Wall -O3 -g src/matmul_strassenalgo.cpp -o bin/matmul_strassenalgo.o -pedantic

echo Compiling Matmul_ikjalgo
g++ -std=c++11 -Wall -O3 -g src/matmul_ikjalgo.cpp -o bin/matmul_ikjalgo.o -pedantic

echo Compiling Matmul_ijkalgo
g++ -std=c++11 -Wall -O3 -g src/matmul_ijkalgo.cpp -o bin/matmul_ijkalgo.o -pedantic

for entry in bin/*.o
do
  f=$(echo "${entry##*/}");
  kernelname=$(echo $f| cut  -d'.' -f 1);
  echo "Running $kernelname on $input_file"
  $entry --input_file $input_file
done
